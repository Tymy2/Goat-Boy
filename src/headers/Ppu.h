#pragma once
#include <stdint.h>

#ifndef PPU_H
#define PPU_H
class PPU{
	public:
		PPU();
		~PPU();
		uint8_t * memory = NULL;
		uint32_t * pixels = NULL;
		int clock = 0;
		uint8_t last_lcdc = 0;
		uint8_t is_enabled = 0;
		uint16_t window_tilemap_addr = 0;
		uint8_t is_window_enabled = 0;
		uint16_t tiledata_addr = 0;
		uint16_t tilemap_addr = 0;
		uint8_t obj_size = 0;
		uint8_t is_obj_enabled = 0;
		uint8_t priority = 0;
		uint32_t colors[4] = { 
			0x9bbc0900, 
			0x8bac0f00, 
			0x30623000,
			0x0f380f00
		};
		void draw_pixel(uint8_t, uint16_t, uint16_t);
		void draw_tile(uint16_t, uint8_t, uint8_t, int8_t, int8_t);
		void update_pixels();
		void update_lcdc_variables();
		void tick(uint16_t);
};
#endif
