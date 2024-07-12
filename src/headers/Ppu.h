#pragma once
#include <stdint.h>

#ifndef PPU_H
#define PPU_H
struct SpriteData {
	uint8_t y = 0;
	uint8_t x = 0;
	uint8_t tile_index = 0;
	bool oam_mode = false;
	bool x_flip = false;
	bool y_flip = false;
	int16_t x_s = 0;
	int16_t y_s = 0;
};
class PPU{
	public:
		PPU();
		~PPU();
		uint8_t * memory = NULL;
		uint32_t * pixels = NULL;
		int clock = 0;
		uint8_t last_lcdc = 0;
		int16_t window_screenline_aux = 0;
		int16_t current_scanline = -1;
		uint8_t current_mode = 0;
		uint8_t is_enabled = 0;
		uint16_t window_tilemap_addr = 0;
		uint8_t is_window_enabled = 0;
		uint16_t tiledata_addr = 0;
		uint16_t tilemap_addr = 0;
		uint8_t obj_size = 0;
		uint8_t is_obj_enabled = 0;
		uint8_t priority = 0;
		uint8_t frame_WX = 0;
		uint8_t frame_WY = 0;
		bool frame_ready = false; 
		uint32_t colors[4] = { 
			0x9bbc0900, 
			0x8bac0f00, 
			0x30623000,
			0x0f380f00
		};
		uint8_t get_color_id(uint8_t tile_index, uint8_t tile_pixel_x, uint8_t tile_pixel_y);
		void draw_sprite_row(struct SpriteData sprite_data, uint8_t sprite_row);
		struct SpriteData get_SpriteData(uint8_t sprite_index);
		void draw_pixel(uint8_t color_id, uint16_t screen_x, uint16_t screen_y, bool priority, bool is_sprite);
		void update_background();
		void update_window();
		void update_sprites();
		void handle_interrupts();
		void update_lcdc_variables();
		void tick(uint16_t cycles);
};
#endif
