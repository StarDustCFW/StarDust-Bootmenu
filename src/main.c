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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "gfx/di.h"
#include "gfx/gfx.h"
#include "soc/hw_init.h"
#include "mem/heap.h"


#include "core/launcher.h"

#include "utils/util.h"
#include "utils/fs_utils.h"
#include "utils/touch.h"
#include "utils/btn.h"
#include "utils/dirlist.h"
#include "menu/gui/gui_argon_menu.h"

#include "minerva/minerva.h"

#define PATH_ENABLE_TOUCH "StarDust/touch"

extern void pivot_stack(u32 stack_top);

static inline void setup_gfx()
{
    u32 *fb = display_init_framebuffer();
    gfx_init_ctxt(&g_gfx_ctxt, fb, 1280, 720, 720);
    gfx_clear_buffer(&g_gfx_ctxt);
    gfx_con_init(&g_gfx_con, &g_gfx_ctxt);
    gfx_con_setcol(&g_gfx_con, 0xFFCCCCCC, 1, BLACK);
}

void deleteall(char* directory, char* filet)
{
f_unlink(directory);
char* payloads = listfil(directory, filet, true);
    u32 i = 0;
    while(payloads[i * 256])
    {
char* payloadpath = (char*)malloc(256);
			if(strlen(&payloads[i * 256]) <= 100){			
			strcpy(payloadpath, directory);
			strcat(payloadpath, "/");
			strcat(payloadpath, &payloads[i * 256]);
			f_unlink(payloadpath);
			}
	i++;
    }
f_unlink(directory);
}

//first Delete After Update
void clean_up()
{
//delete bootloader for tinfoil 
deleteall("/bootloader/ini", "*");
deleteall("/bootloader/payloads", "*");
deleteall("/bootloader/libtools", "*");
deleteall("/bootloader/sys", "*");
deleteall("/bootloader", "*");

deleteall("/ftpd", "*");

deleteall("/ReiNX/patches/es_patches", "*");
deleteall("/ReiNX/sysmodules.dis", "*");
    f_unlink("/ReiNX/sysmodules/fs_mitm.kip");
    f_unlink("/ReiNX/sysmodules/ldn_mitm.kip");
deleteall("/ReiNX/exefs_patches/Youtube", "*");
deleteall("/ReiNX/titles/010000000000000D/exefs", "*");
deleteall("/ReiNX/titles/010000000000000D", "*");
deleteall("/ReiNX/titles/0100000000000032/exefs", "*");
deleteall("/ReiNX/titles/0100000000000032", "*");
deleteall("/ReiNX/titles/0100000000000032/flags", "*");

deleteall("/sxos/titles/0100000000000034", "*");
deleteall("/SXOS/titles/0100000000000034", "*");


deleteall("/switch/KosmosToolbox", "*");
deleteall("/switch/KosmosUpdater", "*");


deleteall("/atmosphere/kips", "*");


	//remove themes
	f_unlink("/atmosphere/titles/0100000000001000/romfs/lyt/Set.szs");
	f_unlink("/atmosphere/titles/0100000000001000/romfs/lyt/common.szs");
	f_unlink("/ReiNX/titles/0100000000001000/romfs/lyt/Set.szs");
	f_unlink("/ReiNX/titles/0100000000001000/romfs/lyt/common.szs");
	f_unlink("/sxos/titles/0100000000001000/romfs/lyt/Set.szs");
	f_unlink("/sxos/titles/0100000000001000/romfs/lyt/common.szs");
	f_unlink("/atmosphere/titles/0100000000001013/fsmitm.flag");
	f_unlink("/ReiNX/titles/0100000000001013/fsmitm.flag");
	f_unlink("/sxos/titles/0100000000001013/fsmitm.flag");
	
//deleteall("/////", "*");	
	//close
	f_unlink("/fixer.del");

}


void ipl_main()
{
    /* Configure Switch Hardware (thanks to hekate project) */
    config_hw();

    /* Init the stack and the heap */
    pivot_stack(0x90010000);
    heap_init(0x90020000);

    /* Init display and gfx module */
    display_init();
    setup_gfx();
    display_backlight_pwm_init();
    display_backlight_brightness(20, 1000);


    /* Train DRAM */
    g_gfx_con.mute = 1; /* Silence minerva, comment for debug */
    minerva();
    g_gfx_con.mute = 0;

    /* Cofigure touch input */
    touch_power_on();
    
    /* Mount Sd card and launch payload */
    if (sd_mount())
    {
        bool cancel_auto_chainloading = btn_read() & BTN_VOL_DOWN;
        bool load_menu = cancel_auto_chainloading || launch_payload("StarDust/payload.bin");
		//remove autoboot
        f_unlink("StarDust/payload.bin");
        f_unlink("StarDust/autobootecho.txt");
		
		if(sd_file_exists("fixer.del"))
		{		
		gfx_printf(&g_gfx_con, "Deleting...\n");		
		clean_up();
		}
		
        gfx_printf(&g_gfx_con, "Autochainload canceled. Loading menu...\n");
        gfx_swap_buffer(&g_gfx_ctxt);
//		msleep(25000);
        if (load_menu)
        gui_init_argon_menu();

    } else {
        gfx_printf(&g_gfx_con, "No sd card found...\n");
    }
        gfx_swap_buffer(&g_gfx_ctxt);
	
BootStrapNX();

	
	
	
/*If payload launch fails wait for user input to reboot the switch */
    gfx_printf(&g_gfx_con, "Press power button to reboot into RCM...\n\n");
    gfx_swap_buffer(&g_gfx_ctxt);
    wait_for_button_and_reboot();
}

