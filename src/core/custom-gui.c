/*
 * Copyright (c) 2018 Guillem96
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

#include "core/custom-gui.h"
#include "gfx/gfx.h"
#include "mem/heap.h"
#include "utils/fs_utils.h"
#include "utils/util.h"
#include "menu/tools/theme.h"
#include <string.h>


custom_gui_t* custom_gui_load(u32 mem)
{
    custom_gui_t* custom_gui = (custom_gui_t*)malloc(sizeof(custom_gui_t));
	//custom backgroun for heach menu
	switch(mem)
	{
		case 1:
		custom_gui->custom_bg = (u8*)theme("back-set.bmp");
		break;
		
		case 2:
		case 5:
		custom_gui->custom_bg = (u8*)theme("back-exp.bmp");
		break;
		
		case 3:
		custom_gui->custom_bg = (u8*)theme("back-mem.bmp");
		break;
		
		case 4:
		custom_gui->custom_bg = (u8*)theme("back-inc.bmp");
		break;
		
		default:
		custom_gui->custom_bg = (u8*)theme("background.bmp");
	}
    custom_gui->title_bmp = (u8*)sd_file_read(CUSTOM_TITLE_PATH);
    return custom_gui;
}

void custom_gui_end(custom_gui_t* cg)
{
    free(cg->custom_bg);
    free(cg->title_bmp);
    free(cg);
}

bool render_custom_background(custom_gui_t* cg)
{
    if (cg->custom_bg == NULL)
        return false;
    
    gfx_render_splash(&g_gfx_ctxt, cg->custom_bg);
    return true;
}

bool render_custom_title(custom_gui_t* cg)
{  
    if (cg->title_bmp == NULL)
        return false;

    u32 bmp_width = (cg->title_bmp[0x12] | (cg->title_bmp[0x13] << 8) | (cg->title_bmp[0x14] << 16) | (cg->title_bmp[0x15] << 24));
    u32 bmp_height = (cg->title_bmp[0x16] | (cg->title_bmp[0x17] << 8) | (cg->title_bmp[0x18] << 16) | (cg->title_bmp[0x19] << 24));
    gfx_render_bmp_arg_bitmap(&g_gfx_ctxt, cg->title_bmp, 420, 10, bmp_width, bmp_height);
    return true;
}

int screenshot(void* params)
{
SDStrap();
    //width, height, and bitcount are the key factors:
    s32 width = 720;
    s32 height = 1280;
    u16 bitcount = 32;//<- 24-bit bitmap

    //take padding in to account
    int width_in_bytes = ((width * bitcount + 31) / 32) * 4;

    //total image size in bytes, not including header
    u32 imagesize = width_in_bytes * height;

    //this value is always 40, it's the sizeof(BITMAPINFOHEADER)
    const u32 bi_size = 40;

    //bitmap bits start after headerfile, 
    //this is sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
    const u32 buf_offset_bits = 54; 

    //total file size:
    u32 filesize = 54 + imagesize;

    //number of planes is usually 1
    const u16 bi_planes = 1;

    //create header:
    //copy to buffer instead of BITMAPFILEHEADER and BITMAPINFOHEADER
    //to avoid problems with structure packing
    unsigned char header[54] = { 0 };
	char* namef = "StarDust/screenshot.bmp";
    memcpy(header, "BM", 2);
    memcpy(header + 2 , &filesize, 4);
    memcpy(header + 10, &buf_offset_bits, 4);
    memcpy(header + 14, &bi_size, 4);
    memcpy(header + 18, &width, 4);
    memcpy(header + 22, &height, 4);
    memcpy(header + 26, &bi_planes, 2);
    memcpy(header + 28, &bitcount, 2);
    memcpy(header + 34, &imagesize, 4);
	
	for (int i=0;i < 30;i++){
		if (sd_file_exists(namef)){
			char charValue=i+'0';
			char tmp[256];
			strcpy(tmp, "StarDust/screenshot-");
			strcat(tmp, charValue);
			strcat(tmp, ".bmp");
		}
	}

    u8* buff = (u8*)malloc(imagesize + 54);
    memcpy(buff, header, 54);
    memcpy(buff + 54, g_gfx_ctxt.fb, imagesize);
    sd_save_to_file(buff, imagesize + 54, namef);
    free(buff);

    g_gfx_con.scale = 2;
    gfx_con_setpos(&g_gfx_con, 0, 605);
    gfx_printf(&g_gfx_con, " Screenshot saved!\n Find it at %s",namef);
    return 0;
}
