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

#include "menu/gui/gui_argon_menu.h"
#include "menu/gui/gui_menu.h"
#include "menu/gui/gui_menu_pool.h"

#include "gfx/gfx.h"

#include "utils/types.h"
#include "utils/fs_utils.h"
#include "utils/dirlist.h"
#include "utils/util.h"
#include "utils/touch.h"

#include "core/launcher.h"
#include "core/payloads.h"
#include "core/custom-gui.h"

#include "mem/heap.h"

#define COLUMNS 4  
#define ROWS 2
#define ELEM_SIZE 230
#define MARGIN_TOP 100
#define MARGIN_LEFT 46
u32 tog = 0;
static int tool_reboot_rcm(void* param);
static int tool_power_off(void* param);
static int tool_menu_tog(void* param);
/* Generate entries dynamically */
static void generate_payloads_entries(char* payloads, gui_menu_t* menu)
{
    if (payloads == NULL)
    {
        g_gfx_con.scale = 4;
        gfx_con_setpos(&g_gfx_con, 140, 250);
        gfx_printf(&g_gfx_con, "Payloads directory is empty...\n");
        
        g_gfx_con.scale = 3;
        gfx_con_setpos(&g_gfx_con, 110, 370);
        gfx_printf(&g_gfx_con, "Place your payloads inside \"%s\"", PAYLOADS_DIR);

        return;
    }

    u32 i = 0;
    /* For each payload generate its logo, its name and its path */
    while(payloads[i * 256])
    {
        char* payload_path = (char*)malloc(256);
        payload_full_path(&payloads[i * 256], payload_path);
        
        char payload_logo[256];
        payload_logo_path(&payloads[i * 256], payload_logo);

			u32 row = i;
        u32 col = i % COLUMNS;
        u32 x = g_gfx_ctxt.width / COLUMNS * col + MARGIN_LEFT;
        u32 y = 140 / ROWS * row + MARGIN_TOP + MARGIN_TOP;

        const char* payload_wo_bin = str_replace(&payloads[i * 256], ".bin", "");
        gui_menu_append_entry(menu, 
            gui_create_menu_entry(payload_wo_bin,
                                    sd_file_read(payload_logo), 
                                    x, y,
                                    200, 200,
                                    (int (*)(void *))launch_payload, (void*)payload_path));
        i++;
    }
}
/* Second Menu */
static void generate_payloads_back(char* payback, gui_menu_t* menu)
{

    u32 e = 0;
    /* For each payload generate its logo, its name and its path */
    while(payback[e * 256])
    {
        char* payload_path = (char*)malloc(256);
        payload_full_back(&payback[e * 256], payload_path);
        
        char payload_logo[256];
        payload_logo_path(&payback[e * 256], payload_logo);

		u32 row = e / COLUMNS;
        u32 col = e % COLUMNS;
        u32 x = g_gfx_ctxt.width / COLUMNS * col + MARGIN_LEFT;
        u32 y = g_gfx_ctxt.height / ROWS * row + MARGIN_TOP + (row == 0 ? 30 : -60);

        const char* payload_wo_bin = str_replace(&payback[e * 256], ".bin", "");
        gui_menu_append_entry(menu, 
            gui_create_menu_entry(payload_wo_bin,
                                    sd_file_read(payload_logo),
                                    x, y,
                                    200, 200,
                                    (int (*)(void *))launch_payload, (void*)payload_path));
        e++;
	
    }
}

/* Init needed menus for ArgonNX */
void gui_init_argon_menu(void)
{
    /* Init pool for menu */
    gui_menu_pool_init();
gui_menu_t* menu = gui_menu_create("ArgonNX");
 
if (tog == 0){
//generate main menu

generate_payloads_entries(dirlist(PAYLOADS_DIR, "*.bin", false), menu);
gui_menu_append_entry(menu, 
    gui_create_menu_entry_no_bitmap("More", 250, 680, 150, 100, tool_menu_tog, NULL));

}else{
//generate menu2
generate_payloads_back(dirlist(PAYBACK_DIR, "*.bin", false), menu);

gui_menu_append_entry(menu, 
    gui_create_menu_entry_no_bitmap("Return", 250, 680, 150, 100, tool_menu_tog, NULL));

}
     gui_menu_append_entry(menu, 
            gui_create_menu_entry_no_bitmap("Screenshot", 400, 680, 150, 100, (int (*)(void *))screenshot, NULL));

    /* Generate reboot rcm and shutdown entry */
    gui_menu_append_entry(menu, 
            gui_create_menu_entry_no_bitmap("Power off", 600, 680, 150, 100, tool_power_off, NULL));

    gui_menu_append_entry(menu, 
            gui_create_menu_entry_no_bitmap("Reboot RCM", 800, 680, 150, 100, tool_reboot_rcm, NULL));
    

    /* Start menu */
    gui_menu_open(menu);

    /* Clear all entries and menus */
    gui_menu_pool_cleanup();
}

static int tool_reboot_rcm(void* param)
{

    gui_menu_pool_cleanup();
    reboot_rcm();
    return 0;
}
static int tool_menu_tog(void* param)
{if (tog == 0){
tog = 1;
}else{
tog = 0;
}
gui_menu_pool_cleanup();
gui_init_argon_menu();
return 0;
}

static int tool_power_off(void* param)
{
    gui_menu_pool_cleanup();
    power_off();
    return 0;
}