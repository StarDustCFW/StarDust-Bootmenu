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
#include "power/max17050.h"
#include <string.h>
#include "menu/gui/gui_argon_menu.h"
#include "menu/gui/gui_menu.h"
#include "menu/gui/gui_menu_pool.h"
#include "menu/gui/custom-gui.h"
#include "gfx/di.h"
#include "gfx/gfx.h"
#include <stdio.h>
#include "utils/types.h"
#include "menu/tools/fs_utils.h"
#include "utils/dirlist.h"
#include "utils/util.h"
#include "menu/tools/touch.h"
#include "utils/btn.h"
#include "core/launcher.h"
#include "menu/gui/custom-gui.h"
#include "mem/heap.h"
#include "minerva/minerva.h"
#include "menu/tools/tools.h"

#define COLUMNS 4
#define ROWS 2
#define ELEM_SIZE 230
#define MARGIN_TOP 100
#define MARGIN_LEFT 46
#define MAX_CHAR 100 typedef char string[MAX_CHAR + 1];

extern char Sversion[4];

u32 brillo = 100;
u32 brilloM = 50;

const char *sysserial;
const char *emuserial;
char *buffer_blk;

//menus
u64 main_menu = 0;
const char* imagemenus = "background.bmp";

//low icons Y
u64 low_icons = 645;

//some indicators
u32 retir = 0;
u32 Incac = 0;
u32 servstep = 0;
u32 isAMS = 1;
u32 iamsafe=0;

/* Init Boot screen */
void gui_init_argon_boot(void)
{
	SDStrap();
	loadTheme();
	/* Init pool for menu */
	if (sd_file_exists("StarDust/autoboot.inc"))
	{
		main_menu = 4;
        imagemenus = "back-inc.bmp";
		f_unlink("StarDust/autoboot.inc");
		Incac = 1;
	}
	if (sd_file_exists("StarDust/flags/IamSafe.flag"))
	{
		f_unlink("StarDust/flags/IamSafe.flag");
		iamsafe = 1;
	}
	
	gui_menu_pool_init();
	gui_menu_t *menu = gui_menu_create("ArgonNX", imagemenus);
	change_brightness(0);

	//show display without icons
	int res = 0;
	if (Incac != 1) {
		res = gui_menu_boot(menu);
	}
			

	if (res > 0){gfx_swap_buffer(&g_gfx_ctxt);}
	//corrections on start
	f_mkdir("StarDust/flags");
	f_unlink("auto.bak");

	//set archive bit
	f_chmod("switch/LinkUser", AM_ARC, AM_ARC);

	//profile redo
	if (sd_file_exists("/atmosphere/contents/0100000000001013/exefs.on"))
	{
		f_move("/atmosphere/contents/0100000000001013/exefs.on", "/atmosphere/contents/0100000000001013/exefs.nsp");
		f_unlink("/atmosphere/contents/0100000000001013/exefs.on");
	}

	//prevent accidental boot to ofw
	if (sd_file_exists("bootloader/hekate_ipl.bkp"))
	{
		f_unlink("bootloader/hekate_ipl.ini");
		f_rename("bootloader/hekate_ipl.bkp", "bootloader/hekate_ipl.ini");
	}
	else
	{
		if (sd_file_size("bootloader/hekate_ipl.ini") == sd_file_size("bootloader/Stock"))
			copyfile("bootloader/hekate_ipl.conf", "bootloader/hekate_ipl.ini");
	}

	//waith user input
	if (sd_file_exists("StarDust/autobootecho.txt") & (Incac == 0))
		btn_wait_timeout(3000, BTN_VOL_UP);

	bool cancel_auto_chainloading = btn_read() & BTN_VOL_UP;
	if (sd_file_exists("StarDust/autobootecho.txt") && (!cancel_auto_chainloading) & (Incac == 0))
	{
		//autoboot
		if (strstr(Sversion, "A") != NULL)
			launcher("StarDust/payloads/Atmosphere.bin");

		if (strstr(Sversion, "S") != NULL)
			launcher("StarDust/payloads/SXOS.bin");

		if (sd_file_exists("/switchroot/ubuntu/coreboot.rom") && (strstr(Sversion, "Ub") != NULL))
		{
			launcher("switchroot/ubuntu/coreboot.rom");
		}

		if (sd_file_exists("/switchroot/android/coreboot.rom") && (strstr(Sversion, "TW") != NULL))
		{
			launcher("switchroot/android/coreboot.rom");
		}
	}
	f_unlink("StarDust/payload.bin");

	if (btn_read() & BTN_VOL_DOWN)
		f_unlink("StarDust/autobootecho.txt");

	pre_load_menus(0, 1);
	gui_init_argon_menu();
}

gui_menu_t *menu;
gui_menu_t *menu_0;
gui_menu_t *menu_1;
gui_menu_t *menu_2;
gui_menu_t *menu_3;
gui_menu_t *menu_4;
gui_menu_t *menu_5;

void pre_load_menus(int menus, bool StarUp)
{
	SDStrap();
	if (menus == 0 || StarUp)
	{
		menu_0 = gui_menu_create("ArgonNX", "background.bmp");
		//generate main menu
		u32 main_iconY = 200;
		u32 main_iconX = 190;
		u32 main_iconXS = 300;

		//Create emummc icon
		u32 buttonY = main_iconY - 67;
		if (sd_file_exists("emummc/emummc.ini"))
		{
			char *str = sd_file_read("emummc/emummc.ini");
			u32 count = strlen(str)-1;
			if (str[count] != '\n'){
				while (true) {
					if (count==0) break;
					if (str[count] == '\n'){
						str[count]='\n';
						str[count+1]='\n';

						sd_save_to_file(str, count+2, "emummc/emummc.ini");
						break;
					}
					str[count]='\0';
					count--;
				}
			}

			if (retir == 0)
			{
				if (strstr(str, "emummc_") != NULL)
				{
					str = str_replace(str, "emummc_", "");
					u32 size = strlen(str);
					str[size]=0;
					str[size]='\n';
					sd_save_to_file(str, size, "emummc/emummc.ini");
				}

				if (strstr(str, " ") != NULL)
				{
					str = str_replace(str, " ", "");
					u32 size = strlen(str);
					str[size]=0;
					str[size]='\n';
					sd_save_to_file(str, size, "emummc/emummc.ini");
				}
				retir = 1;
				if (strstr(str, "enabled=1") != NULL)
				{
					retir = 2;
				}
			}

			if (retir == 2)
			{
				//				main_iconX = main_iconX + main_iconXS/2;
				create(menu_0, "Icons/bon.bmp", main_iconX + 50, buttonY, (int (*)(void *))tool_emu, (void *)0); //- 80, - 500
			}
			else
			{
				create(menu_0, "Icons/boff.bmp", main_iconX + 50, buttonY, (int (*)(void *))tool_emu, (void *)1); //- 80, - 500
			}
		}

		create(menu_0, "Icons/Atmosphere.bmp", main_iconX, main_iconY, (int (*)(void *))launcher, (void *)"/StarDust/payloads/Atmosphere.bin");
		main_iconX = main_iconX + main_iconXS;

		//			if(retir <= 1)
		//			{
		//			gui_menu_append_entry(menu_0,gui_create_menu_entry("",theme("Icons/ReiNX.bmp"), main_iconX, main_iconY, 300 , 300,(int (*)(void *))launcher, (void*)"/StarDust/payloads/ReiNX.bin"));
		main_iconX = main_iconX + main_iconXS;
		//			}

		create(menu_0, "Icons/SXOS.bmp", main_iconX, main_iconY, (int (*)(void *))launcher, (void *)"/StarDust/payloads/SXOS.bin");
		//
		create(menu_0, "Icons/Stock.bmp", 540, main_iconY + 100, (int (*)(void *))hekateOFW, (void *)1);
		if (sd_file_exists("/switchroot/android/coreboot.rom"))
			create(menu_0, "Icons/Android.bmp", 590, main_iconY - 30, (int (*)(void *))launcher, (void *)"/switchroot/android/coreboot.rom");
		else if (sd_file_exists("/switchroot_android/coreboot.rom"))
			create(menu_0, "Icons/Android.bmp", 590, main_iconY - 30, (int (*)(void *))launcher, (void *)"/switchroot_android/coreboot.rom");

		if (sd_file_exists("/switchroot/ubuntu/coreboot.rom"))
			create(menu_0, "Icons/Ubuntu.bmp", 590, main_iconY + 230, (int (*)(void *))launcher, (void *)"/switchroot/ubuntu/coreboot.rom");
		//
		u64 iconrowY = low_icons-5;
		u64 iconrowX = 130;
		u64 iconrowXS = 250;
		create(menu_0, "Icons/Lockpick_RCM.bmp", iconrowX, iconrowY, (int (*)(void *))launcher, (void *)"/StarDust/payloads/Lockpick_RCM.bin");
		iconrowX = iconrowX + iconrowXS;
		create(menu_0, "Icons/TegraEX.bmp", iconrowX, iconrowY, (int (*)(void *))launcher, (void *)"/StarDust/payloads/TegraExplorer.bin");
		iconrowX = iconrowX + iconrowXS;
		iconrowX = iconrowX + 80;
		if (Incac == 0)
			create(menu_0, "Icons/Incognito.bmp", iconrowX, iconrowY, (int (*)(void *))Incognito, (void *)"1");
		else
			create(menu_0, "Icons/Incognito.bmp", iconrowX, iconrowY, (int (*)(void *))tool_Menus, (void *)4);

		iconrowX = iconrowX + iconrowXS;
		create(menu_0, "Icons/zHekate.bmp", iconrowX, iconrowY, (int (*)(void *))hekateOFW, (void *)0);
		//	gui_menu_append_entry(menu_0,gui_create_menu_entry("",theme("Icons/Incognito.bmp"),iconrowX+700, iconrowY, 200 , 200,(int (*)(void *))tool_Menus, (void*)6));

		create(menu_0, "Icons/SD.bmp", 10, low_icons, tool_extr_rSD, NULL);
		//create(menu_0, "Icons/rcm.bmp", 700, low_icons, (int (*)(void *))launcher, (void *)"payload.bin");
		if (iamsafe == 0)
			create(menu_0, "Icons/themes.bmp", 605, low_icons, tool_menu_rem, NULL);
		//			gui_menu_append_entry(menu_0,gui_create_menu_entry("",theme("Icons/Reinx-off.bmp"),366, main_iconY, 300 , 300, NULL, NULL));
		//call menu 1
		create(menu_0, "Icons/gear.bmp", 1200, low_icons, (int (*)(void *))tool_Menus, (void *)1);
		menu = menu_0;
	}

	if (menus == 1 || StarUp)
	{
		static int start_point = 0;
		if (menu_1 == NULL || StarUp)
		{
			menu_1 = gui_menu_create("ArgonNX", "back-set.bmp");
			//list custom skins
			AThemes_list(menu_1, 80, 90);
		//Create SXOS transfer buttons
			if (sd_file_exists("emuMMC/EF00/eMMC/00") & sd_file_exists("emuMMC/EF00/eMMC/boot0") & sd_file_exists("emuMMC/EF00/eMMC/boot1"))
				create(menu_1, "Icons/swap.bmp", 380, 600, (int (*)(void *))tool_emu, (void *)66);

			if (sd_file_exists("sxos/emunand/full.00.bin") & sd_file_exists("sxos/emunand/boot0.bin") & sd_file_exists("sxos/emunand/boot1.bin"))
				create(menu_1, "Icons/Transfer.bmp", 380, 600, (int (*)(void *))tool_emu, (void *)33); //- 80, - 500

			if (!sd_file_exists("emummc/emummc.ini"))
				create(menu_1, "Icons/link_hide.bmp", 580, 600, (int (*)(void *))tool_emu, (void *)99);

			//call menu 5 create(menu_1, "Icons/HB.bmp", 790, 600, (int (*)(void *))tool_Menus, (void *)5);

			start_point = menu_1->next_entry;
		}
		else
		{
			menu_1->next_entry = start_point + 1;
		}

			//draw power
			create(menu_1, "Icons/power.bmp", 80, 500, tool_power_off, NULL);

		//dinamic
		/*
			menu->entries[menu->next_entry] = menu_entry;
			menu->next_entry++;
		*/
		//draw autoboot
		if (sd_file_exists("StarDust/autobootecho.txt"))
			create(menu_1, "Icons/Auton.bmp", 1050, 600, (int (*)(void *))Autoboot, (void *)0);
		else
			create(menu_1, "Icons/Autoff.bmp", 1050, 600, (int (*)(void *))Autoboot, (void *)1);

		//draw Ulaunch
		if (sd_file_exists("StarDust/flags/ulaunch.flag"))
			create(menu_1, "Icons/ulon.bmp", 850, 600, (int (*)(void *))uLaunch, (void *)0);
		else
			create(menu_1, "Icons/uloff.bmp", 850, 600, (int (*)(void *))uLaunch, (void *)1);
		/*
		*/

		//draw CFW select
		if (isAMS)
			create(menu_1, "Icons/serv-ams.bmp", 350, 165, (int (*)(void *))serv_CFW, (void *)0);
		else
			create(menu_1, "Icons/serv-sxos.bmp", 350, 165, (int (*)(void *))serv_CFW, (void *)1);

		//services
		//		gui_menu_append_entry(menu_1,gui_create_menu_entry_no_bitmap("Services",servX+30, servY-10, 150, 100, NULL, NULL));
		servstep = 0;
		serv_display(menu_1, "420000000000000E", "FTP");
		serv_display(menu_1, "420000000007E51A", "TeslaOVL");
		serv_display(menu_1, "010000000000bd00", "MissionCon");
		serv_display(menu_1, "0100000000000081", "nx-btred");
		serv_display(menu_1, "690000000000000D", "Sys-Con");
		serv_display(menu_1, "0100000000000352", "Emuiio");
		serv_display(menu_1, "4200000000000010", "LanPlay");
		serv_display(menu_1, "00FF0000A53BB665", "SysDVR");
		serv_display(menu_1, "0000000000534C56", "ReverseNX");
		serv_display(menu_1, "00FF747765616BFF", "sys-tweak");
		serv_display(menu_1, "00FF0000636C6BFF", "Sys-Clk");
		serv_display(menu_1, "430000000000000B", "sys-botbase");
		serv_display(menu_1, "4200000000000000", "SysTune");
		serv_display(menu_1, "420000000000000B", "SysPlay");
		//serv_display(menu_1,"0100000000000BEF","Disk-USB");
		//serv_display(menu_1,"0100000000000069","ReiSpoof");
		//serv_display(menu_1,"0100000000000FAF","HDI");

		//themes for cfw
		u32 temX = 310;
		u32 temY = 440;
		u32 temS = 300;
		if (!sd_file_exists("atmosphere/contents/0100000000001000/fsmitm.flag"))
			create(menu_1, "Icons/sw-off.bmp", temX, temY, (int (*)(void *))tool_Themes_on, (void *)"atmosphere");
		else
			create(menu_1, "Icons/sw-on.bmp", temX, temY, (int (*)(void *))tool_Themes_off, (void *)"atmosphere");
		gui_menu_append_entry(menu_1, gui_create_menu_entry_no_bitmap("AMS", temX + 30, temY + 30, 150, 100, NULL, NULL));
		temX = temX + temS;

		if (!sd_file_exists("sxos/titles/0100000000001000/fsmitm.flag"))
			create(menu_1, "Icons/sw-off.bmp", temX, temY, (int (*)(void *))tool_Themes_on, (void *)"sxos");
		else
			create(menu_1, "Icons/sw-on.bmp", temX, temY, (int (*)(void *))tool_Themes_off, (void *)"sxos");
		gui_menu_append_entry(menu_1, gui_create_menu_entry_no_bitmap("SXOS", temX + 30, temY + 30, 150, 100, NULL, NULL));
		temX = temX + temS;

		if (isAMS)
		{
			if (!sd_file_exists("atmosphere/contents/0100000000001013/exefs.nsp"))
				create(menu_1, "Icons/sw-off.bmp", temX, temY, (int (*)(void *))tool_Themes_on, (void *)"Profile");
			else
				create(menu_1, "Icons/sw-on.bmp", temX, temY, (int (*)(void *))tool_Themes_off, (void *)"Profile");
		}
		else
		{
			if (!sd_file_exists("sxos/titles/0100000000001013/exefs.nsp"))
				create(menu_1, "Icons/sw-off.bmp", temX, temY, (int (*)(void *))tool_Themes_on, (void *)"Profile");
			else
				create(menu_1, "Icons/sw-on.bmp", temX, temY, (int (*)(void *))tool_Themes_off, (void *)"Profile");
		}

		gui_menu_append_entry(menu_1, gui_create_menu_entry_no_bitmap("Profile", temX + 30, temY + 30, 150, 100, NULL, NULL));

		//Ajustar brillo
		if (!sd_file_exists("StarDust/flags/b50.flag"))
			create(menu_1, "Icons/day.bmp", 1200, 100, (int (*)(void *))medislay, (void *)0);
		else
			create(menu_1, "Icons/nay.bmp", 1200, 100, (int (*)(void *))medislay, (void *)0);
		menu = menu_1;
	}

	if (menus == 3)
	{
		menu_3 = gui_menu_create("ArgonNX", "back-mem.bmp");
		//Menu Here
		//call
		create(menu_3, "Icons/gear.bmp", 1200, low_icons, (int (*)(void *))tool_Menus, (void *)1);
		menu = menu_3;
	}

	if (menus == 4)
	{
		menu_4 = gui_menu_create("ArgonNX", "back-inc.bmp");

		//		incognito togle
		//Getinfo from text
		u32 sys_inc = 0;
		u32 emu_inc = 0;

		//get blank or not
		u32 sys_blk = 0;
		u32 emu_blk = 0;

		u32 xcol = 90;
		u32 rowinc = 900 + xcol;
		u32 colinc = 150;
		u32 rowsepar = 50;
		u32 colsepar = 100;

		if (sd_mount())
		{
			if (sd_file_exists("atmosphere/config/exosphere.ini"))
			{
				void *buf = sd_file_read("atmosphere/config/exosphere.ini");
				buffer_blk = buf;
			}
			if (strstr(buffer_blk, "blank_prodinfo_emummc=1") != NULL)
			{
				emu_blk = 1;
			}

			if (strstr(buffer_blk, "blank_prodinfo_sysmmc=1") != NULL)
			{
				sys_blk = 1;
			}

			if (sd_file_exists("emummc/emummc.ini"))
			{
				if (!sd_file_exists("StarDust/emunand_serial.txt"))
				{
					sd_save_to_file("", 0, "StarDust/emunand_serial.txt");
					Incognito("1");
				}

				void *buf = sd_file_read("StarDust/emunand_serial.txt");
				emuserial = buf;

				if (strstr(emuserial, "XAW0000000000") != NULL)
				{
					emu_inc = 1;
				}

				if (sd_file_exists("atmosphere/config/exosphere.ini"))
				{
					if (emu_blk == 1)
					{
						create(menu_4, "Icons/sw-on.bmp", 1000, 100, (int (*)(void *))Incognito, (void *)"DBE");
					}
					else
					{
						create(menu_4, "Icons/sw-off.bmp", 1000, 100, (int (*)(void *))Incognito, (void *)"BE");
					}
					gui_menu_append_entry(menu_4, gui_create_menu_entry_no_bitmap("Blank_Prodinfo", 1020, 90, 150, 100, NULL, NULL));
				}

				gui_menu_append_entry(menu_4, gui_create_menu_entry_no_bitmap("SysNand", 170, 200, 150, 100, NULL, NULL));
				gui_menu_append_entry(menu_4, gui_create_menu_entry_no_bitmap("EmuNand", 970, 200, 150, 100, NULL, NULL));

				rowinc = rowinc - rowsepar;
				colinc = colinc + colsepar;
				gui_menu_append_entry(menu_4, gui_create_menu_entry_no_bitmap(emuserial, rowinc + 30, colinc - 20, 150, 100, NULL, NULL));
				if (!sd_file_exists("prodinfo_emunand.bin"))
					create(menu_4, "Icons/inc3.bmp", rowinc, colinc, (int (*)(void *))Incognito, (void *)"2");
				else
					create(menu_4, "Icons/inc0-off.bmp", rowinc, colinc, NULL, NULL);

				rowinc = rowinc - rowsepar;
				colinc = colinc + colsepar;
				if (emu_inc == 0)
					create(menu_4, "Icons/inc4.bmp", rowinc, colinc, (int (*)(void *))Incognito, (void *)"3");
				else
					create(menu_4, "Icons/inc1-off.bmp", rowinc, colinc, NULL, NULL);

				rowinc = rowinc - rowsepar;
				colinc = colinc + colsepar;
				if (sd_file_exists("prodinfo_emunand.bin"))
					create(menu_4, "Icons/inc5.bmp", rowinc, colinc, (int (*)(void *))Incognito, (void *)"4");
				else
				{
					create(menu_4, "Icons/inc2-off.bmp", rowinc, colinc, NULL, NULL);
					if (emu_inc == 1)
					{
						gui_menu_append_entry(menu_4, gui_create_menu_entry_no_bitmap("Missing", rowinc + 30, colinc + 100, 150, 100, NULL, NULL));
						gui_menu_append_entry(menu_4, gui_create_menu_entry_no_bitmap("/prodinfo_emunand.bin", rowinc + 30, colinc + 120, 150, 100, NULL, NULL));
					}
				}
			}
			else
			{
				rowinc = 550;
				rowsepar = 0;
			}

			rowinc = 0 + xcol;
			colinc = 150;
			rowsepar = 50;
			colsepar = 100;
			if (sd_file_exists("StarDust/sysnand_serial.txt"))
			{

				void *buf = sd_file_read("StarDust/sysnand_serial.txt");
				sysserial = buf;
				if (strstr(sysserial, "XAW0000000000") != NULL)
					sys_inc = 1;

				if (sd_file_exists("emummc/emummc.ini"))
				{
					void *buf = sd_file_read("StarDust/emunand_serial.txt");
					emuserial = buf;
				}
				else
				{
					rowinc = 550;
					rowsepar = 0;
				}

				if (strstr(emuserial, "XAW0000000000") != NULL)
					emu_inc = 1;

				rowinc = rowinc + rowsepar;
				colinc = colinc + colsepar;

				if (sd_file_exists("atmosphere/config/exosphere.ini"))
				{
					if (sys_blk == 1)
					{
						create(menu_4, "Icons/sw-on.bmp", rowinc - 8 - rowsepar, 100, (int (*)(void *))Incognito, (void *)"DBS");
					}
					else
					{
						create(menu_4, "Icons/sw-off.bmp", rowinc - 8 - rowsepar, 100, (int (*)(void *))Incognito, (void *)"BS");
					}
					gui_menu_append_entry(menu_4, gui_create_menu_entry_no_bitmap("Blank_Prodinfo", rowinc + 10 - rowsepar, 90, 150, 100, NULL, NULL));
				}
				gui_menu_append_entry(menu_4, gui_create_menu_entry_no_bitmap(sysserial, rowinc + 40, colinc - 20, 150, 100, NULL, NULL));
				if (!sd_file_exists("prodinfo_sysnand.bin"))
					create(menu_4, "Icons/inc0.bmp", rowinc, colinc, (int (*)(void *))Incognito, (void *)"5");
				else
					create(menu_4, "Icons/inc0-off.bmp", rowinc, colinc, NULL, NULL);

				rowinc = rowinc + rowsepar;
				colinc = colinc + colsepar;
				if (sys_inc == 0)
					create(menu_4, "Icons/inc1.bmp", rowinc, colinc, (int (*)(void *))Incognito, (void *)"6");
				else
					create(menu_4, "Icons/inc1-off.bmp", rowinc, colinc, NULL, NULL);

				rowinc = rowinc + rowsepar;
				colinc = colinc + colsepar;

				if (sd_file_exists("prodinfo_sysnand.bin"))
					create(menu_4, "Icons/inc2.bmp", rowinc, colinc, (int (*)(void *))Incognito, (void *)"7");
				else
				{
					create(menu_4, "Icons/inc2-off.bmp", rowinc, colinc, NULL, NULL);
					if (sys_inc == 1)
					{
						gui_menu_append_entry(menu_4, gui_create_menu_entry_no_bitmap("Missing", rowinc + 30, colinc + 100, 150, 100, NULL, NULL));
						gui_menu_append_entry(menu_4, gui_create_menu_entry_no_bitmap("/prodinfo_sysnand.bin", rowinc + 30, colinc + 120, 150, 100, NULL, NULL));
					}
				}
			}
			else
			{
				gui_menu_append_entry(menu_4, gui_create_menu_entry_no_bitmap("Biskeys error, try again", rowinc + 30, colinc - 10, 150, 100, NULL, NULL));
			}

			//		gui_menu_append_entry(menu_4,gui_create_menu_entry("",theme("Icons/arrowl.bmp"),200, low_icons, 70, 70,(int (*)(void *))tool_Menus, (void*)1));
		}

		menu = menu_4;
	}

	if (menus == 5)//disabled
	{
		menu_5 = gui_menu_create("ArgonNX", "back-exp.bmp");
/*
		//List Files And folders
		char *HBpath = "/switch";
		char *folder = listfol(HBpath, "*", true);
		u32 r = 0;
		u32 i = 0;
		u32 f = 0;
		u32 y = 60;
		u32 x = 150;
		u32 space = 80;

		//display folder
		gui_menu_append_entry(menu_5, gui_create_menu_entry_no_bitmap(HBpath, strlen(HBpath) * 8 - 40 + 150, 30, 5, 5, NULL, NULL));
		gui_menu_append_entry(menu_5, gui_create_menu_entry_no_bitmap("Homebrew", strlen(HBpath) * 8 - 40 + 150, 50, 5, 5, NULL, NULL));
		gui_menu_append_entry(menu_5, gui_create_menu_entry_no_bitmap("Homebrew      Toogle.", strlen(HBpath) * 8 - 40 + 890, 690, 5, 5, NULL, NULL));

		while (folder[r * 256])
		{
			if (f >= 33)
			{
				gui_menu_append_entry(menu_5, gui_create_menu_entry_no_bitmap("More...", 1100, 690, 150, 100, NULL, NULL));
				break;
			}
			if (strlen(&folder[r * 256]) <= 100)
			{
				char folname[20];
				char *str = &folder[r * 256];
				int h;
				for (h = 0; h < 19; h++)
					folname[h] = str[h];
				folname[h] = '\0';
				if ((strstr(folname, ".overlays") == NULL) & (strstr(folname, "XXX") == NULL))
				{
					char *source_fol = (char *)malloc(256);
					char *source_folder = (char *)malloc(256);
					strcpy(source_fol, "/switch");
					strcat(source_fol, "/");
					strcat(source_fol, folname);
					strcpy(source_folder, source_fol);
					strcat(source_fol, "/hide.flag");
					if (sd_file_exists(source_fol))
					{
						f_chmod(source_folder, AM_ARC, AM_ARC);
						create(menu_5, "Icons/sw-off.bmp", x, y + 30, (int (*)(void *))HBhide, (void *)source_fol);
					}
					else
					{
						f_chmod(source_folder, AM_RDO, AM_RDO | AM_ARC);
						create(menu_5, "Icons/sw-on.bmp", x, y + 30, (int (*)(void *))HBhide, (void *)source_fol);
					}
					//					gui_menu_append_entry(menu_5,gui_create_menu_entry_no_bitmap(folname, x+strlen(folname)*8-80, y+25, 150, 0, NULL, NULL));
					gui_menu_append_entry(menu_5, gui_create_menu_entry_no_bitmap(folname, x + 25, y + 25, 150, 0, NULL, NULL));
					y = y + space;
				}

				f++;
				i++;
				if ((i == 8) || (i == 15))
				{
					y = 60;
					if (i == 15)
					{
						i = 0;
						x = 940;
					}
					else
						x = 545;
				}
			}
			r++;
		}
*/
		//call menu 1
		create(menu_5, "Icons/gear.bmp", 1200, low_icons, (int (*)(void *))tool_Menus, (void *)1);
		menu = menu_5;
	}
}

/* Init needed menus for ArgonNX */
void gui_init_argon_menu(void)
{
	SDStrap();
	/* Init pool for menu */
	gui_menu_pool_init();
	//	change_brightness(1);
	//	menu = gui_menu_create("ArgonNX",main_menu);
	//main menu 0-------------------------------------
	switch (main_menu)
	{
	case 0:
	{
		if (menu_0 == NULL)
			pre_load_menus(main_menu, 0);
		if (menu_0 == menu)
			break;
		menu = menu_0;
	}
	break;

	case 1:
	{
		if (menu_1 == NULL)
			pre_load_menus(main_menu, 0);
		if (menu_1 == menu)
			break;
		menu = menu_1;
	}
	break;

	case 3:
	{
		if (menu_3 == NULL)
			pre_load_menus(main_menu, 0);
		if (menu_3 == menu)
			break;
		menu = menu_3;
	}
	break;

	case 4:
	{
		if (menu_4 == NULL)
			pre_load_menus(main_menu, 0);
		if (menu_4 == menu)
			break;
		menu = menu_4;
	}
	break;

	case 5:
	{
		if (menu_5 == NULL)
			pre_load_menus(main_menu, 0);
		if (menu_5 == menu)
			break;
		menu = menu_5;
	}
	break;
	}

	static_menu_elements(menu);
	/* Start menu_1 */
	change_brightness(0);
	gui_menu_open(menu);
	/* Clear all entries and menus */
	gui_menu_pool_cleanup();
}

int static_menu_elements(gui_menu_t *menu)
{
	if (main_menu != 0)
		create(menu, "Icons/home.bmp", 10, low_icons, (int (*)(void *))tool_Menus, (void *)0);
		//create(menu, "/StarDust/skins/xbox/Icons/home.bmp", 10, low_icons, (int (*)(void *))tool_Menus, (void *)0);
/*
	if (main_menu != 1)
		create(menu, "Icons/power.bmp", 605, low_icons, tool_power_off, NULL);

*/
	
	gui_menu_append_entry(menu, gui_create_menu_entry("", sd_file_read(""), 0, 0, 35, 35, (int (*)(void *))screenshot, NULL));

	//battery
	u32 battPercent = 0;
	max17050_get_property(MAX17050_RepSOC, (int *)&battPercent);
	u32 battimgper = (battPercent >> 8) & 0xFF;
	u32 batY = 5;
	u32 batX = 1200;
	if ((battimgper <= 100) & (battimgper >= 76))
		create(menu, "Icons/bat1.bmp", batX, batY, (int (*)(void *))bat_show, (void *)battimgper);
	if ((battimgper <= 75) & (battimgper >= 51))
		create(menu, "Icons/bat2.bmp", batX, batY, (int (*)(void *))bat_show, (void *)battimgper);
	if ((battimgper <= 50) & (battimgper >= 26))
		create(menu, "Icons/bat3.bmp", batX, batY, (int (*)(void *))bat_show, (void *)battimgper);
	if ((battimgper <= 25) & (battimgper >= 5))
		create(menu, "Icons/bat4.bmp", batX, batY, (int (*)(void *))bat_show, (void *)battimgper);
	if (battimgper <= 4)
		create(menu, "Icons/bat5.bmp", batX, batY, (int (*)(void *))bat_show, (void *)battimgper);
	return 0;
}

int tool_reboot_rcm(void *param)
{
	reboot_rcm();
	return 0;
}

int tool_power_off(void *param)
{
	gui_menu_pool_cleanup();
	power_off();
	return 0;
}

//eject sd card
int tool_extr_rSD(void *param)
{
	SDStrap();
	gui_menu_pool_cleanup();
	gfx_swap_buffer(&g_gfx_ctxt);
	change_brightness(0);
	g_gfx_con.scale = 3;
	gfx_con_setpos(&g_gfx_con, 160, 100);
	gfx_printf(&g_gfx_con, "Ya puedes extraer la SD, Al terminar\n");
	gfx_con_setpos(&g_gfx_con, 230, 130);
	gfx_printf(&g_gfx_con, "Pon la SD y presiona POWER\n\n");
	gfx_con_setpos(&g_gfx_con, 110, 600);
	gfx_printf(&g_gfx_con, "You can now extract the SD, When you finish\n");
	gfx_con_setpos(&g_gfx_con, 230, 630);
	gfx_printf(&g_gfx_con, "Put the SD and press POWER\n");
	gfx_swap_buffer(&g_gfx_ctxt);
	btn_wait_timeout(10000, BTN_POWER);
	display_backlight_brightness(0, 1000);
	/* Clear all entries and menus */
	BootStrapNX();
	return 0;
}

//Emu tool
int tool_emu(u32 status)
{
	SDStrap();

	//give sxos emu to ams
	if (status == 33)
	{
		f_unlink("sxos/eMMC");
		change_brightness(1);
		f_mkdir("emuMMC");
		f_mkdir("emuMMC/EF00");
		f_rename("/sxos/emunand", "/emuMMC/EF00/eMMC");
		f_rename("/Emutendo", "/emuMMC/EF00/Nintendo");

		f_rename("emuMMC/EF00/eMMC/boot0.bin", "emuMMC/EF00/eMMC/boot0");
		f_rename("emuMMC/EF00/eMMC/boot1.bin", "emuMMC/EF00/eMMC/boot1");

		f_rename("emuMMC/EF00/eMMC/full.00.bin", "emuMMC/EF00/eMMC/00");
		f_rename("emuMMC/EF00/eMMC/full.01.bin", "emuMMC/EF00/eMMC/01");
		f_rename("emuMMC/EF00/eMMC/full.02.bin", "emuMMC/EF00/eMMC/02");
		f_rename("emuMMC/EF00/eMMC/full.03.bin", "emuMMC/EF00/eMMC/03");
		f_rename("emuMMC/EF00/eMMC/full.04.bin", "emuMMC/EF00/eMMC/04");
		f_rename("emuMMC/EF00/eMMC/full.05.bin", "emuMMC/EF00/eMMC/05");
		f_rename("emuMMC/EF00/eMMC/full.06.bin", "emuMMC/EF00/eMMC/06");
		f_rename("emuMMC/EF00/eMMC/full.07.bin", "emuMMC/EF00/eMMC/07");
		f_rename("emuMMC/EF00/eMMC/full.08.bin", "emuMMC/EF00/eMMC/08");
		f_rename("emuMMC/EF00/eMMC/full.09.bin", "emuMMC/EF00/eMMC/09");
		f_rename("emuMMC/EF00/eMMC/full.10.bin", "emuMMC/EF00/eMMC/10");
		f_rename("emuMMC/EF00/eMMC/full.11.bin", "emuMMC/EF00/eMMC/11");
		f_rename("emuMMC/EF00/eMMC/full.12.bin", "emuMMC/EF00/eMMC/12");
		f_rename("emuMMC/EF00/eMMC/full.13.bin", "emuMMC/EF00/eMMC/13");
		f_rename("emuMMC/EF00/eMMC/full.14.bin", "emuMMC/EF00/eMMC/14");
		if (sd_file_exists("emuMMC/EF00/eMMC/00") & sd_file_exists("emuMMC/EF00/eMMC/boot0") & sd_file_exists("emuMMC/EF00/eMMC/boot1"))
		{
			f_unlink("emummc/emummc.bak");
			f_rename("emummc/emummc.ini", "emummc/emummc.bak");
			f_mkdir("emummc");
			FIL fp;
			f_open(&fp, "emummc/emummc.ini", FA_WRITE | FA_CREATE_ALWAYS);
			f_puts("[emummc]\n", &fp);
			f_puts("enabled=1\n", &fp);
			f_puts("sector=0x0\n", &fp);
			f_puts("path=emuMMC/EF00\n", &fp);
			f_puts("id=0x0000\n", &fp);
			f_puts("nintendo_path=emuMMC/EF00/Nintendo\n", &fp);
			f_close(&fp);
			sd_save_to_file("", 0, "emuMMC/EF00/file_based");
			pre_load_menus(1, 1);
		}
	}

	//return emunand to sxos
	if (status == 66)
	{
		f_unlink("sxos/emunand");
		change_brightness(1);
		f_rename("/emuMMC/EF00/eMMC", "/sxos/emunand");

		f_rename("sxos/emunand/boot0", "sxos/emunand/boot0.bin");
		f_rename("sxos/emunand/boot1", "sxos/emunand/boot1.bin");

		f_rename("sxos/emunand/00", "sxos/emunand/full.00.bin");
		f_rename("sxos/emunand/01", "sxos/emunand/full.01.bin");
		f_rename("sxos/emunand/02", "sxos/emunand/full.02.bin");
		f_rename("sxos/emunand/03", "sxos/emunand/full.03.bin");
		f_rename("sxos/emunand/04", "sxos/emunand/full.04.bin");
		f_rename("sxos/emunand/05", "sxos/emunand/full.05.bin");
		f_rename("sxos/emunand/06", "sxos/emunand/full.06.bin");
		f_rename("sxos/emunand/07", "sxos/emunand/full.07.bin");
		f_rename("sxos/emunand/08", "sxos/emunand/full.08.bin");
		f_rename("sxos/emunand/09", "sxos/emunand/full.09.bin");
		f_rename("sxos/emunand/10", "sxos/emunand/full.10.bin");
		f_rename("sxos/emunand/11", "sxos/emunand/full.11.bin");
		f_rename("sxos/emunand/12", "sxos/emunand/full.12.bin");
		f_rename("sxos/emunand/13", "sxos/emunand/full.13.bin");
		f_rename("sxos/emunand/14", "sxos/emunand/full.14.bin");
		f_rename("/emuMMC/EF00/Nintendo", "/Emutendo");
		if (sd_file_exists("sxos/emunand/full.00.bin") & sd_file_exists("sxos/emunand/boot0.bin") & sd_file_exists("sxos/emunand/boot1.bin"))
		{
			f_unlink("emummc/emummc.ini");
			f_rename("emummc/emummc.bak", "emummc/emummc.ini");
			f_unlink("emuMMC/EF00/file_based");
			f_unlink("/emuMMC/EF00");
			f_unlink("emummc");
			retir = 0;
			pre_load_menus(1, 1);
		}

	}

	//link sxos hide partition to ams
	if (status == 99)
	{
		change_brightness(1);
		f_mkdir("emummc");
		FIL fp;
		f_open(&fp, "emummc/emummc.ini", FA_WRITE | FA_CREATE_ALWAYS);
		f_puts("[emummc]\n", &fp);
		f_puts("enabled=1\n", &fp);
		f_puts("sector=0x2\n", &fp);
		f_puts("nintendo_path=Emutendo\n", &fp);
		f_close(&fp);
		pre_load_menus(1, 1);
	}

	if (status == 1)
	{
		char *str1 = sd_file_read("emummc/emummc.ini");
		char *payload_wo_bin = str_replace(str1, "enabled=0", "enabled=1");
		FIL op;
		f_open(&op, "emummc/emummc.ini", FA_READ);
		u32 size = f_size(&op);
		f_close(&op);
		printerCU(payload_wo_bin,"",5000);
//		payload_wo_bin[size]='\0';
//		payload_wo_bin[size]='\n';
		payload_wo_bin[size]=0;
		payload_wo_bin[size]='\n';
		//printerCU(payload_wo_bin,"",5000);

		sd_save_to_file(payload_wo_bin, size, "emummc/emummc.ini");
		retir = 2;
		pre_load_menus(0, 0);
	}

	if (status == 0)
	{
		char *str1 = sd_file_read("emummc/emummc.ini");
		char *payload_wo_bin = str_replace(str1, "enabled=1", "enabled=0");
		FIL op;
		f_open(&op, "emummc/emummc.ini", FA_READ);
		u32 size = f_size(&op);
		f_close(&op);
		printerCU(payload_wo_bin,"",5000);
//		payload_wo_bin[size]='\0';
//		payload_wo_bin[size]='\n';
		payload_wo_bin[size]=0;
		payload_wo_bin[size]='\n';
		sd_save_to_file(payload_wo_bin, size, "emummc/emummc.ini");
		retir = 1;
		//printerCU(payload_wo_bin,"",5000);
		pre_load_menus(0, 0);
	}

	gui_init_argon_menu();
	return 0;
}

int tool_Menus(u32 param)
{
	SDStrap();
	//set menu number
	main_menu = param;

	//gui_menu_pool_cleanup();
	gui_init_argon_menu();
	return 0;
}

void tool_servises_on(char *title)
{
	SDStrap();
	change_brightness(1);
	char *path = (char *)malloc(256);
	if (isAMS)
	{
		strcpy(path, "atmosphere/contents/");
		strcat(path, title);
		strcat(path, "/flags");
		f_mkdir(path);
		strcat(path, "/boot2.flag");
		sd_save_to_file("", 0, path);
	}
	else
	{
		strcpy(path, "sxos/titles/");
		strcat(path, title);
		strcat(path, "/flags");
		f_mkdir(path);
		strcat(path, "/boot2.flag");
		sd_save_to_file("", 0, path);
	}
	pre_load_menus(1, 0);
	gui_init_argon_menu();
}

void tool_servises_off(char *title)
{
	SDStrap();
	change_brightness(1);
	char *path = (char *)malloc(256);
	if (isAMS)
	{
		strcpy(path, "atmosphere/contents/");
		strcat(path, title);
		strcat(path, "/flags/boot2.flag");
		f_unlink(path);
	}
	else
	{
		strcpy(path, "sxos/titles/");
		strcat(path, title);
		strcat(path, "/flags/boot2.flag");
		f_unlink(path);
	}
	pre_load_menus(1, 0);
	gui_init_argon_menu();
}

//Themes ON
void tool_Themes_on(char *cfw)
{
	SDStrap();
	char *path = (char *)malloc(256);
	if (strstr(cfw, "sxos") != NULL)
	{
		strcpy(path, cfw);
		moverall("/sxos/titles/0100000000001000/sfmor", "/sxos/titles/0100000000001000/romfs", "*", "");
		strcat(path, "/titles/0100000000001000/fsmitm.flag");
		sd_save_to_file("", 0, path);
	}

	if (strstr(cfw, "Profile") != NULL)
	{
		if (isAMS)
			f_move("/atmosphere/contents/0100000000001013/exefs.off", "/atmosphere/contents/0100000000001013/exefs.nsp");
		else
			f_move("/sxos/titles/0100000000001013/exefs.off", "/sxos/titles/0100000000001013/exefs.nsp");
	}

	if (strstr(cfw, "atmosphere") != NULL)
	{
		moverall("/atmosphere/contents/0100000000001000/sfmor", "/atmosphere/contents/0100000000001000/romfs", "*", "");
		//		copyarall("/atmosphere/contents/0100000000001000/sfmor", "/atmosphere/contents/0100000000001000/romfs", "*","");
		sd_save_to_file("", 0, "atmosphere/contents/0100000000001000/fsmitm.flag");
	}
	pre_load_menus(1, 0);
	gui_init_argon_menu();
}

//Themes OFF
void tool_Themes_off(char *cfw)
{
	SDStrap();
	if (strstr(cfw, "sxos") != NULL)
	{
		moverall("/sxos/titles/0100000000001000/romfs", "/sxos/titles/0100000000001000/sfmor", "*", "");
		f_unlink("/sxos/titles/0100000000001000/fsmitm.flag");
	}

	if (strstr(cfw, "Profile") != NULL)
	{
		if (isAMS)
			f_move("/atmosphere/contents/0100000000001013/exefs.nsp", "/atmosphere/contents/0100000000001013/exefs.off");
		else
			f_move("/sxos/titles/0100000000001013/exefs.nsp", "/sxos/titles/0100000000001013/exefs.off");
	}

	if (strstr(cfw, "atmosphere") != NULL)
	{
		//			deleteall("/atmosphere/contents/0100000000001000/romfs", "*", "");
		moverall("/atmosphere/contents/0100000000001000/romfs", "/atmosphere/contents/0100000000001000/sfmor", "*", "");
		f_unlink("atmosphere/contents/0100000000001000/fsmitm.flag");
	}
	pre_load_menus(1, 0);
	gui_init_argon_menu();
}

//safe boot
int tool_menu_rem(void *param)
{
	SDStrap();
	change_brightness(1);
	f_unlink("/atmosphere/contents/0100000000001000/fsmitm.flag");
	f_unlink("/atmosphere/contents/0100000000001000/romfs_metadata.bin");
	deleteall("/atmosphere/contents/0100000000001000/romfs/lyt", "*", "Delete 0100000000001000");
	f_unlink("/SXOS/titles/0100000000001000/fsmitm.flag");
	f_unlink("/sxos/titles/0100000000001000/romfs_metadata.bin");
	f_unlink("/sxos/titles/0100000000000034-OFF/exefs.nsp");
	f_unlink("/sxos/titles/0100000000000034-OFF");
	f_rename("/sxos/titles/0100000000000034", "/sxos/titles/0100000000000034-OFF");
	f_unlink("/sxos/titles/0100000000000034/exefs.nsp");
	//this is a panic option so i will disable the servises also
	char *folder_ams = listfol("/atmosphere/contents", "*", true);
	u32 r = 0;
	while (folder_ams[r * 256])
	{
		if (strlen(&folder_ams[r * 256]) <= 100)
		{
			char *source_ams = (char *)malloc(256);
			strcpy(source_ams, "/atmosphere/contents/");
			strcat(source_ams, &folder_ams[r * 256]);
			strcat(source_ams, "/flags/boot2.flag");
			f_unlink(source_ams);
		}
		r++;
	}

	char *folder_sxos = listfol("/sxos/titles", "*", true);
	u32 e = 0;
	while (folder_sxos[e * 256])
	{
		if (strlen(&folder_sxos[e * 256]) <= 100)
		{
			char *source_sxos = (char *)malloc(256);
			strcpy(source_sxos, "/sxos/titles/");
			strcat(source_sxos, &folder_sxos[e * 256]);
			strcat(source_sxos, "/flags/boot2.flag");
			f_unlink(source_sxos);
		}
		e++;
	}
	sd_save_to_file("", 0, "StarDust/flags/IamSafe.flag");
	sd_save_to_file("#Safeboot flag", 13, "fixer.del");
	launcher("payload.bin");
	return 0;
}

//Themes///// copy
int tool_theme(char *param)
{
	SDStrap();
	change_brightness(1);
	saveTheme(param);
	pre_load_menus(1, 1);
	gui_init_argon_menu();
	return 0;
}

int bat_show(u32 percent)
{
	gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0xFFCCCCCC, 0xFF191414);
	gfx_con_setpos(&g_gfx_con, 1205, 15);
	gfx_printf(&g_gfx_con, "%k%d%%%k", 0xFF00FF22, percent, 0xFFCCCCCC);

	//      gfx_printf(&g_gfx_con, "%k%d%%%k", 0xFF00FF22,percent,0xFFCCCCCC);
	gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0, 0xFF191414);
	//		for (u32 i =10; i < 720; i = i +20){					gfx_con_setpos(&g_gfx_con, 10, i);	gfx_printf(&g_gfx_con, " %d -------------------------------------------------------------", i);		}
	return 0;
}

void serv_CFW(int cfw)
{
	SDStrap();
	change_brightness(1);
	isAMS = cfw;
	pre_load_menus(1, 0);
	gui_init_argon_menu();
}

void serv_display(gui_menu_t *menu, char *titleid, char *name)
{
	SDStrap();
	static u32 servYF = 250;
	static u32 servXF = 160;

	static u32 servX = 160;
	static u32 servY = 250;

	static u32 sepaserv = 70;
	if (servstep == 0){
		servY = servYF;
		servX = servXF;
	}
		
	if (servstep > 9)
		return;

	char *path = (char *)malloc(256);
	if (isAMS)
		strcpy(path, "atmosphere/contents/");
	else
		strcpy(path, "sxos/titles/");
	strcat(path, titleid);
	strcat(path, "/exefs.nsp");

	char *flagpath = (char *)malloc(256);
	if (isAMS)
		strcpy(flagpath, "atmosphere/contents/");
	else
		strcpy(flagpath, "sxos/titles/");
	strcat(flagpath, titleid);
	strcat(flagpath, "/flags/boot2.flag");

	if (!sd_file_exists(path))
		return;

	if (sd_file_exists(flagpath))
	{
		create(menu, "Icons/sw-on.bmp", servX, servY, (int (*)(void *))tool_servises_off, (void *)titleid);
	}
	else
	{
		create(menu, "Icons/sw-off.bmp", servX, servY, (int (*)(void *))tool_servises_on, (void *)titleid);
	}
	gui_menu_append_entry(menu, gui_create_menu_entry_no_bitmap(name, servX + 30, servY + 30, 150, 100, NULL, NULL));
	servstep++;
	if (servstep % 2 == 0)
	{
		servY = servYF;
		servX = servX + 205;
		//servstep = 0;
	}
	else
	{
		servY = servY + sepaserv;
	}
}

int Incognito(char *order){
	SDStrap();
	if (strstr(order, "B") != NULL)
	{

		if (strstr(order, "BS") != NULL)
		{
			buffer_blk = str_replace(buffer_blk, "blank_prodinfo_sysmmc=0", "blank_prodinfo_sysmmc=1");
		}

		if (strstr(order, "DBS") != NULL)
		{
			buffer_blk = str_replace(buffer_blk, "blank_prodinfo_sysmmc=1", "blank_prodinfo_sysmmc=0");
		}

		if (strstr(order, "BE") != NULL)
		{
			buffer_blk = str_replace(buffer_blk, "blank_prodinfo_emummc=0", "blank_prodinfo_emummc=1");
		}

		if (strstr(order, "DBE") != NULL)
		{
			buffer_blk = str_replace(buffer_blk, "blank_prodinfo_emummc=1", "blank_prodinfo_emummc=0");
		}

		u32 size = strlen(buffer_blk) - 1;
		sd_save_to_file(buffer_blk, size, "atmosphere/config/exosphere.ini");
		pre_load_menus(4, 0);
		gui_init_argon_menu();
		return 0;
	}

	sd_save_to_file(order, 1, "StarDust/autoboot.inc");
	launcher("StarDust/payloads/Incognito_RCM.bin");
	return 0;
}

int Autoboot(u32 fil){
	SDStrap();
	if (fil == 0)
		f_unlink("StarDust/autobootecho.txt");

	if (fil == 1)
		sd_save_to_file("", 0, "StarDust/autobootecho.txt");
	
	pre_load_menus(1, 0);
	gui_init_argon_menu();
	return 0;
};

int AThemes_list(gui_menu_t *menu, u32 gridX, u32 gridY){
	//generate themes Dinamy
	u32 tm_ajust = 90;
	char *folder = listfol("StarDust/skins", "*", true);
	u32 r = 0;
	u32 w = 0;
	while (folder[r * 256])
	{
		if (strlen(&folder[r * 256]) <= 100)
		{
			char *source_fol = (char *)malloc(256);
			strcpy(source_fol, "skins/");
			strcat(source_fol, &folder[r * 256]);
			strcat(source_fol, "/");
			char *source_icon = (char *)malloc(256);
			strcpy(source_icon, "/StarDust/");
			strcat(source_icon, source_fol);
			strcat(source_icon, "icon.bmp");
			if (sd_file_exists(source_icon))
			{
				gui_menu_append_entry(menu, gui_create_menu_entry("", sd_file_read(source_icon), gridX, gridY, 70, 70, (int (*)(void *))tool_theme, (void *)source_fol));
				gridX = gridX + tm_ajust;
			}
			w++;
		}
		if (w == 11)
			break;
		r++;
	}
	return 0;
}

void medislay(char *flags){
	SDStrap();
	change_brightness(0);
	if (sd_file_exists("StarDust/flags/b50.flag"))
		f_unlink("StarDust/flags/b50.flag");
	else
		sd_save_to_file("", 0, "StarDust/flags/b50.flag");
	pre_load_menus(1, 0);
	gui_init_argon_menu();
}
/**/

int uLaunch(u32 fil)
{
	SDStrap();
	if (fil == 0)
	{
		f_unlink("/atmosphere/contents/01008BB00013C000/exefs.nsp");
		f_unlink("/atmosphere/contents/010000000000100B/exefs.nsp");
		f_unlink("/atmosphere/contents/010000000000100B/fsmitm.flag");
		f_unlink("/atmosphere/contents/010000000000100B/romfs.bin");
		f_unlink("/atmosphere/contents/0100000000001000/exefs.nsp");
		f_unlink("/atmosphere/contents/0100000000001001/exefs.nsp");

		f_unlink("/sxos/titles/titles/01008BB00013C000/exefs.nsp");
		f_unlink("/sxos/titles/010000000000100B/exefs.nsp");
		f_unlink("/sxos/titles/010000000000100B/fsmitm.flag");
		f_unlink("/sxos/titles/010000000000100B/romfs.bin");
		f_unlink("/sxos/titles/0100000000001000/exefs.nsp");
		f_unlink("/sxos/titles/0100000000001001/exefs.nsp");
		f_unlink("StarDust/flags/ulaunch.flag");
	}

	if (fil == 1)
	{
		copyarall("/StarDust/ulaunch/SdOut/ulaunch", "/ulaunch", "*", "Installing ulaunch");
		copyarall("/StarDust/ulaunch/SdOut/atmosphere", "/atmosphere", "*", "Installing ulaunch");
		//		copyarall("/StarDust/ulaunch/titles", "/ReiNX/titles", "*","Installing ulaunch");
		//copyarall("/StarDust/ulaunch/titles", "/sxos/titles", "*", "Installing ulaunch");
		f_mkdir("StarDust/flags");
		sd_save_to_file("", 0, "StarDust/flags/ulaunch.flag");
	}
	printerCU("", "", 1); //flush print
	pre_load_menus(1, 0);
	gui_init_argon_menu();
	return 0;
}

void hekateOFW(u32 tipo) {
	SDStrap();
	if (tipo == 0)
	{

		if (sd_file_exists("bootloader/hekate_ipl.bkp"))
		{
			f_unlink("bootloader/hekate_ipl.ini");
			f_rename("bootloader/hekate_ipl.bkp", "bootloader/hekate_ipl.ini");
		}
		else
		{
			copyfile("bootloader/hekate_ipl.conf", "bootloader/hekate_ipl.ini");
		}
	}

	if (tipo == 1)
	{
		if (sd_file_size("bootloader/stock") != sd_file_size("bootloader/hekate_ipl.ini"))
		{
			f_rename("bootloader/hekate_ipl.ini", "bootloader/hekate_ipl.bkp");
		}
		copyfile("bootloader/stock", "bootloader/hekate_ipl.ini");
	}
	launcher("/StarDust/payloads/hekate.bin");
}

int launcher(char *path){
	SDStrap();
	gfx_swap_buffer(&g_gfx_ctxt);
	display_backlight_brightness(30, 1000);	

	u32 bootS = sd_file_size("StarDust/boot.dat");
	u32 bootF = sd_file_size("StarDust/boot_forwarder.dat");
	u32 bootR = sd_file_size("boot.dat");

	//Atmosphere
    if(strstr(path,"Atmosphere") != NULL)
    {
		if (sd_file_exists ("StarDust/autobootecho.txt")||(btn_read() & BTN_VOL_UP))
		sd_save_to_file("Atmosphere", 10, "StarDust/autobootecho.txt");
        if (bootF < bootR){
            copyfile("StarDust/boot_forwarder.dat","boot.dat");
        }
    }
	
	//SxOS
    if((strstr(path,"sxos") != NULL) || (strstr(path,"SXOS") != NULL))
    {
		g_gfx_con.scale = 3;


		if (bootS != bootR){
			gfx_con_setpos(&g_gfx_con, 370, 350);
			gfx_printf(&g_gfx_con, "Loading Boot.dat\n",bootS,bootR);
			gfx_swap_buffer(&g_gfx_ctxt);
			copyfile("StarDust/boot.dat","boot.dat");
		}
			
		if (sd_file_exists ("StarDust/autobootecho.txt")||(btn_read() & BTN_VOL_UP))
		sd_save_to_file("SXOS", 4, "StarDust/autobootecho.txt");
	}

    if(strstr(path,"android") != NULL)
    {
		if (sd_file_exists ("StarDust/autobootecho.txt")||(btn_read() & BTN_VOL_UP))
		sd_save_to_file("TWRP", 4, "StarDust/autobootecho.txt");		
    }
    if(strstr(path,"ubuntu") != NULL)
    {
		if (sd_file_exists ("StarDust/autobootecho.txt")||(btn_read() & BTN_VOL_UP))
		sd_save_to_file("Ubuntu", 6, "StarDust/autobootecho.txt");		
    }
	display_backlight_brightness(10, 1000);
	launch_payload(path);
	return 0;
}


/*
void HBhide(char *folder)
{
	SDStrap();
	char *source_fol = (char *)malloc(256);
	strcpy(source_fol, "/switch");
	strcat(source_fol, "/");
	strcat(source_fol, folder);
	strcat(source_fol, "/hide.flag");
	if (sd_file_exists(folder))
		f_unlink(folder);
	else
		sd_save_to_file("", 0, folder);
	pre_load_menus(5, 0);
}
*/
void change_brightness(u32 type)
{
	SDStrap();
	int bright = 100;
	if (sd_file_exists("StarDust/flags/b50.flag"))
		bright = 10;

	if (type == 0)
		display_backlight_brightness(bright, 1000);

	if (type == 1)
		display_backlight_brightness(bright / 2, 1000);
}
