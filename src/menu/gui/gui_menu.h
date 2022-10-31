/*  
 * Copyright (c) 2018 Guillem96
 *
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _MENU_H_
#define _MENU_H_
#include "soc/hw_init.h"
#include "utils/types.h"
#include "menu/gui/gui_menu_entry.h"
#include "menu/gui/custom-gui.h"
#include <stdio.h>

#define MAX_ENTRIES 0x80

typedef struct
{
	char title[0x100];
	int next_entry;
	int selected_index;
    custom_gui_t* custom_gui;
	gui_menu_entry_t *entries[MAX_ENTRIES];
} gui_menu_t;


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

void loadTheme();
void saveTheme(char *param);
void *theme(char *path);

/* Allocate menu to heap */
gui_menu_t *gui_menu_create(const char *title,char *back);

/* Add an entry to menu */
void gui_menu_append_entry(gui_menu_t *menu, gui_menu_entry_t *menu_entry);

/* Handle all menu related stuff */
int gui_menu_open(gui_menu_t *menu);
int gui_menu_boot(gui_menu_t *menu);
int gui_menu_open3(gui_menu_t *menu);

/* Deallocate a menu from heap */
void gui_menu_destroy(gui_menu_t *menu);

#endif