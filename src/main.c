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

#include "mem/heap.h"

#include "soc/hw_init.h"

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

void clean_up()
{
f_unlink("bootloader/.picasa.ini");
f_unlink("bootloader/hekate_ipl.ini");
f_unlink("bootloader/hekate_ipl.ini.bck");
f_unlink("bootloader/update.bin");
f_unlink("bootloader/payloads/Atmosphere.bin");
f_unlink("bootloader/payloads/payload.bin");
f_unlink("bootloader/payloads/ReiNX.bin");
f_unlink("bootloader/payloads/sxos.bin");
f_unlink("bootloader/sys/libsys_lp0.bso");
f_unlink("bootloader/payloads/Lockpick_RCM.bin");
f_unlink("bootloader/payloads/fusee-primary.bin");
f_unlink("bootloader/bootlogo.bmp");
f_unlink("bootloader/libtools");
f_unlink("bootloader/payloads");
f_unlink("bootloader/sys");
f_unlink("bootloader/ini");
f_unlink("bootloader");

	f_unlink("/ftpd/connect.mp3");
	f_unlink("/ftpd/disconnect.mp3");
	f_unlink("/ftpd/pauseoff.mp3");
	f_unlink("/ftpd/pauseon.mp3");
	f_unlink("/ftpd");
	
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

    f_unlink("/ReiNX/patches/es_patches/D2D2430244D162C9FAABE8C89A58C6E3962160F1.ips");
    f_unlink("/ReiNX/patches/es_patches/F65FBA401BAC3CDDEA4917DE22E8B426B3A6C3AD.ips");
    f_unlink("/ReiNX/patches/es_patches");
    f_unlink("/ReiNX/sysmodules.dis/fs_mitm.kip");
    f_unlink("/ReiNX/sysmodules.dis/ldn_mitm.kip");
    f_unlink("/ReiNX/sysmodules.dis/pm.kip");
    f_unlink("/ReiNX/sysmodules/fs_mitm.kip");
    f_unlink("/ReiNX/sysmodules/ldn_mitm.kip");
    f_unlink("/ReiNX/exefs_patches/Youtube/534E296FFB5F854D94A719F7727FDE2700000000000000000000000000000000.ips");
    f_unlink("/ReiNX/exefs_patches/Youtube");
    f_unlink("/ReiNX/exefs_patches");
    f_unlink("/ReiNX/titles/010000000000000D/exefs.nsp");
    f_unlink("/ReiNX/titles/010000000000000D");
    f_unlink("/ReiNX/titles/0100000000000032/flags/boot2.flag");
    f_unlink("/ReiNX/titles/0100000000000032/flags");
    f_unlink("/ReiNX/titles/0100000000000032/exefs.nsp");
    f_unlink("/ReiNX/titles/0100000000000032");
    f_unlink("/ReiNX/titles/0100000000000036/exefs.nsp");
	
	f_unlink("/switch/KosmosToolbox/config.json");
	f_unlink("/switch/KosmosToolbox/KosmosToolbox.nro");
	f_unlink("/switch/KosmosToolbox");
	
	f_unlink("/switch/KosmosUpdater/internal.db");
	f_unlink("/switch/KosmosUpdater/KosmosUpdater.nro");
	f_unlink("/switch/KosmosUpdater/settings.cfg");
	f_unlink("/switch/KosmosUpdater");
	
	f_unlink("/atmosphere/kips/fs_mitm.kip");
	f_unlink("/atmosphere/kips/ldn_mitm.kip");
	f_unlink("/atmosphere/kips/loader.kip");
	f_unlink("/atmosphere/kips/pm.kip");
	f_unlink("/atmosphere/kips/sm.kip");
	f_unlink("/atmosphere/kips/");
	f_unlink("/atmosphere/kips/");
	f_unlink("/atmosphere/kips/");
	f_unlink("/atmosphere/kips/");
	f_unlink("/atmosphere/kips/");
	f_unlink("/atmosphere/kips/");

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
    display_backlight_brightness(100, 1000);


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
        if (load_menu)
            gui_init_argon_menu();

    } else {
        gfx_printf(&g_gfx_con, "No sd card found...\n");
    }

    /* If payload launch fails wait for user input to reboot the switch */
    gfx_printf(&g_gfx_con, "Press power button to reboot into RCM...\n\n");
    gfx_swap_buffer(&g_gfx_ctxt);
    wait_for_button_and_reboot();
}

