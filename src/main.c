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
		g_gfx_con.scale = 3;
		gfx_con_setpos(&g_gfx_con, 1070, 10);
		gfx_con_setcol(&g_gfx_con, 0xFF008F39, 0xFF726F68, 0xFF191414);
		gfx_printf(&g_gfx_con, "CleanUP\n");
		gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0, 0xFF191414);
gfx_swap_buffer(&g_gfx_ctxt);
//delete bootloader for tinfoil
/*
deleteall("/bootloader/ini", "*");
deleteall("/bootloader/payloads", "*");
deleteall("/bootloader/libtools", "*");
deleteall("/bootloader/sys", "*");
deleteall("/bootloader", "*");
*/
deleteall("/ftpd", "*");
deleteall("/StarDust/ftpd", "*");

deleteall("/ReiNX/patches/es_patches", "*");
deleteall("/ReiNX/sysmodules.dis", "*");
f_unlink("/ReiNX/sysmodules/fs_mitm.kip");
f_unlink("/ReiNX/sysmodules/ldn_mitm.kip");
deleteall("/ReiNX/exefs_patches/Youtube", "*");
//deleteall("/ReiNX/titles/010000000000000D/exefs", "*");
//deleteall("/ReiNX/titles/010000000000000D", "*");
deleteall("/ReiNX/titles/0100000000000032/exefs", "*");
deleteall("/ReiNX/titles/0100000000000032", "*");
deleteall("/ReiNX/titles/0100000000000032/flags", "*");

//Delete patches that reinx d'not use any more
deleteall("/ReiNX/exefs_patches/am_dev_function", "*");
deleteall("/ReiNX/exefs_patches/disable_ca_verification", "*");
deleteall("/ReiNX/exefs_patches/es_patches", "*");
deleteall("/ReiNX/exefs_patches/fatal_force_extra_info", "*");
deleteall("/ReiNX/exefs_patches/Signature Patches by br4z0rf", "*");
deleteall("/ReiNX/exefs_patches/Signature Patches by Rajkosto", "*");
deleteall("/ReiNX/exefs_patches", "*");
deleteall("/ReiNX/kip_patches/default_nogc", "*");
deleteall("/ReiNX/kip_patches/fs_patches", "*");
deleteall("/ReiNX/kip_patches", "*");
deleteall("/ReiNX/nro_patches/disable_browser_ca_verification", "*");
deleteall("/ReiNX/nro_patches", "*");


//ams Blawar mod
f_unlink("/atmosphere/fusee-secondary.bin.sig");
f_unlink("/atmosphere/hbl.json");
f_unlink("/atmosphere/hbl.nsp.sig");
f_unlink("/atmosphere/hbl_ori.nsp");
deleteall("/switch/mercury", "*");
f_unlink("atmosphere/flags/bis_write.flag");

//music
f_rename("/StarDust/music/fondo.mp3", "/StarDust/music/0.mp3");
f_rename("/StarDust/music/fondo1.mp3","/StarDust/music/1.mp3");
f_rename("/StarDust/music/fondo2.mp3","/StarDust/music/2.mp3");
f_rename("/StarDust/music/fondo3.mp3","/StarDust/music/3.mp3");
f_rename("/StarDust/music/fondo4.mp3","/StarDust/music/4.mp3");
f_rename("/StarDust/music/fondo5.mp3","/StarDust/music/5.mp3");
f_rename("/StarDust/music/fondo6.mp3","/StarDust/music/6.mp3");


deleteall("/themes/StarDust-2.0", "*");


//pequeña correccion
f_rename("/switch/incognito/prodinfo.bin", "/backup/prodinfo.bin");
f_rename("for_tegraRCM_PC.bin", "boot_payload.bin");
f_unlink("for_tegraRCM_PC.bin");
f_rename("Payload_de_arranque.bin","boot_payload.bin");
f_unlink("Payload_de_arranque.bin");


//borrar archivos inesesarios que se acumulan 
deleteall("/switch/KosmosToolbox", "*");
deleteall("/switch/KosmosUpdater", "*");
deleteall("/atmosphere/kips", "*");
deleteall("/Toolkit/splashes", "*");
deleteall("/Toolkit", "*");
deleteall("/uboot", "*");
deleteall("/modules/hid_mitm", "*");
deleteall("/modules", "*");
deleteall("/RR/logos", "*");
deleteall("/RR/payloads", "*");
deleteall("/RR/sys", "*");
deleteall("/RR", "*");
f_unlink("/switch/toolbox.nro");
f_unlink("/switch/ChoiDujourNX.nro");
f_unlink("/switch/ftpd.nro");
f_unlink("/switch/changelog");
f_unlink("/switch/appstore.nro");
f_unlink("/switch/Goldleaf.nro");
f_unlink("/switch/incognito.nro");
f_unlink("/switch/info.json");
f_unlink("/switch/In-Home-Switching.nro");
f_unlink("/switch/ldnmitm_config.nro");
f_unlink("/switch/lithium.nro");
f_unlink("/switch/nxmtp.nro");
f_unlink("/switch/reboot_to_payload.nro");
f_unlink("/switch/RetroReloadedUpdater.nro");
f_unlink("/switch/Lithium_README.md");
f_unlink("/switch/README.md");
f_unlink("/RR_version.txt");
f_unlink("/ums_boot0.ini");
f_unlink("/ums_boot1.ini");
f_unlink("/ums_emmc.ini");
f_unlink("/ums_sd.ini");
f_unlink("/atmosphere/prodinfo.ini");
f_unlink("/atmosphere/template.txt");
f_unlink("/ReiNX/template.txt");
f_unlink("/sxos/template.txt");
//nada en contra de RR es solo que si usas este pack esos ficheros son innesesarios

//changes for new menu
f_unlink("/StarDust/payback/Stock.bin");
f_unlink("/StarDust/payloads/zBackup.bin");
f_unlink("/StarDust/logos/zBackup.bmp");
f_unlink("/StarDust/logos/negative.bmp");
f_unlink("/StarDust/logos/biskeydump.bmp");
f_unlink("/StarDust/logos/Backup.bmp");
f_unlink("/StarDust/logos/Hekate.bmp");
f_unlink("/StarDust/Atheme/1/logos/zBackup.bmp");
f_unlink("/StarDust/Atheme/2/logos/zBackup.bmp");
f_unlink("/StarDust/Atheme/3/logos/zBackup.bmp");
f_unlink("/StarDust/Atheme/4/logos/zBackup.bmp");
f_unlink("/StarDust/Atheme/5/logos/zBackup.bmp");
f_unlink("/StarDust/Atheme/6/logos/zBackup.bmp");
f_unlink("/StarDust/Atheme/7/logos/zBackup.bmp");
f_unlink("/StarDust/Atheme/1/Icons/arrowr.bmp");
f_unlink("/StarDust/Atheme/2/Icons/arrowr.bmp");
f_unlink("/StarDust/Atheme/3/Icons/arrowr.bmp");
f_unlink("/StarDust/Atheme/4/Icons/arrowr.bmp");
f_unlink("/StarDust/Atheme/5/Icons/arrowr.bmp");
f_unlink("/StarDust/Atheme/6/Icons/arrowr.bmp");
f_unlink("/StarDust/Atheme/7/Icons/arrowr.bmp");
f_unlink("/StarDust/Icons/screenshot.bmp");
f_unlink("/StarDust/Atheme/1/Icons/screenshot.bmp");
f_unlink("/StarDust/Atheme/2/Icons/screenshot.bmp");
f_unlink("/StarDust/Atheme/3/Icons/screenshot.bmp");
f_unlink("/StarDust/Atheme/4/Icons/screenshot.bmp");
f_unlink("/StarDust/Atheme/5/Icons/screenshot.bmp");
f_unlink("/StarDust/Atheme/7/Icons/screenshot.bmp");
/*
f_unlink("/");
f_unlink("/");
f_unlink("/");
f_unlink("/");
f_unlink("/");
f_unlink("/");
f_unlink("/");
f_unlink("/");
f_unlink("/");
*/
f_rename("/Backup/prodinfo.bin", "/prodinfo_sysnand.bin");
f_unlink("/nsp/Haku33SwitchCleaner_[05229B5E9D160000].nsp");

	//remove themes
//	f_unlink("/atmosphere/titles/0100000000001000/romfs/lyt/Entrance.szs");//9.0.0 Theme suport
	f_unlink("/atmosphere/titles/0100000000001000/romfs/lyt/Set.szs");
	f_unlink("/ReiNX/titles/0100000000001000/romfs/lyt/Set.szs");
	f_unlink("/sxos/titles/0100000000001000/romfs/lyt/Set.szs");
	f_unlink("/atmosphere/titles/0100000000001000/romfs/lyt/common.szs");
	f_unlink("/ReiNX/titles/0100000000001000/romfs/lyt/common.szs");
	f_unlink("/sxos/titles/0100000000001000/romfs/lyt/common.szs");
	f_unlink("/atmosphere/titles/0100000000001013/fsmitm.flag");
	f_unlink("/ReiNX/titles/0100000000001013/fsmitm.flag");
	f_unlink("/sxos/titles/0100000000001013/fsmitm.flag");
	
/*	u32 burntFuses = 0;
	for (u32 i = 0; i < 32; i++)
	{
		if ((fuse_read_odm(7) >> i) & 1)
			burntFuses++;
	}
	if(burntFuses == 11)
	{
		f_unlink("/atmosphere/titles/0100000000001000/fsmitm.flag");
		f_unlink("/atmosphere/titles/420000000000000E/flags/boot2.flag");
	}
*/
//fix old Emunand transfer
	if (sd_file_exists ("sxos/eMMC/00")&sd_file_exists ("sxos/eMMC/boot0")&sd_file_exists ("sxos/eMMC/boot1"))
	{
		f_mkdir("emuMMC");
		f_mkdir("emuMMC/EF00");
		f_rename("/sxos/eMMC","/emuMMC/EF00/eMMC");
		f_rename("/Emutendo","/emuMMC/EF00/Nintendo");
			if (sd_file_exists ("emuMMC/EF00/eMMC/00")&sd_file_exists ("emuMMC/EF00/eMMC/boot0")&sd_file_exists ("emuMMC/EF00/eMMC/boot1"))
			{
				f_unlink("emummc/emummc.bak");
				f_rename("emummc/emummc.ini","emummc/emummc.bak");
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
			}
	}


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
            // write self to payload.bin
/*			FIL fp;
            f_open(&fp, "payload.bik", FA_CREATE_NEW | FA_WRITE);
            u32 payload_size = *(u32 *)(0x40003000 + 0x84) - 0x40003000;
            f_write(&fp, (u8 *)0x40003000, payload_size, NULL);
            f_close(&fp);
*/
/*		bool cancel_auto_chainloading = btn_read() & BTN_VOL_DOWN;
        bool load_menu = cancel_auto_chainloading || launch_payload("StarDust/payload.bin");
*/		//remove autoboot

		if(sd_file_exists("fixer.del"))
		clean_up();		
//        gfx_printf(&g_gfx_con, "Autochainload canceled. Loading menu...\n");
//        gfx_swap_buffer(&g_gfx_ctxt);
//		msleep(25000);
//        if (load_menu)
        gui_init_argon_boot();
//		gui_init_argon_menu();
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

