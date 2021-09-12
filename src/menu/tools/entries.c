#include <stdio.h>
#include "menu/gui/gui_argon_menu.h"
#include "menu/tools/theme.h"
#include "menu/tools/entries.h"
#include "utils/types.h"
#include "utils/fs_utils.h"

void create(gui_menu_t *menu, char *path, int x, int y, int (*handler)(void *), void *param)
{
	bit_hdr_t file_header;
	dip_hdr_t file_info_header;
	FIL fptr;
	f_open(&fptr, theme(path), FA_READ);

	__off_t size = f_size(&fptr);

	f_read(&fptr, &file_header, sizeof(file_header), NULL);
	f_read(&fptr, &file_info_header, sizeof(file_info_header), NULL);

	f_lseek(&fptr, 0);

	void *buf = malloc(size);
	f_read(&fptr, buf, size, NULL);

	f_close(&fptr);

	gui_menu_append_entry(menu, gui_create_menu_entry("", buf, x, y, file_info_header.width, file_info_header.height, handler, param));
}

void create_no_bitmap(gui_menu_t *menu, char *text, int x, int y, int width, int height)
{
	gui_menu_append_entry(menu, gui_create_menu_entry_no_bitmap(text, x, y, width, height, NULL, NULL));
}