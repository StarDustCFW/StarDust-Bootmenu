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


//Fix After Update STD
void clean_up()
{
	printerCU("Clean old files","CleanUP...",0);
	//Delete Reinx d'not use any more
	deleteall("/ReiNX", "*","");
	deleteall("/sept/reinx.bin", "*","");
	f_unlink("/ReiNX.bin");
	f_unlink("StarDust/payloads/ReiNX.bin");

	//Ams Blawar mod
	f_unlink("/atmosphere/fusee-secondary_ori.bin");
	f_unlink("/atmosphere/fusee-secondary.bin.sig");
	f_unlink("/atmosphere/hbl.json");
	f_unlink("/atmosphere/hbl.nsp.sig");
	f_unlink("/atmosphere/hbl_ori.nsp");
	deleteall("/switch/mercury", "*","");

	//Pequeña correccion
	f_rename("Payload_de_arranque.bin","boot_payload.bin");
	f_unlink("Payload_de_arranque.bin");
	f_rename("/Backup/prodinfo.bin", "/prodinfo_sysnand.bin");

	printerCU("Clean Trash files","CleanUP...",0);
	//borrar archivos inesesarios que se acumulan
	deleteall("/atmosphere/contents/010000000000000D", "*","");
	deleteall("/atmosphere/contents/010000000000002B", "*","");
	deleteall("/atmosphere/contents/010000000000003C", "*","");
	deleteall("/atmosphere/contents/0100000000000008", "*","");
	deleteall("/atmosphere/contents/0100000000000032", "*","");
	deleteall("/atmosphere/contents/0100000000000034", "*","");
	deleteall("/atmosphere/contents/0100000000000036", "*","");
	deleteall("/atmosphere/contents/0100000000000037", "*","");
	deleteall("/atmosphere/contents/0100000000000042", "*","");
	deleteall("/switch/sys-clk-Editor", "*","");
	deleteall("System Volume Information", "*","");
	deleteall("/atmosphere/flags", "*","");
	deleteall("/sxos/flags", "*","");
	deleteall("/sxos/titles/4200000000000010", "*","");
	deleteall("/sxos/titles/420000000000000E", "*","");
	f_unlink("/bootloader/payloads/boot_menu.bin");
	f_unlink("/bootloader/update.bin");
	deleteall("/nsp", "*","");
	deleteall("/BCAT", "*","");
	f_unlink("/StarDust/payloads/Stock.bin");
	f_unlink("/atmosphere/BCT.ini");
	f_unlink("/atmosphere/loader.ini");
	f_unlink("/atmosphere/system_settings.ini");
	deleteall("/switch/Z-stuff", "*","");
	deleteall("/sxos/titles/0100000000000034-OFF", "*","");
	deleteall("/sxos/titles/0100000000000034", "*","");
	f_unlink("/Haku33.nro");
	f_unlink("/Lock-Logs.nro");
	deleteall("/ftpd", "*","");
	deleteall("/StarDust/ftpd", "*","");
	deleteall("/themes/StarDust-2.0", "*","");
	f_unlink("for_tegraRCM_PC.bin");
	deleteall("/switch/KosmosToolbox", "*","");
	deleteall("/switch/KosmosUpdater", "*","");
	deleteall("/atmosphere/kips", "*","");
	deleteall("/Toolkit", "*","");
	deleteall("/uboot", "*","");
	deleteall("/modules", "*","");
	deleteall("/RR", "*","");
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
	//nada en contra de RR es solo que si usas este pack esos ficheros son innesesarios
	f_unlink("/atmosphere/prodinfo.ini");
	f_unlink("/atmosphere/template.txt");
	f_unlink("/sxos/template.txt");
	f_unlink("/switch/Amiigo.nro");
	f_unlink("/bootloader/hekate_ipl.bak");
//	deleteall("/switch/incognito", "*","");
	deleteall("/atmosphere/exefs_patches/NxThemesInstaller", "*","");
	deleteall("/sxos/exefs_patches/NxThemesInstaller", "*","");
	deleteall("/atmosphere/contents/010000000007E51A", "*","");//Tesla change
	//tinfoil clean
	//deleteall("/atmosphere/kip_patches/loader_patches", "*","");
	f_unlink("/bootloader/patches.ini");

	//RR ovl
	f_unlink("/atmosphere/contents/010000000000100C/exefs.nsp");
	f_unlink("/sxos/titles/010000000000100C/exefs.nsp");
	
	//shop FS
	f_unlink("/atmosphere/contents/010000000000100B/exefs.nsp");
	f_unlink("/atmosphere/contents/010000000000100B/romfs.bin");
	f_unlink("/reboot.nro");

	printerCU("Correct some Trash files for not crash","CleanUP...",0);
	deleteall("/switch/screen-nx", "*","");
	//changes for new menu
	f_unlink("/StarDust/payloads/zBackup.bin");
	deleteall("/StarDust/payback", "*","");
	deleteall("/StarDust/logos", "*","");
	deleteall("/StarDust/Atheme", "*","");

	deleteall("/atmosphere/contents/0100000000000BEF", "*","");
	deleteall("/atmosphere/contents/010000000000100B", "*","");
	deleteall("/atmosphere/contents/0100000000001010", "*","");
	deleteall("/atmosphere/contents/0100000000000803", "*","");
	deleteall("/atmosphere/contents/050000BADDAD0000", "*","");

	f_unlink("/StarDust/Icons/screenshot.bmp");
	f_unlink("/StarDust/payloads/zBackup.bin");
	deleteall("/atmosphere/contents", "romfs_metadata.bin","Clean romfs_metadata");
	f_unlink("/atmosphere/contents/010000000000100C/fsmitm.flag");

	sd_save_to_file("",0,"atmosphere/contents/0100000000001000/fsmitm.flag");
	
	//old reverse
	deleteall("/atmosphere/contents/0100000000534C56", "*","");
	deleteall("/sxos/titles/0100000000534C56", "*","");
	
	//remove themes
	f_unlink("/atmosphere/contents/0100000000001000/romfs/lyt/Entrance.szs");//9.1.0 Theme suport
	f_unlink("/atmosphere/contents/0100000000001000/sfmor/lyt/Entrance.szs");//9.1.0 Theme suport
	f_unlink("/atmosphere/contents/0100000000001000/romfs/lyt/Set.szs");
	f_unlink("/atmosphere/contents/0100000000001013/fsmitm.flag");
	f_unlink("/atmosphere/contents/0100000000001000/romfs/lyt/common.szs");
	deleteall("/atmosphere/contents/0100000000001013/romfs/", "*","");

	f_unlink("/sxos/titles/0100000000001000/romfs/lyt/Entrance.szs");//9.1.0 Theme suport
	f_unlink("/sxos/titles/0100000000001000/romfs/lyt/Set.szs");
	f_unlink("/sxos/titles/0100000000001000/romfs/lyt/common.szs");
	f_unlink("/sxos/titles/0100000000001013/fsmitm.flag");
	deleteall("/sxos/titles/0100000000001013/romfs/", "*","");
	f_unlink("StarDust/boot.temp");

/**/
//	Has Archive Bit
	if(HasArchBit("atmosphere")||HasArchBit("atmosphere/contents"))
	{
		printerCU("Fix Archive bit in: /Atmosphere","CleanUP...",0);
		Killflags("atmosphere");	
	}

	if(HasArchBit("switch")||HasArchBit("switch/XXX"))
	{
		printerCU("Fix Archive bit in: /switch","CleanUP...",0);
		Killflags("switch");	
	} 

	//not pegascape units
	if(fusesB() > 5)
	{
		printerCU("Remove PegaScape Files","CleanUP...",0);
		deleteall("/pegascape", "*","");
		f_unlink("/switch/fakenews-injector.nro");
	}
	keys();

	//fix old Emunand transfer
	if (sd_file_exists ("sxos/eMMC/00")&sd_file_exists ("sxos/eMMC/boot0")&sd_file_exists ("sxos/eMMC/boot1"))
	{
		printerCU("Fix Emu Transfer","CleanUP...",0);
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
				f_puts("path=emuMMC/EF00\n", &fp);
				f_puts("nintendo_path=emuMMC/EF00/Nintendo\n", &fp);
				f_close(&fp);
				sd_save_to_file("", 0, "emuMMC/EF00/file_based");
			}
	}

	//deleteall("/////", "*","");	
	//close
	printerCU("","",1);//flush print
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
		//some test verify payload 
		if(sd_file_exists("StarDust/flags/ONE.flag"))
		{
			f_unlink("StarDust/flags/ONE.flag");
			launch_payload("payload.bin");
		}
/*		bool cancel_auto_chainloading = btn_read() & BTN_VOL_DOWN;
        bool load_menu = cancel_auto_chainloading || launch_payload("StarDust/payload.bin");
*/
		//update stardust
		bool cancel_auto_chainloading = btn_read() & BTN_VOL_UP;
		if(sd_file_exists("StarDust_update/fixer.del")& !cancel_auto_chainloading)
		{
			if (sd_file_exists ("license.dat")&!sd_file_exists ("license.dat.bak")){
				printerCU("Backup SXOS license.dat","BackingUP...",0);
				copyfile("license.dat", "license.dat.bak");
			}
			moverall("/StarDust_update", "", "*","Updating");
				gfx_con_setpos(&g_gfx_con, 1, 100);		
				gfx_printf(&g_gfx_con, "Clean Update\n");
				gfx_swap_buffer(&g_gfx_ctxt);
			deleteall("/StarDust_update", "*","Clean Update");
			f_rename("/StarDust_update", "/StarDust_corrupt_update");//just in case
			launch_payload("payload.bin");
		}
		
		if(sd_file_exists("fixer.del"))
		clean_up();

//        gfx_printf(&g_gfx_con, "Autochainload canceled. Loading menu...\n");
//      gfx_swap_buffer(&g_gfx_ctxt);
//		msleep(2000);
//		BootStrapNX();
		
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

