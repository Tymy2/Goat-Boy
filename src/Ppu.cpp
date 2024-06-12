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
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define TILE_SIZE 16

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

#define DOTS_PER_CYCLE 4
#define DOTS_PER_SCANLINE 456

bool oam_mode = false;

PPU::PPU(){
	this->pixels = (uint32_t *)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
}

PPU::~PPU(){
	free(this->pixels);
}

void PPU::draw_pixel(uint8_t pixel_value, uint16_t x, uint16_t y){
	if(oam_mode && (pixel_value > 0)){
		return;
	}
	this->pixels[x + (y * SCREEN_WIDTH)] = this->colors[pixel_value];
}

void PPU::draw_tile(uint16_t tile_index, uint8_t tile_x, uint8_t tile_y, int8_t offset_x, int8_t offset_y){
    uint16_t tile_addr = this->tiledata_addr + (tile_index * TILE_SIZE);
    uint16_t pixel_x = (uint16_t(tile_x) * 8) - offset_x;
    uint16_t pixel_y = (uint16_t(tile_y) * 8) - offset_y;
    for(uint8_t i = 0; i < TILE_SIZE; i+=2){
		uint16_t y_coord_pixel = pixel_y + (i/2);
		if(y_coord_pixel >= 144){
			continue;
		}
		uint8_t byte_1 = this->memory[tile_addr + i];
        uint8_t byte_2 = this->memory[tile_addr + i + 1];
        for(int8_t bit_pos = 7; bit_pos >= offset_x; bit_pos--){
			uint16_t x_coord_pixel = pixel_x + (7-bit_pos);
			if(x_coord_pixel >= 160){
				break;
			}
            uint8_t pixel_value = (((byte_2 >> bit_pos) & 1) << 1) + ((byte_1 >> bit_pos) & 1);
            this->draw_pixel(pixel_value, x_coord_pixel, y_coord_pixel);
        }
    }
}

void PPU::update_pixels(){
	uint8_t SCX = this->memory[SCX_ADDR];
	uint8_t SCY = this->memory[SCY_ADDR];
	uint8_t tile_start_x = SCX/8;
	uint8_t tile_start_y = SCY/8;
	int8_t offset_x = SCX - (tile_start_x * 8);
	int8_t offset_y = SCY - (tile_start_y * 8);
    for(uint16_t i = 0; i < 440; i++){
        uint8_t screen_x = i%20;
        uint8_t screen_y = i/20;

        uint16_t tilemap_x = ((tile_start_x + screen_x) % 32);
        uint16_t tilemap_y = ((tile_start_y + screen_y) % 32);

        uint16_t tile_map_index = tilemap_x + (tilemap_y * 32);
        uint16_t tile_index = this->memory[this->tilemap_addr + tile_map_index];
		this->draw_tile(tile_index, screen_x, screen_y, offset_x, offset_y);
    }
}

void PPU::update_window(){
	if(!this->is_window_enabled){
		return;
	}

	uint8_t WX = this->memory[0xff4b];
	uint8_t WY = this->memory[0xff4a];
	if(WX > 166 || WY > 143){
		return;
	}

	uint8_t tile_start_x = WX/8;
	uint8_t tile_start_y = WY/8;
	int8_t offset_x = WX - (tile_start_x * 8);
	int8_t offset_y = WY - (tile_start_y * 8);
	for(uint16_t i = 0; i < 440; i++){
        uint8_t screen_x = i%20;
        uint8_t screen_y = i/20;

        uint16_t tilemap_x = ((tile_start_x + screen_x) % 32);
        uint16_t tilemap_y = ((tile_start_y + screen_y) % 32);

        uint16_t tile_map_index = tilemap_x + (tilemap_y * 32);
        uint16_t tile_index = this->memory[this->window_tilemap_addr + tile_map_index];
		this->draw_tile(tile_index, screen_x, screen_y, offset_x, offset_y);
    }

}

void PPU::update_sprites(){
	if(!this->is_obj_enabled){
		return;
	}

	uint16_t mem_addr = 0xfe00;
	uint16_t old_tiledata_addr = this->tiledata_addr;
	this->tiledata_addr = 0x8000;
	for(uint8_t sprite_i = 0; sprite_i < 40; sprite_i++){
		uint8_t offset = sprite_i*4;
		uint8_t y_pos = this->memory[mem_addr+offset]-16;
		uint8_t x_pos = this->memory[mem_addr+offset+1]-8;
		uint8_t tile_i = this->memory[mem_addr+offset+2];
		uint8_t attributes = this->memory[mem_addr+offset+3];
		
		if(y_pos >= 144 || x_pos >= 160){
			continue;
		}
		
		uint8_t tile_x = (x_pos / 8);
		uint8_t offset_x = x_pos - (tile_x*8);
		uint8_t tile_y = (y_pos / 8);
		uint8_t offset_y = y_pos - (tile_y*8);

		oam_mode = (attributes & 0x80) > 0;
		
		if(this->obj_size == 16){
			this->draw_tile(tile_i & 0xfe, tile_x, tile_y, offset_x, offset_y);
			this->draw_tile(tile_i | 0x01, tile_x, tile_y+1, offset_x, offset_y);
		}else{
			this->draw_tile(tile_i, tile_x, tile_y, offset_x, offset_y);
		}
		
	}
	this->tiledata_addr = old_tiledata_addr;
}

/*
LCDC:
	bit 7 -> LCD & PPU enable: 0 = Off; 1 = On
	bit 6 -> Window tile map area: 0 = 9800–9BFF; 1 = 9C00–9FFF
	bit 5 -> Window enable: 0 = Off; 1 = On
	bit 4 -> BG & Window tile data area: 0 = 8800–97FF; 1 = 8000–8FFF
	bit 3 -> BG tile map area: 0 = 9800–9BFF; 1 = 9C00–9FFF
	bit 2 -> OBJ size: 0 = 8×8; 1 = 8×16
	bit 1 -> OBJ enable: 0 = Off; 1 = On
	bit 0 -> BG & Window enable / priority [Different meaning in CGB Mode]: 0 = Off; 1 = On
*/
void PPU::update_lcdc_variables(){
	uint8_t lcdc = this->memory[LCDC_ADDR];
	if(lcdc == this->last_lcdc){ // if nothing changed, then dont bother in updating
		return;
	}
	this->last_lcdc = lcdc;

	this->is_enabled = lcdc & LCDC_PPU_ENABLED_BIT;	
	this->window_tilemap_addr = lcdc & LCDC_WINDOW_TILEMAP_BIT ? 0x9C00 : 0x9800;
	this->is_window_enabled = lcdc & LCDC_WINDOW_ENABLED_BIT;
	this->tiledata_addr = lcdc & LCDC_BG_AND_WINDOW_ADDR_BIT ? 0x8000 : 0x8800;
	this->tilemap_addr = lcdc & LCDC_BG_TILEMAP_BIT ? 0x9C00 : 0x9800;
	this->obj_size = lcdc & LCDC_OBJ_SIZE_BIT ? 16 : 8;
	this->is_obj_enabled = lcdc & LCDC_OBJ_ENABLED_BIT;
	this->priority = lcdc & LCDC_PRIORITY_BIT;
}

uint8_t get_mode(int clock_ticks, uint8_t scanline){
	if(scanline >= 144){
		return 1;
	}

	uint8_t dots_in_scanline = (clock_ticks / 4) % 456;
	if(dots_in_scanline <= 80){
		return 2;
	}

	if(dots_in_scanline <=252){
		return 3;
	}
	return 0;
}

void PPU::tick(uint16_t cpu_cycles_index){
	this->clock += CYCLES[cpu_cycles_index] * DOTS_PER_CYCLE;
	uint8_t scanline = this->clock / DOTS_PER_SCANLINE;
	this->memory[LY_ADDR] = scanline;
	bool lyc_eq_ly = (scanline == this->memory[LYC_ADDR]);
	uint8_t mode = get_mode(this->clock, scanline);

	// updating STAT register
	this->memory[STAT_ADDR] |= lyc_eq_ly << 2;
	this->memory[STAT_ADDR] |= mode;

	// STAT interrupt requested
	uint8_t interrupts_states = (lyc_eq_ly << 3) + ((0x1 << mode) & 0b111 ); // the and operation just discards the mode 3
	uint8_t stat_interrupts_selects = this->memory[STAT_ADDR] >> 3;
	if((interrupts_states & stat_interrupts_selects) > 0){ // Stat interrupt can trigger
		this->memory[IF_ADDR] |= STAT_INTERRUPT_BIT;
	}
	
	// VBLANK interrupt requested
	if(scanline == 144){
		this->memory[IF_ADDR] |= VBLANK_INTERRUPT_BIT;
	}

	if(scanline >= 154){
		this->clock = 0;
		this->memory[LY_ADDR] = 0;
		this->update_lcdc_variables();		
		if(this->is_enabled){
			this->update_pixels();
			this->update_window();
			this->update_sprites();
		}
	}
}
