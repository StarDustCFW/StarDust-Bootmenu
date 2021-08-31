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
#include "soc/fuse.h"
#include "core/launcher.h"
#include "utils/util.h"
#include "utils/fs_utils.h"
#include "utils/touch.h"
#include "utils/btn.h"
#include "utils/dirlist.h"
#include "menu/gui/gui_argon_menu.h"
#include "minerva/minerva.h"
#include "utils/clean.h"

extern void pivot_stack(u32 stack_top);
static inline void setup_gfx()
{
	u32 *fb = display_init_framebuffer();
	gfx_init_ctxt(&g_gfx_ctxt, fb, 1280, 720, 720);
	gfx_clear_buffer(&g_gfx_ctxt);
	gfx_con_init(&g_gfx_con, &g_gfx_ctxt);
	gfx_con_setcol(&g_gfx_con, 0xFFCCCCCC, 1, BLACK);
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
		//some test verify payload
		if (sd_file_exists("StarDust/flags/ONE.flag"))
		{
			f_unlink("StarDust/flags/ONE.flag");
			launch_payload("payload.bin");
		}
		/*
		bool cancel_auto_chainloading = btn_read() & BTN_VOL_DOWN;
		bool load_menu = cancel_auto_chainloading || launch_payload("StarDust/payload.bin");
		*/
		//update stardust
		bool cancel_auto_chainloading = btn_read() & BTN_VOL_UP;
		if (sd_file_exists("StarDust_update/fixer.del") & !cancel_auto_chainloading)
		{
			if (sd_file_exists("license.dat") & !sd_file_exists("license.dat.bak"))
			{
				printerCU("Backup SXOS license.dat", "BackingUP...", 0);
				copyfile("license.dat", "license.dat.bak");
			}
			moverall("/StarDust_update", "", "*", "Updating");
			gfx_con_setpos(&g_gfx_con, 1, 100);
			gfx_printf(&g_gfx_con, "Clean Update\n");
			gfx_swap_buffer(&g_gfx_ctxt);
			deleteall("/StarDust_update", "*", "Clean Update");
			f_rename("/StarDust_update", "/StarDust_corrupt_update"); //just in case
			launch_payload("payload.bin");
		}

		if (sd_file_exists("fixer.del"))
			clean_up();

		/*
		gfx_printf(&g_gfx_con, "Autochainload canceled. Loading menu...\n");
		gfx_swap_buffer(&g_gfx_ctxt);
		msleep(2000);
		BootStrapNX();
		
		if (load_menu)
		*/
		gui_init_argon_boot();
		//gui_init_argon_menu();
	}
	else
	{
		gfx_printf(&g_gfx_con, "No sd card found...\n");
	}
	gfx_swap_buffer(&g_gfx_ctxt);

	BootStrapNX();

	/*If payload launch fails wait for user input to reboot the switch */
	gfx_printf(&g_gfx_con, "Press power button to reboot into RCM...\n\n");
	gfx_swap_buffer(&g_gfx_ctxt);
	wait_for_button_and_reboot();
}
