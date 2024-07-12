#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include "headers/ppu.h"
#include "headers/cycles.h"

#define IF_ADDR 0xff0f
#define LCDC_ADDR 0xff40
#define STAT_ADDR 0xff41
#define LY_ADDR 0xff44
#define LYC_ADDR 0xff45
#define SCY_ADDR 0xff42
#define SCX_ADDR 0xff43
#define WY_ADDR 0xff4a
#define WX_ADDR 0xff4b
#define OAM_ADDR 0xfe00
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define TILE_SIZE 16
#define TILEMAP_BLOCK_0 0x9800
#define TILEMAP_BLOCK_1 0x9C00
#define TILEDATA_BLOCK_0 0x9000
#define TILEDATA_BLOCK_1 0x8000
#define OBJ_SIZE_BASIC 8
#define OBJ_SIZE_EXTENDED 16
#define MAX_SPRITES_PER_LINE 10

//LCDC bit mask
#define LCDC_PPU_ENABLED_BIT 		0b10000000
#define LCDC_WINDOW_TILEMAP_BIT		0b01000000
#define LCDC_WINDOW_ENABLED_BIT		0b00100000
#define LCDC_BG_AND_WINDOW_ADDR_BIT	0b00010000
#define LCDC_BG_TILEMAP_BIT			0b00001000
#define LCDC_OBJ_SIZE_BIT			0b00000100
#define LCDC_OBJ_ENABLED_BIT		0b00000010
#define LCDC_PRIORITY_BIT			0b00000001

#define VBLANK_INTERRUPT_BIT 0b00001
#define STAT_INTERRUPT_BIT   0b00010

#define MODE_0 0
#define MODE_1 1
#define MODE_2 2
#define MODE_3 3

#define DOTS_PER_CYCLE 4
#define DOTS_PER_SCANLINE 456

PPU::PPU(){
	this->pixels = (uint32_t *)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
}

PPU::~PPU(){
	free(this->pixels);
}

void PPU::draw_pixel(uint8_t pixel_value, uint16_t x, uint16_t y, bool priority, bool is_sprite){
	if(priority && (this->pixels[x + (y * SCREEN_WIDTH)] != this->colors[0]) ){
		return;
	}
	if(!is_sprite){
		uint8_t order = this->memory[0xff47];
		uint8_t palette_value = (order >> (2 * pixel_value)) & 0x3;
		this->pixels[x + (y * SCREEN_WIDTH)] = this->colors[palette_value];
		return;
	}
	if(pixel_value != 0){
		this->pixels[x + (y * SCREEN_WIDTH)] = this->colors[pixel_value];
	}
}

uint8_t PPU::get_color_id(uint8_t tile_index, uint8_t tile_pixel_x, uint8_t tile_pixel_y){
	uint16_t tile_addr = this->tiledata_addr + (tile_index * TILE_SIZE);
	if(this->tiledata_addr == TILEDATA_BLOCK_0){
		tile_addr = this->tiledata_addr + (int16_t(int8_t(tile_index)) * TILE_SIZE);
	}
	uint8_t tile_row_byte_1 = this->memory[tile_addr + (tile_pixel_y * 2)];
	uint8_t tile_row_byte_2 = this->memory[tile_addr + (tile_pixel_y * 2) + 1];
	uint8_t color_id = (((tile_row_byte_2 >> (7-tile_pixel_x)) & 1) << 1) + ((tile_row_byte_1 >> (7-tile_pixel_x)) & 1);
	return color_id;
}

void PPU::update_background(){
	uint8_t SCX = this->memory[SCX_ADDR];
	uint8_t SCY = this->memory[SCY_ADDR];

	uint8_t tile_pixel_y = (SCY + this->current_scanline) % 8;
	uint8_t tile_y = ((SCY + this->current_scanline) / 8) % 32;
	for(uint16_t pixel_i = 0; pixel_i < SCREEN_WIDTH; pixel_i++){
		uint8_t tile_x = ((SCX + pixel_i) / 8) % 32;
		uint8_t tile_pixel_x = (SCX + pixel_i) % 8;
		uint16_t tile_map_index = tile_x + (tile_y * 32);
        uint8_t tile_index = this->memory[this->tilemap_addr + tile_map_index];
		uint8_t color_id = this->get_color_id(tile_index, tile_pixel_x, tile_pixel_y);
		this->draw_pixel(color_id, pixel_i, this->current_scanline, false, false);
    }
}

void PPU::update_window(){
	if(!this->is_window_enabled){
		return;
	}
	if(this->frame_WX > 166 || this->frame_WY > 143){
		return;
	}
	uint16_t WX = this->frame_WX - 7;
	if(current_scanline < this->frame_WY){
		return;
	}
	uint8_t tile_pixel_y = this->window_screenline_aux % 8;
	uint8_t tile_y = ((this->window_screenline_aux) / 8) % 32;
	uint16_t pixel_i_aux = 0;
	for(uint16_t pixel_i = WX; pixel_i < 160; pixel_i++){
		uint8_t tile_x = ((pixel_i_aux) / 8) % 32;
		uint8_t tile_pixel_x = (pixel_i_aux) % 8;	
		uint16_t tile_map_index = tile_x + (tile_y * 32);
   	    uint8_t tile_index = this->memory[this->window_tilemap_addr + tile_map_index];
		uint8_t color_id = this->get_color_id(tile_index, tile_pixel_x, tile_pixel_y);
		this->draw_pixel(color_id, pixel_i, this->current_scanline, false, false);
		pixel_i_aux++;
  	}
	this->window_screenline_aux++;
}

struct SpriteData PPU::get_SpriteData(uint8_t sprite_index){
	SpriteData sprite_data;
	uint8_t offset = sprite_index*4;
	sprite_data.y = this->memory[OAM_ADDR+offset];
	sprite_data.x = this->memory[OAM_ADDR+offset+1];
	sprite_data.tile_index = this->memory[OAM_ADDR+offset+2];
	uint8_t attributes = this->memory[OAM_ADDR+offset+3];
	sprite_data.oam_mode = (attributes & 0x80) >> 7;
	sprite_data.y_flip = (attributes & 0x40) >> 6;
	sprite_data.x_flip = (attributes & 0x20) >> 5;
	sprite_data.x_s = int16_t(sprite_data.x) - 8;
	sprite_data.y_s = int16_t(sprite_data.y) - 16;
	return sprite_data;
}

void PPU::draw_sprite_row(struct SpriteData sprite_data, uint8_t sprite_row){
	uint16_t tile_addr = TILEDATA_BLOCK_1 + (sprite_data.tile_index * TILE_SIZE);
	uint8_t offset = 2 * (sprite_data.y_flip ? 8 - sprite_row : sprite_row);
	uint8_t byte_1 = this->memory[tile_addr + offset];
    uint8_t byte_2 = this->memory[tile_addr + offset + 1];
    for(int8_t bit_pos = 7; bit_pos >= 0; bit_pos--){
		int32_t x_coord_pixel = sprite_data.x_s + (sprite_data.x_flip ? bit_pos : (7-bit_pos) );
		if(x_coord_pixel >= 160 || x_coord_pixel < 0){
			continue;
		}
		uint8_t color_id = (((byte_2 >> bit_pos) & 1) << 1) + ((byte_1 >> bit_pos) & 1);
    	this->draw_pixel(color_id, x_coord_pixel, this->current_scanline, sprite_data.oam_mode, true);
    }
}

void PPU::update_sprites(){
	if(!this->is_obj_enabled){
		return;
	}
	uint8_t sprites_in_line = 0;
	for(uint8_t sprite_i = 0; sprite_i < 40; sprite_i++){
		if(sprites_in_line == MAX_SPRITES_PER_LINE){
			break;
		}
		SpriteData sprite_data = this->get_SpriteData(sprite_i);
		// sprite is unbounded to be seen
		if( (sprite_data.x >= 168) || (sprite_data.x == 0) || (sprite_data.y >= 160) || (sprite_data.y == 0) ){
			continue;
		}
		// if no sprite pixel is inside the scanline, continue
		if( (current_scanline < sprite_data.y_s) || (current_scanline >= (sprite_data.y_s + this->obj_size)) ){
			continue;
		}
		uint8_t sprite_row = this->current_scanline - sprite_data.y_s;
		if(this->obj_size == 16){
			sprite_data.tile_index = sprite_row < OBJ_SIZE_BASIC ? sprite_data.tile_index & 0xfe : sprite_data.tile_index | 0x01;
			sprite_row %= 8;
		}
		this->draw_sprite_row(sprite_data, sprite_row);
		sprites_in_line++;
	}
}

void PPU::update_lcdc_variables(){
	uint8_t lcdc = this->memory[LCDC_ADDR];
	if(lcdc == this->last_lcdc){ // if nothing changed, then dont bother in updating
		return;
	}
	this->last_lcdc = lcdc;
	this->is_enabled = lcdc & LCDC_PPU_ENABLED_BIT;	
	this->window_tilemap_addr = lcdc & LCDC_WINDOW_TILEMAP_BIT ? TILEMAP_BLOCK_1 : TILEMAP_BLOCK_0;
	this->is_window_enabled = lcdc & LCDC_WINDOW_ENABLED_BIT;
	this->tiledata_addr = lcdc & LCDC_BG_AND_WINDOW_ADDR_BIT ? TILEDATA_BLOCK_1 : TILEDATA_BLOCK_0;
	this->tilemap_addr = lcdc & LCDC_BG_TILEMAP_BIT ? TILEMAP_BLOCK_1 : TILEMAP_BLOCK_0;
	this->obj_size = lcdc & LCDC_OBJ_SIZE_BIT ? OBJ_SIZE_EXTENDED : OBJ_SIZE_BASIC;
	this->is_obj_enabled = lcdc & LCDC_OBJ_ENABLED_BIT;
	this->priority = lcdc & LCDC_PRIORITY_BIT;
}

uint8_t get_mode(int clock_ticks, uint16_t scanline){
	if(scanline >= 144){
		return MODE_1;
	}

	uint16_t dots_in_scanline = clock_ticks % DOTS_PER_SCANLINE;
	if(dots_in_scanline <= 80){
		return MODE_2;
	}

	if(dots_in_scanline <= 252){
		return MODE_3;
	}

	return MODE_0;
}

void PPU::handle_interrupts(){
	bool lyc_eq_ly = (this->current_scanline == this->memory[LYC_ADDR]);

	// updating STAT register
	this->memory[STAT_ADDR] &= 0x78;
	this->memory[STAT_ADDR] |= lyc_eq_ly << 2;
	this->memory[STAT_ADDR] |= this->current_mode;

	// STAT interrupt requested
	uint8_t interrupts_states = (lyc_eq_ly << 3) + ((0x1 << this->current_mode) & 0b111 ); // the and operation just discards the mode 3
	uint8_t stat_interrupts_selects = this->memory[STAT_ADDR] >> 3;
	if((interrupts_states & stat_interrupts_selects) > 0){ // Stat interrupt can trigger
		this->memory[IF_ADDR] |= STAT_INTERRUPT_BIT;
	}

	// VBLANK interrupt requested
	if(this->current_scanline == 144){
		this->memory[IF_ADDR] |= VBLANK_INTERRUPT_BIT;
		this->frame_ready = true;
	}
}

void PPU::tick(uint16_t cpu_cycles_index){
	this->update_lcdc_variables();
	if(!this->is_enabled){
		return;
	}
	this->frame_ready = false;

	this->clock += CYCLES[cpu_cycles_index];// * DOTS_PER_CYCLE;
	uint8_t scanline = this->clock / DOTS_PER_SCANLINE;
	this->memory[LY_ADDR] = scanline;
	uint8_t previous_mode = this->current_mode;
	this->current_mode = get_mode(this->clock, scanline);

	if(this->current_scanline != scanline){
		this->current_scanline = scanline;
		this->handle_interrupts();
		if(this->current_scanline == 0){
			this->frame_WX = this->memory[WX_ADDR];
			this->frame_WY = this->memory[WY_ADDR];

		}else if(this->current_scanline >= 154){
			this->clock = 0;
			this->memory[LY_ADDR] = 0;
			this->current_scanline = -1;
			this->window_screenline_aux = 0;
			return;
		}
	}

	if(this->current_mode != previous_mode){
		if(this->current_scanline < 144 && this->current_mode == MODE_3){
			this->update_background();
			this->update_window();
			this->update_sprites();
		}
	}

}
