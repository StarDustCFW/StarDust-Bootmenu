#include <stdio.h>
#include "utils/types.h"
#include "menu/gui/gui_argon_menu.h"

typedef struct bitmap_hdr
{
	__uint16_t signature;
	__uint32_t file_size;
	__uint16_t reserved_1;
	__uint16_t reserved_2;
	__uint32_t pixel_offset;
} __attribute__((packed)) bit_hdr_t;

typedef struct dip_hdr
{
	__uint32_t size;
	__int32_t width;
	__int32_t height;
	__uint16_t planes;
	__uint16_t bits;
	__uint32_t compression;
	__uint32_t imgsize;
	__int32_t x_resolution;
	__int32_t y_resolution;
	__uint32_t no_colors;
	__uint32_t imp_colors;
} __attribute__((packed)) dip_hdr_t;

void create(gui_menu_t *menu, char *path, int x, int y, int (*handler)(void *), void *param);
void create_no_bitmap(gui_menu_t *menu, char *text, int x, int y, int width, int height);