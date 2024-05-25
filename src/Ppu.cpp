#include "ppu.h"
#include <cstdlib>
#include <iostream>
#include <stdlib.h>

#define LCDC_ADDR 0xff40
#define LY_ADDR 0xff44
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

PPU::PPU(){
	this->pixels = (uint32_t *)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
}

PPU::~PPU(){
	free(this->pixels);
}

void PPU::draw_pixel(uint8_t pixel_value, uint16_t x, uint16_t y){
	this->pixels[x + (y * SCREEN_WIDTH)] = this->colors[pixel_value];
}

void PPU::draw_tile(uint16_t tile_index, uint8_t tile_x, uint8_t tile_y){
    uint16_t tile_addr = this->tiledata_addr + (tile_index * TILE_SIZE);
    uint16_t pixel_x = uint16_t(tile_x) * 8;
    uint16_t pixel_y = uint16_t(tile_y) * 8;
    for(uint8_t i = 0; i < TILE_SIZE; i+=2){
        uint8_t byte_1 = this->memory[tile_addr + i];
        uint8_t byte_2 = this->memory[tile_addr + i + 1];
        for(int8_t bit_pos = 7; bit_pos >= 0; bit_pos--){
            uint8_t pixel_value = (((byte_2 >> bit_pos) & 1) << 1) + ((byte_1 >> bit_pos) & 1);
            this->draw_pixel(pixel_value, pixel_x + (7-bit_pos), pixel_y + (i/2));
        }
    }
}

void PPU::update_pixels(){
	uint8_t SCX = this->memory[SCX_ADDR];
	uint8_t SCY = this->memory[SCY_ADDR];
    for(uint16_t i = 0; i < 360; i++){
        uint8_t screen_x = i%20;
        uint8_t screen_y = i/20;

        uint16_t t_tile_x = ((SCX + screen_x) % 32);
        uint16_t t_tile_y = (((SCY/8) + screen_y) % 32) * 32;

        uint16_t tile_map_index = t_tile_x + t_tile_y;
        uint16_t tile_index = this->memory[this->tilemap_addr + tile_map_index];
		this->draw_tile(tile_index, screen_x, screen_y);
    }
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
	this->is_window_enabled = lcdc & LCDC_WINDOW_TILEMAP_BIT;
	this->tiledata_addr = lcdc & LCDC_BG_AND_WINDOW_ADDR_BIT ? 0x8000 : 0x9000;
	this->tilemap_addr = lcdc & LCDC_BG_TILEMAP_BIT ? 0x9C00 : 0x9800;
	this->obj_size = lcdc & LCDC_OBJ_SIZE_BIT ? 16 : 8;
	this->is_obj_enabled = lcdc & LCDC_OBJ_ENABLED_BIT;
	this->priority = lcdc & LCDC_PPU_ENABLED_BIT;
}

void PPU::tick(){
	if(this->clock < 100){
		this->clock++;
		return;
	}
	this->clock = 0;
	this->memory[LY_ADDR] = 0x90; //testing only, should remove. indicates vblank period
								  
	this->update_lcdc_variables();

	if(this->is_enabled){
		this->update_pixels();
	}
}
