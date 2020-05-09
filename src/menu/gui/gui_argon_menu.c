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
#include "core/custom-gui.h"
#include "gfx/di.h"
#include "gfx/gfx.h"
#include <stdio.h>

#include "utils/types.h"
#include "utils/fs_utils.h"
#include "utils/dirlist.h"
#include "utils/util.h"
#include "utils/touch.h"
#include "utils/btn.h"
#include "core/launcher.h"
#include "core/payloads.h"
#include "core/custom-gui.h"

#include "mem/heap.h"

#define COLUMNS 4  
#define ROWS 2
#define ELEM_SIZE 230
#define MARGIN_TOP 100
#define MARGIN_LEFT 46
#define MAX_CHAR 100 typedef char string[MAX_CHAR+1];
//static int tool_menu_rSD(void* param);
//static int findLastIndex(char* str, char* x);
char *gobacknow(char* sd_path) ;

static int tool_extr_rSD(void* param);
static int tool_reboot_rcm(void* param);
static int tool_power_off(void* param);
static int tool_menu_rem(void* param);
int tool_theme(char* param);
static int tool_emu(u32 param);
//static int zbackup(char* param);
static int tool_Menus(u32 param);
int tool_dir(char *param);
int tool_servises(u32 param);
int tool_servises_on(char* title);
int tool_servises_off(char* title);
int tool_Themes_on(char* cfw);
int tool_Themes_off(char* cfw);
int tool_filete(char* fil);
int tool_file_act(u32 fil);
int memloader(u32 fil);
int Incognito(char* order);
int Autoboot(u32 fil);
int uLaunch(u32 fil);
int AThemes_list(gui_menu_t* menu, u32 gridX, u32 gridY);
void HBhide(char* folder);


int bat_show(u32 percent);
void serv_display(gui_menu_t* menu,char* titleid,char* name);
void help_display(void);
void Ajbrillo(u32 tipo);
void hekateOFW(u32 tipo);
void medislay(char *flags);
u32 brillo = 100;
u32 brilloM = 50;
		const char* sysserial;
		const char* emuserial;
		char* buffer_blk;


//menus
u64 main_menu = 0;

//sub menus
u32 permsubY = 0;
u32 permsubX = 0;
u32 sub_buttonW = 0;
u32 sub_buttonH = 0;

//some indicators
u32 onemi = 0;
u32 retir = 0;
u32 Incac = 0;

//services
u32 servYF = 250;
u32 servXF = 160;
//serv2
u32 servX = 30;
u32 servY = 80;
u32 sepaserv = 70;
u32 servstep = 0;


//dinamic directory
char *directory = "";
char *filete = "";
char *filpay = "3333333333";
char Aversion[1];

//help Switch
u32 help_switch = 1;
void Ajbrillo(u32 tipo)
{
if (!sd_mount()){BootStrapNX();}//check sd
		if (sd_file_exists("StarDust/flags/b50.flag"))
		{
			brillo = 10;
			brilloM = brillo/2;
		}else{
			brillo = 100;
			brilloM = brillo/2;
		}
		
		if (tipo == 0)
		display_backlight_brightness(brillo, 1000);

		if (tipo == 1)
		display_backlight_brightness(brilloM, 1000);
}


void gui_init_argon_boot(void)
{
if (!sd_mount()){BootStrapNX();}//check sd
    /* Init pool for menu */
		if (sd_file_exists("StarDust/autoboot.inc"))
		{
			main_menu = 4;
			f_unlink("StarDust/autoboot.inc");
			Incac = 1;

		}
    gui_menu_pool_init();

    gui_menu_t* menu = gui_menu_create("ArgonNX",main_menu);
	
	Ajbrillo(0);
	
	//show display without icons
    gui_menu_open2(menu);


	
	//corrections on start
	f_mkdir("StarDust/flags");
	f_unlink("auto.bak");

	//set archive bit
	 f_chmod("switch/LinkUser", AM_ARC, AM_ARC);
	 
	//prepare use of sxos dongle 
	if ((sd_file_size("boot.dat") != sd_file_size("StarDust/boot_forwarder.dat"))&(sd_file_exists ("StarDust/boot_forwarder.dat")))
	copyfile("StarDust/boot_forwarder.dat","boot.dat");
		

	//prevent accidental boot to ofw
	if (sd_file_size("bootloader/hekate_ipl.ini") != sd_file_size("bootloader/hekate_ipl.conf"))
	copyfile("bootloader/hekate_ipl.conf","bootloader/hekate_ipl.ini");//
	
	//waith user input
	if (sd_file_exists("StarDust/autobootecho.txt")& (!sd_file_exists ("StarDust/autoboot.inc")))
	btn_wait_timeout(3000, BTN_VOL_UP);

    bool cancel_auto_chainloading = btn_read() & BTN_VOL_UP;
    if ((!cancel_auto_chainloading) & (!sd_file_exists ("StarDust/autoboot.inc")))
	{
		//autoboot
		char *str;
		if (g_sd_mounted){
			void *buf;
			buf = sd_file_read("StarDust/autobootecho.txt");
			str = buf;
			Aversion[0] = str[0];
		}
		if(strstr(Aversion,"A") != NULL)
		launch_payload("StarDust/payloads/Atmosphere.bin");

//		if(strstr(Aversion,"R") != NULL)
//		launch_payload("StarDust/payloads/ReiNX.bin");
		
		if(strstr(Aversion,"S") != NULL)
		launch_payload("StarDust/payloads/SXOS.bin");
	}
	f_unlink("StarDust/payload.bin");
	if (btn_read() & BTN_VOL_DOWN)
	f_unlink("StarDust/autobootecho.txt");
	
	//dinamic correction for ams folder
	sd_save_to_file("", 0, "atmosphere/titles/killme.flag");
	if (sd_file_exists("atmosphere/titles/killme.flag"))
	{
		f_rename("/atmosphere/titles","/atmosphere/contents");
		copyarall("/atmosphere/titles/0100000000001000", "/atmosphere/contents/0100000000001000", "*","");
		deleteall("/atmosphere/titles", "*","");
	}
gui_menu_pool_cleanup();
gui_init_argon_menu();
}


/* Init needed menus for ArgonNX */
void gui_init_argon_menu(void)
{
if (!sd_mount()){BootStrapNX();}//check sd
		//catch incognito return
    /* Init pool for menu */
    gui_menu_pool_init();
gui_menu_t* menu = gui_menu_create("ArgonNX",main_menu);



//low icons Y
u64 low_icons = 645;

//main menu 0-------------------------------------
	if(main_menu == 0)
	{
		//generate main menu
		u32 main_iconY = 190;
		u32 main_iconX = 190;
		u32 main_iconXS = 300;

		//Create emummc icon
			u32 buttonY = main_iconY-67;
			if (sd_file_exists("emummc/emummc.ini"))
			{

				char *str = sd_file_read("emummc/emummc.ini");
				if(retir == 0)
				{
					if(strstr(str,"emummc_") != NULL)
					{
					str = str_replace(str, "emummc_", "");
					u32 size = strlen(str)-1;
					sd_save_to_file(str,size,"emummc/emummc.ini");
					}
					
					if(strstr(str," ") != NULL)
					{
					str = str_replace(str, " ", "");
					u32 size = strlen(str)-1;
					sd_save_to_file(str,size,"emummc/emummc.ini");
					}
				retir = 1;
					if(strstr(str,"enabled=1") != NULL)
					{retir = 2;}
				}
/*
	g_gfx_con.scale = 3;
    gfx_con_setpos(&g_gfx_con, 470, 250);
    gfx_printf(&g_gfx_con, "\n%s\n",str);
    gfx_swap_buffer(&g_gfx_ctxt);
			msleep(5000);
*/
				if(retir == 2)
				{
	//				main_iconX = main_iconX + main_iconXS/2;
					gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/bon.bmp"),main_iconX+50, buttonY, 200 , 75,(int (*)(void *))tool_emu, (void*)0)); //- 80, - 500
				}else{
					gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/boff.bmp"),main_iconX+50, buttonY, 200 , 75,(int (*)(void *))tool_emu, (void*)1)); //- 80, - 500
				}
			}


			
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/logos/Atmosphere.bmp"), main_iconX, main_iconY, 300 , 300,(int (*)(void *))launch_payload, (void*)"/StarDust/payloads/Atmosphere.bin"));
			main_iconX = main_iconX + main_iconXS;

//			if(retir <= 1)
//			{
//			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/logos/ReiNX.bmp"), main_iconX, main_iconY, 300 , 300,(int (*)(void *))launch_payload, (void*)"/StarDust/payloads/ReiNX.bin"));
			main_iconX = main_iconX + main_iconXS;
//			}

			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/logos/SXOS.bmp"), main_iconX, main_iconY, 300 , 300,(int (*)(void *))launch_payload, (void*)"/StarDust/payloads/SXOS.bin"));
//				
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/logos/Stock.bmp"),540, main_iconY +100, 200 , 100,(int (*)(void *))hekateOFW, (void*)1));

			if (sd_file_exists ("/switchroot_android/coreboot.rom")|| sd_file_exists ("/switchroot_android/coreboot.bin"))
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/Android.bmp"),590, main_iconY-10, 100, 100, (int (*)(void *))launch_payload, (void*)"/switchroot_android/coreboot.bin"));
//
			u64 iconrowY = 550;
			u64 iconrowX = 170;
			u64 iconrowXS = 250;
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/logos/Lockpick_RCM.bmp"),iconrowX, iconrowY, 200 , 75,(int (*)(void *))launch_payload, (void*)"/StarDust/payback/Lockpick_RCM.bin"));
			iconrowX = iconrowX + iconrowXS;
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/logos/memloader.bmp"),iconrowX, iconrowY, 200 , 75,(int (*)(void *))tool_Menus, (void*)3));
			iconrowX = iconrowX + iconrowXS;
			
			if (Incac == 0)
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/logos/Incognito.bmp"),iconrowX, iconrowY, 200 , 75,(int (*)(void *))Incognito, (void*)"1"));
			else
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/logos/Incognito.bmp"),iconrowX, iconrowY, 200 , 75,(int (*)(void *))tool_Menus, (void*)4));
		
			iconrowX = iconrowX + iconrowXS;
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/logos/zHekate.bmp"), iconrowX, iconrowY, 200 , 75,(int (*)(void *))hekateOFW, (void*)0));
		//	gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/logos/Incognito.bmp"),iconrowX+700, iconrowY, 200 , 200,(int (*)(void *))tool_Menus, (void*)6));
	

		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/SD.bmp"),10, low_icons, 70, 70, tool_extr_rSD, NULL));
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/rcm.bmp"),750, low_icons, 70, 70, tool_reboot_rcm, NULL));
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/themes.bmp"),450, low_icons, 70, 70, tool_menu_rem, NULL));
//			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/logos/Reinx-off.bmp"),366, main_iconY, 300 , 300, NULL, NULL));
		//call menu 1
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/gear.bmp"),1200, low_icons, 70, 70, (int (*)(void *))tool_Menus, (void*)1));
	}

	//Menu 1 Ajustes
	if(main_menu == 1)
	{

		//list custom skins
		AThemes_list(menu,80,90);
		
		//draw autoboot
		if (sd_file_exists ("StarDust/autobootecho.txt"))
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/Auton.bmp"),1050, 600, 200 , 75,(int (*)(void *))Autoboot, (void*)0));
		else
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/Autoff.bmp"),1050, 600, 200 , 75,(int (*)(void *))Autoboot, (void*)1));
		
		//draw Ulaunch
		if (sd_file_exists ("StarDust/flags/ulaunch.flag"))
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/ulon.bmp"),850, 600, 200 , 75,(int (*)(void *))uLaunch, (void*)0));
		else
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/uloff.bmp"),850, 600, 200 , 75,(int (*)(void *))uLaunch, (void*)1));

		//services
		servY = servYF;
		servX = servXF;
		servstep = 0;
//		gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Services",servX+30, servY-10, 150, 100, NULL, NULL));
		serv_display(menu,"420000000000000E","FTP");
		serv_display(menu,"0100000000000352","Emuiio"); 
		serv_display(menu,"4200000000000010","LanPlay");
		serv_display(menu,"0100000000000BEF","Disk-USB");
		serv_display(menu,"4200000000000000","SysTune"); 
		serv_display(menu,"690000000000000D","Sys-Con");
		serv_display(menu,"420000000007E51A","TeslaOVL");
		serv_display(menu,"00FF0000A53BB665","SysDVR");
		serv_display(menu,"00FF0000636C6BFF","Sys-Clk");
		serv_display(menu,"420000000000000B","SysPlay"); 
		serv_display(menu,"0000000000534C56","ReverseNX");
		serv_display(menu,"0100000000000069","ReiSpoof");
		serv_display(menu,"0100000000000FAF","HDI");

		//themes for cfw
			u32 temX = 310;
			u32 temY = 440;
			u32 temS = 300;
				if (!sd_file_exists ("atmosphere/contents/0100000000001000/fsmitm.flag"))
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sw-off.bmp"),temX, temY, 200, 75,(int (*)(void *))tool_Themes_on, (void*)"atmosphere"));
				else
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sw-on.bmp"),temX, temY, 200, 75,(int (*)(void *))tool_Themes_off, (void*)"atmosphere"));
				gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("AMS",temX+30, temY+30, 150, 100, NULL, NULL));				
				temX = temX + temS;
				
				if (!sd_file_exists ("sxos/titles/0100000000001000/fsmitm.flag"))
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sw-off.bmp"),temX, temY, 200, 75,(int (*)(void *))tool_Themes_on, (void*)"sxos"));
				else
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sw-on.bmp"),temX, temY, 200, 75,(int (*)(void *))tool_Themes_off, (void*)"sxos"));
				gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("SXOS",temX+30, temY+30, 150, 100, NULL, NULL));
				temX = temX + temS;

				if (!sd_file_exists ("atmosphere/contents/0100000000001013/exefs.nsp"))
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sw-off.bmp"),temX, temY, 200, 75,(int (*)(void *))tool_Themes_on, (void*)"Profile"));
				else
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sw-on.bmp"),temX, temY, 200, 75,(int (*)(void *))tool_Themes_off, (void*)"Profile"));
				gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Profile",temX+30, temY+30, 150, 100, NULL, NULL));
				

		//Create SXOS transfer buttons
		if (sd_file_exists ("emuMMC/EF00/eMMC/00")&sd_file_exists ("emuMMC/EF00/eMMC/boot0")&sd_file_exists ("emuMMC/EF00/eMMC/boot1"))
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/swap.bmp"),380, 600, 200 , 75,(int (*)(void *))tool_emu, (void*)66));

		if (sd_file_exists ("sxos/emunand/full.00.bin")&sd_file_exists ("sxos/emunand/boot0.bin")&sd_file_exists ("sxos/emunand/boot1.bin"))
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/Transfer.bmp"),380, 600, 200 , 75,(int (*)(void *))tool_emu, (void*)33)); //- 80, - 500

		if (!sd_file_exists("emummc/emummc.ini"))
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/link_hide.bmp"),580, 600, 200 , 75,(int (*)(void *))tool_emu, (void*)99)); 

		//call menu 5
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/HB.bmp"),790, 600, 70, 70,(int (*)(void *))tool_Menus, (void*)5));

	

		
		//Ajustar brillo 
		if (!sd_file_exists("StarDust/flags/b50.flag"))
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/day.bmp"),1200, 100, 70, 70,(int (*)(void *))medislay, (void*)0));
			else
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/nay.bmp"),1200, 100, 70, 70,(int (*)(void *))medislay, (void*)0));

		//draw power
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/power.bmp"),80, 500, 70, 70, tool_power_off, NULL));
		
		//call menu 2
//		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/file-browser.bmp"),250, low_icons, 70, 70,(int (*)(void *))tool_Menus, (void*)2));
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/file-browser.bmp"),250, low_icons, 70, 70,(int (*)(void *))launch_payload, (void*)"/StarDust/payback/TegraExplorer.bin"));

		
		
		}

	//Menu 2 Explorer
	if(main_menu == 2)
	{
			//List Files And folders
			char* files = listfil(directory, "*", true);
			char* folder = listfol(directory, "*", true);				
				u32 heith = 70; u32 row1 = 100; u32 row2 = 500; u32 row3 = 910;
				u32 r = 0; u32 w = 0; u32 i = 0; u32 f = 0; u32 y = heith; u32 x = row1; u32 space = 50;
			//nav Buttons
			if(strlen(directory) >= 1){
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/root.bmp"),154, 24, 50, 50,(int (*)(void *))tool_Menus, (void*)33));
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sd-back.bmp"),54, 24, 50, 50,(int (*)(void *))tool_file_act, (void*)99));				
			}else
			gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("sdmc:/", 80, 85, 150, 100, NULL, NULL));

			if(strlen(filete) >= 1)
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/del.bmp"),254, 24, 50, 50, (int (*)(void *))tool_file_act, (void*)0));
		
			//display folder
			gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(directory, 60+strlen(directory)*8-30, 85, 150, 5, NULL, NULL));
			
			//build folders 
			while(folder[r * 256])
			{
			if(f >= 33){
			gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("More...", 1100,690, 150, 100, NULL, NULL));
			break;}
				if(strlen(&folder[r * 256]) <= 100){			
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/folder.bmp"),x, y+30, 310, 25,(int (*)(void *))tool_dir, (void*)&folder[r * 256]));
			char folname[17];
			char *str = &folder[r * 256];
			for (int h = 0;h< 17;h++)
			folname[h] = str[h];
			folname[16] = '\0';
			gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(folname, x+strlen(folname)*8+40, y+35, 1, 1, NULL, NULL));

				y = y + space;
				
				f++;
				i++;
				if((i == 11)||(i == 22)){
					y = heith;
					if(i == 22){
					i = 0;
					x = row3;}
					else
					x = row2;
					}
				}
			r++;
			}

			//build files
			while(files[w * 256])
			{
			if(f >= 33){
			gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("More...", 1100,690, 150, 100, NULL, NULL));
			break;}
			if(strlen(&files[w * 256]) <= 100){
					//files Icons
					if(strstr(&files[w * 256],".bin") != NULL)
					{
						if(strstr(&files[w * 256],"prodinfo_emunand.bin") != NULL||strstr(&files[w * 256],"prodinfo_sysnand.bin") != NULL)
						gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/files.bmp"),x, y+30, 310, 25,(int (*)(void *))tool_filete, (void*)&files[w * 256]));
							else
						gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/files-bin.bmp"),x, y+30, 310, 25,(int (*)(void *))tool_filete, (void*)&files[w * 256]));
					}else{
						if(strstr(&files[w * 256],".txt") != NULL||strstr(&files[w * 256],".ini") != NULL||strstr(&files[w * 256],".conf") != NULL)
						{
							gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/files-txt.bmp"),x, y+30, 310, 25,(int (*)(void *))tool_filete, (void*)&files[w * 256]));
						}else{
							if(strstr(&files[w * 256],".png") != NULL||strstr(&files[w * 256],".bmp") != NULL||strstr(&files[w * 256],".jpg") != NULL)
								gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/files-img.bmp"),x, y+30, 310, 25,(int (*)(void *))tool_filete, (void*)&files[w * 256]));
							else
								gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/files.bmp"),x, y+30, 310, 25,(int (*)(void *))tool_filete, (void*)&files[w * 256]));
						}
					}
					
			char filname[17];
			char *str = &files[w * 256];
			for (int h = 0;h< 17;h++)
			filname[h] = str[h];
			filname[16] = '\0';

				gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(filname, x+strlen(filname)*8+40, y+35, 1, 1, NULL, NULL));
				y = y + space;

			f++;			
			i++;			
					
				if((i == 11)||(i == 22)){
					y = heith;
					if(i == 22){
					i = 0;
					x = row3;}
					else
					x = row2;
					}
				}
			w++;
			}
			
			//file select
			if(strlen(filete) >= 1)
			{
//				u32 actX = 900;
//				u32 actY = 1;
				char* filto;
				if(strlen(directory) >= 1)
					filto = str_replace(filete, directory, "");
				else
					filto = filete;
//				gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(filto, 60+strlen(filto)*8-40, actY+70, 150, 100, NULL, NULL));
				gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(filto, strlen(filto)*8-40+350, 5, 150, 50, (int (*)(void *))tool_filete, (void*)""));
//				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/del.bmp"),actX, actY, 50, 50, (int (*)(void *))tool_file_act, (void*)0));
			}

		
		
		
		//call menu 1
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/gear.bmp"),1200, low_icons, 70, 70, (int (*)(void *))tool_Menus, (void*)1));
	}
		 
	//Menu 3 Memloader
	if(main_menu == 3)
	{
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/logos/memloader_boot0.bmp"),80, 300, 200 , 200,(int (*)(void *))memloader, (void*)0));
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/logos/memloader_boot1.bmp"),380, 300, 200 , 200,(int (*)(void *))memloader, (void*)1));
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/logos/memloader_emmc.bmp"),680, 300, 200 , 200,(int (*)(void *))memloader, (void*)2));
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/logos/memloader_sd.bmp"),980, 300, 200 , 200,(int (*)(void *))memloader, (void*)3));
		//call
		
	}
	
	//Menu 4 Incognito
	if(main_menu == 4)
	{
//		incognito togle
		//Getinfo from text
		u32 sys_inc = 0;
		u32 emu_inc = 0;
		
		//get blank or not 
		u32 sys_blk = 0;
		u32 emu_blk = 0;
		
		u32 xcol = 90;
		u32 rowinc = 900+xcol;
		u32 colinc = 150;
		u32 rowsepar = 50;
		u32 colsepar = 100;

		if (sd_mount())
		{
		if (sd_file_exists ("exosphere.ini")){
			void *buf = sd_file_read("exosphere.ini");
			buffer_blk = buf;
		}
		if(strstr(buffer_blk,"blank_prodinfo_emummc=1") != NULL)
		{emu_blk = 1;}

		if(strstr(buffer_blk,"blank_prodinfo_sysmmc=1") != NULL)
		{sys_blk = 1;}
	
	
/*
	g_gfx_con.scale = 1;
    gfx_swap_buffer(&g_gfx_ctxt);
   gfx_con_setpos(&g_gfx_con, 470, 20);
    gfx_printf(&g_gfx_con, "\n%s\n",buffer_blk);
    gfx_swap_buffer(&g_gfx_ctxt);
			msleep(5000);
						if (sys_blk == 1){
							gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sw-on.bmp"),970, y+30, 200, 75,(int (*)(void *))HBhide, (void*)source_fol));
						}else{
							gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sw-off.bmp"),170, y+30, 200, 75,(int (*)(void *))HBhide, (void*)source_fol));
						}
*/

			if (sd_file_exists("emummc/emummc.ini"))
			{
					if(!sd_file_exists("StarDust/emunand_serial.txt"))
					{
						sd_save_to_file("", 0, "StarDust/emunand_serial.txt");						
						Incognito("1");
					}
					
					void *buf = sd_file_read("StarDust/emunand_serial.txt");
						emuserial = buf;

					if(strstr(emuserial,"XAW0000000000") != NULL)
					{emu_inc = 1;}
					
					if (sd_file_exists ("exosphere.ini")){
						if (emu_blk == 1){
							gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sw-on.bmp"),1000, 100, 200, 75, (int (*)(void *))Incognito, (void*)"DBE"));
						}else{
							gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sw-off.bmp"),1000, 100, 200, 75, (int (*)(void *))Incognito, (void*)"BE"));
						}
					gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Blank_Prodinfo", 1020,90, 150, 100, NULL, NULL));
					}

					gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("SysNand", 170,200, 150, 100, NULL, NULL));
					gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("EmuNand", 970,200, 150, 100, NULL, NULL));

					rowinc = rowinc - rowsepar;
					colinc = colinc + colsepar;
					gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(emuserial, rowinc+30,colinc-20, 150, 100, NULL, NULL));
					if (!sd_file_exists("prodinfo_emunand.bin"))
					gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/inc3.bmp"),rowinc, colinc, 200, 75,(int (*)(void *))Incognito, (void*)"2"));
					else
					gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/inc0-off.bmp"),rowinc, colinc, 200, 75,NULL, NULL));

					rowinc = rowinc - rowsepar;
					colinc = colinc + colsepar;
					if(emu_inc == 0)
					gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/inc4.bmp"),rowinc, colinc, 200, 75,(int (*)(void *))Incognito, (void*)"3"));
					else
					gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/inc1-off.bmp"),rowinc, colinc, 200, 75,NULL, NULL));

					rowinc = rowinc - rowsepar;
					colinc = colinc + colsepar;
					if (sd_file_exists("prodinfo_emunand.bin"))
					gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/inc5.bmp"),rowinc, colinc, 200, 75,(int (*)(void *))Incognito, (void*)"4"));
					else{
					gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/inc2-off.bmp"),rowinc, colinc, 200, 75,NULL, NULL));
						if(emu_inc == 1){
						gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Missing", rowinc+30,colinc+100, 150, 100, NULL, NULL));						
						gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("/prodinfo_emunand.bin", rowinc+30,colinc+120, 150, 100, NULL, NULL));						
						}
					}

			}else{
				rowinc = 550;
				rowsepar = 0;
			}

		rowinc = 0+xcol;
		colinc = 150;
		rowsepar = 50;
		colsepar = 100;
			if (sd_file_exists("StarDust/sysnand_serial.txt"))
			{
				
					void *buf = sd_file_read("StarDust/sysnand_serial.txt");
					sysserial = buf;
					if(strstr(sysserial,"XAW0000000000") != NULL)
						sys_inc = 1;

					if (sd_file_exists("emummc/emummc.ini"))
					{
						void *buf = sd_file_read("StarDust/emunand_serial.txt");
						emuserial = buf;
					}else{
					rowinc = 550;
					rowsepar = 0;
					}

					if(strstr(emuserial,"XAW0000000000") != NULL)
						emu_inc = 1;


				rowinc = rowinc + rowsepar;
				colinc = colinc + colsepar;
				
				if (sd_file_exists ("exosphere.ini")){
					if (sys_blk == 1)
					{
						gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sw-on.bmp"),rowinc-8-rowsepar, 100, 200, 75, (int (*)(void *))Incognito, (void*)"DBS"));
					}else{
						gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sw-off.bmp"),rowinc-8-rowsepar, 100, 200, 75, (int (*)(void *))Incognito, (void*)"BS"));
					}
				gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Blank_Prodinfo", rowinc+10-rowsepar,90, 150, 100, NULL, NULL));
				}
				gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(sysserial, rowinc+40,colinc-20, 150, 100, NULL, NULL));
				if (!sd_file_exists("prodinfo_sysnand.bin"))
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/inc0.bmp"),rowinc, colinc, 200, 75,(int (*)(void *))Incognito, (void*)"5"));
				else
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/inc0-off.bmp"),rowinc, colinc, 200, 75,NULL, NULL));
				
				rowinc = rowinc + rowsepar;
				colinc = colinc + colsepar;
				if(sys_inc == 0)
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/inc1.bmp"),rowinc, colinc, 200, 75,(int (*)(void *))Incognito, (void*)"6"));
				else
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/inc1-off.bmp"),rowinc, colinc, 200, 75,NULL, NULL));
				
				rowinc = rowinc + rowsepar;
				colinc = colinc + colsepar;
				
				if (sd_file_exists("prodinfo_sysnand.bin"))
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/inc2.bmp"),rowinc, colinc, 200, 75,(int (*)(void *))Incognito, (void*)"7"));
				else{
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/inc2-off.bmp"),rowinc, colinc, 200, 75,NULL, NULL));
					if(sys_inc == 1){
					gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Missing", rowinc+30,colinc+100, 150, 100, NULL, NULL));						
					gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("/prodinfo_sysnand.bin", rowinc+30,colinc+120, 150, 100, NULL, NULL));						
					}
				
				}
			
			}else{
				gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Biskeys error, try again", rowinc+30,colinc-10, 150, 100, NULL, NULL));
				}


				
		//		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/arrowl.bmp"),200, low_icons, 70, 70,(int (*)(void *))tool_Menus, (void*)1));
		}

	}

	//Menu 5 Homebrew
	if(main_menu == 5)
	{
			//List Files And folders
			char* HBpath = "/switch";
			char* folder = listfol(HBpath, "*", true);				
			u32 r = 0; u32 i = 0; u32 f = 0; u32 y = 60; u32 x = 150; u32 space = 80;
				
			//nav Buttons
//			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/root.bmp"),170, 1, 70, 70,(int (*)(void *))tool_Menus, (void*)33));
//			if(strlen(HBpath) >= 1)
//			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sd-back.bmp"),270, 1, 70, 70,(int (*)(void *))tool_file_act, (void*)99));
		
			//display folder
			gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(HBpath, strlen(HBpath)*8-40+150, 30, 5, 5, NULL, NULL));
			gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Homebrew", strlen(HBpath)*8-40+150, 50, 5, 5, NULL, NULL));
			gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Homebrew      Toogle.", strlen(HBpath)*8-40+890, 690, 5, 5, NULL, NULL));
			
			while(folder[r * 256])
			{
			if(f >= 33){gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("More...", 1100,690, 150, 100, NULL, NULL));	break;}
				if(strlen(&folder[r * 256]) <= 100){			
				char folname[20];
				char *str = &folder[r * 256];
				int h;
				for (h = 0;h< 19;h++)
				folname[h] = str[h];
				folname[h] = '\0';
				if((strstr(folname,".overlays") == NULL)&(strstr(folname,"XXX") == NULL)){		
					char* source_fol = (char*)malloc(256);
					char* source_folder = (char*)malloc(256);
					strcpy(source_fol, "/switch");
					strcat(source_fol, "/");
					strcat(source_fol, folname);
					strcpy(source_folder ,source_fol);
					strcat(source_fol, "/hide.flag");
						if (sd_file_exists (source_fol)){
							f_chmod(source_folder, AM_ARC, AM_ARC);
							gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sw-off.bmp"),x, y+30, 200, 75,(int (*)(void *))HBhide, (void*)source_fol));
						}else{
							f_chmod(source_folder, AM_RDO, AM_RDO | AM_ARC);
							gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sw-on.bmp"),x, y+30, 200, 75,(int (*)(void *))HBhide, (void*)source_fol));
						}
//					gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(folname, x+strlen(folname)*8-80, y+25, 150, 0, NULL, NULL));
					gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(folname, x+25, y+25, 150, 0, NULL, NULL));
					y = y + space;
				}

				f++;
				i++;
				if((i == 8)||(i == 15)){
					y = 60;
					if(i == 15){i = 0;x = 940;}else x = 545;
					}
				}
			r++;
			}


		
		
		
		//call menu 1
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/gear.bmp"),1200, low_icons, 70, 70, (int (*)(void *))tool_Menus, (void*)1));
	}
		 

/* Generate permanent entries**/
//if (help_switch == 0)
//gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/logos/logoSD.bmp"),1, 630, 100, 100, NULL, NULL));
//gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/logos/logoSD.bmp"),1, 630, 100, 100,(int (*)(void *)) help_display, NULL));
//else
//gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/help.bmp"),10, 650, 70, 70, (int (*)(void *))tool_Menus, (void*)4));

//help_switch = 0;
if(main_menu != 0)
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/home.bmp"),10, low_icons, 70, 70,(int (*)(void *))tool_Menus, (void*)0));

if(main_menu != 1)
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/power.bmp"),605, low_icons, 70, 70, tool_power_off, NULL));
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read(""),0, 0, 35, 35, (int (*)(void *))screenshot, NULL));
//battery
u32 battPercent = 0;
max17050_get_property(MAX17050_RepSOC, (int *)&battPercent);		
u32 battimgper = (battPercent >> 8) & 0xFF;
u32 batY = 5;
u32 batX = 1200;
if ((battimgper <= 100) & (battimgper >= 76))
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/bat1.bmp"),batX, batY, 70, 35,(int (*)(void *))bat_show, (void*)battimgper));
if ((battimgper <= 75) & (battimgper >= 51))
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/bat2.bmp"),batX, batY, 70, 35,(int (*)(void *))bat_show, (void*)battimgper));
if ((battimgper <= 50) & (battimgper >= 26))
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/bat3.bmp"),batX, batY, 70, 35,(int (*)(void *))bat_show, (void*)battimgper));
if ((battimgper <= 25) & (battimgper >= 3))
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/bat4.bmp"),batX, batY, 70, 35,(int (*)(void *))bat_show, (void*)battimgper));
if (battimgper <= 2)
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/bat5.bmp"),batX, batY, 70, 35,(int (*)(void *))bat_show, (void*)battimgper));

    /* Start menu */
	Ajbrillo(0);
    gui_menu_open(menu);

    /* Clear all entries and menus */
    gui_menu_pool_cleanup();
}

static int tool_reboot_rcm(void* param)
{
   reboot_rcm();
    return 0;
}

static int tool_power_off(void* param)
{
    gui_menu_pool_cleanup();
    power_off();
    return 0;
}

//eject sd card
static int tool_extr_rSD(void* param)
{
if (!sd_mount()){BootStrapNX();}//check sd
	gfx_swap_buffer(&g_gfx_ctxt);
	Ajbrillo(0);
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
	BootStrapNX();
    return 0;
}

//Emu tool
static int tool_emu(u32 status)
{
if (!sd_mount()){BootStrapNX();}//check sd
	
	//give sxos emu to ams
	if(status == 33)
	{
	f_unlink("sxos/eMMC");
	Ajbrillo(1);
	f_mkdir("emuMMC");
	f_mkdir("emuMMC/EF00");
	f_rename("/sxos/emunand","/emuMMC/EF00/eMMC");
	f_rename("/Emutendo","/emuMMC/EF00/Nintendo");
	
	f_rename("emuMMC/EF00/eMMC/boot0.bin","emuMMC/EF00/eMMC/boot0");
	f_rename("emuMMC/EF00/eMMC/boot1.bin","emuMMC/EF00/eMMC/boot1");

	f_rename("emuMMC/EF00/eMMC/full.00.bin","emuMMC/EF00/eMMC/00");
	f_rename("emuMMC/EF00/eMMC/full.01.bin","emuMMC/EF00/eMMC/01");
	f_rename("emuMMC/EF00/eMMC/full.02.bin","emuMMC/EF00/eMMC/02");
	f_rename("emuMMC/EF00/eMMC/full.03.bin","emuMMC/EF00/eMMC/03");
	f_rename("emuMMC/EF00/eMMC/full.04.bin","emuMMC/EF00/eMMC/04");
	f_rename("emuMMC/EF00/eMMC/full.05.bin","emuMMC/EF00/eMMC/05");
	f_rename("emuMMC/EF00/eMMC/full.06.bin","emuMMC/EF00/eMMC/06");
	f_rename("emuMMC/EF00/eMMC/full.07.bin","emuMMC/EF00/eMMC/07");
	f_rename("emuMMC/EF00/eMMC/full.08.bin","emuMMC/EF00/eMMC/08");
	f_rename("emuMMC/EF00/eMMC/full.09.bin","emuMMC/EF00/eMMC/09");
	f_rename("emuMMC/EF00/eMMC/full.10.bin","emuMMC/EF00/eMMC/10");
	f_rename("emuMMC/EF00/eMMC/full.11.bin","emuMMC/EF00/eMMC/11");
	f_rename("emuMMC/EF00/eMMC/full.12.bin","emuMMC/EF00/eMMC/12");
	f_rename("emuMMC/EF00/eMMC/full.13.bin","emuMMC/EF00/eMMC/13");
	f_rename("emuMMC/EF00/eMMC/full.14.bin","emuMMC/EF00/eMMC/14");
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

		gui_init_argon_menu();
	return 0;
	}

	//return emunand to sxos
	if(status == 66)
	{
	f_unlink("sxos/emunand");
	Ajbrillo(1);
	f_rename("/emuMMC/EF00/eMMC","/sxos/emunand");
	
	f_rename("sxos/emunand/boot0","sxos/emunand/boot0.bin");
	f_rename("sxos/emunand/boot1","sxos/emunand/boot1.bin");

	f_rename("sxos/emunand/00","sxos/emunand/full.00.bin");
	f_rename("sxos/emunand/01","sxos/emunand/full.01.bin");
	f_rename("sxos/emunand/02","sxos/emunand/full.02.bin");
	f_rename("sxos/emunand/03","sxos/emunand/full.03.bin");
	f_rename("sxos/emunand/04","sxos/emunand/full.04.bin");
	f_rename("sxos/emunand/05","sxos/emunand/full.05.bin");
	f_rename("sxos/emunand/06","sxos/emunand/full.06.bin");
	f_rename("sxos/emunand/07","sxos/emunand/full.07.bin");
	f_rename("sxos/emunand/08","sxos/emunand/full.08.bin");
	f_rename("sxos/emunand/09","sxos/emunand/full.09.bin");
	f_rename("sxos/emunand/10","sxos/emunand/full.10.bin");
	f_rename("sxos/emunand/11","sxos/emunand/full.11.bin");
	f_rename("sxos/emunand/12","sxos/emunand/full.12.bin");
	f_rename("sxos/emunand/13","sxos/emunand/full.13.bin");
	f_rename("sxos/emunand/14","sxos/emunand/full.14.bin");
	f_rename("/emuMMC/EF00/Nintendo","/Emutendo");
		if (sd_file_exists ("sxos/emunand/full.00.bin")&sd_file_exists ("sxos/emunand/boot0.bin")&sd_file_exists ("sxos/emunand/boot1.bin"))
		{
			f_unlink("emummc/emummc.ini");
			f_rename("emummc/emummc.bak","emummc/emummc.ini");
			f_unlink("emuMMC/EF00/file_based");
			f_unlink("/emuMMC/EF00");
			f_unlink("emummc");
			retir = 0;
		}

		gui_init_argon_menu();
	return 0;
	}

	
	//link sxos hide partition to ams
	if(status == 99)
	{
	Ajbrillo(1);
			f_mkdir("emummc");
			FIL fp;
			f_open(&fp, "emummc/emummc.ini", FA_WRITE | FA_CREATE_ALWAYS);
			f_puts("[emummc]\n", &fp);
			f_puts("enabled=1\n", &fp);
			f_puts("sector=0x2\n", &fp);
			f_puts("nintendo_path=Emutendo\n", &fp);
			f_close(&fp);
		gui_init_argon_menu();
	return 0;
	}

	




	if(status == 1)
	{
	char *str1 = sd_file_read("emummc/emummc.ini");
	char* payload_wo_bin = str_replace(str1, "enabled=0", "enabled=1");
	FIL op;
	f_open(&op, "emummc/emummc.ini", FA_READ);
	u32 size = f_size(&op);
	f_close(&op);
	sd_save_to_file(payload_wo_bin,size,"emummc/emummc.ini");
	retir = 2;
	}

	if(status == 0)
	{
	char *str1 = sd_file_read("emummc/emummc.ini");
	char* payload_wo_bin = str_replace(str1, "enabled=1", "enabled=0");
	FIL op;
	f_open(&op, "emummc/emummc.ini", FA_READ);
	u32 size = f_size(&op);
	f_close(&op);
	sd_save_to_file(payload_wo_bin,size,"emummc/emummc.ini");
	retir = 1;
	}

/*	
if (sd_file_exists ("emummc/emummc.ini"))
f_rename("emummc/emummc.ini.bak","emummc/emummc.ini");
*/
	gui_init_argon_menu();
return 0;
}

static int tool_Menus(u32 param)
{
if (!sd_mount()){BootStrapNX();}//check sd

if(param == 33){
directory = "";
gui_init_argon_menu();
return 0;
}
Ajbrillo(1);
//check if is the same menu
if(main_menu == param)
return 0;

if(param == 0)
{
directory = "";
filete = "";
}
//set menu number
main_menu = param;

//gui_menu_pool_cleanup();
gui_init_argon_menu();
return 0;
}

int tool_servises_on(char* title)
{
if (!sd_mount()){BootStrapNX();}//check sd
Ajbrillo(1);
char* path = (char*)malloc(256);
		strcpy(path, "atmosphere/contents/");
		strcat(path, title);
		strcat(path, "/flags");
		f_mkdir(path);
		strcat(path, "/boot2.flag");
		sd_save_to_file("",0,path);
		strcpy(path, "sxos/titles/");
		strcat(path, title);
		strcat(path, "/flags");
		f_mkdir(path);
		strcat(path, "/boot2.flag");
		sd_save_to_file("",0,path);
gui_init_argon_menu();
return 0;
}

int tool_servises_off(char* title)
{
if (!sd_mount()){BootStrapNX();}//check sd
Ajbrillo(1);
char* path = (char*)malloc(256);
		strcpy(path, "atmosphere/contents/");
		strcat(path, title);
		strcat(path, "/flags/boot2.flag");
		f_unlink(path);
		strcpy(path, "sxos/titles/");
		strcat(path, title);
		strcat(path, "/flags/boot2.flag");
		f_unlink(path);

gui_init_argon_menu();
return 0;
}

//Themes ON
int tool_Themes_on(char* cfw)
{
if (!sd_mount()){BootStrapNX();}//check sd
char* path = (char*)malloc(256);
		if(strstr(cfw,"sxos") != NULL)
		{
		strcpy(path, cfw);
		strcat(path, "/titles/0100000000001000/fsmitm.flag");
		sd_save_to_file("",0,path);
		}
		
		if(strstr(cfw,"Profile") != NULL)
		moverall("/atmosphere/contents/0100000000001013-OFF", "atmosphere/contents/0100000000001013", "*","");
		
		if(strstr(cfw,"atmosphere") != NULL)
		{
		moverall("/atmosphere/contents/0100000000001000/sfmor", "/atmosphere/contents/0100000000001000/romfs", "*","");
//		copyarall("/atmosphere/contents/0100000000001000/sfmor", "/atmosphere/contents/0100000000001000/romfs", "*","");
		sd_save_to_file("",0, "atmosphere/contents/0100000000001000/fsmitm.flag");
		}
gui_init_argon_menu();
return 0;
}

//Themes OFF
int tool_Themes_off(char* cfw)
{
if (!sd_mount()){BootStrapNX();}//check sd
char* path = (char*)malloc(256);
		if(strstr(cfw,"sxos") != NULL)
		{
			strcpy(path, cfw);
			strcat(path, "/titles/0100000000001000/fsmitm.flag");
			f_unlink(path);
		}
		
		if(strstr(cfw,"Profile") != NULL)
		moverall("/atmosphere/contents/0100000000001013", "atmosphere/contents/0100000000001013-OFF", "*","");
		
		if(strstr(cfw,"atmosphere") != NULL)
		{
//			deleteall("/atmosphere/contents/0100000000001000/romfs", "*", "");
			moverall("/atmosphere/contents/0100000000001000/romfs", "/atmosphere/contents/0100000000001000/sfmor", "*","");
			f_unlink("atmosphere/contents/0100000000001000/fsmitm.flag");
		}
gui_init_argon_menu();
return 0;
}

//file mgr dir change
int tool_dir(char *param)
{
if (!sd_mount()){BootStrapNX();}//check sd
Ajbrillo(1);
	if(strlen(directory) < 1)
	{
		directory = param;
	}else{
		strcat(directory, "/");
		strcat(directory, param);
	}
gui_init_argon_menu();
return 0;
}

//hacer doble toque file mgr
int tool_filete(char* fil)
{
if (!sd_mount()){BootStrapNX();}//check sd
	if(strlen(directory) <= 1)
	{filete = fil;
	}else{
		char tmp[256];
		strcpy(tmp, directory);
		strcat(tmp, "/");
		strcat(tmp, fil);
	filete = tmp;		
	}
	
	if(strstr(filete,filpay) != NULL)
	{
		if(strstr(filete,".bin") != NULL)
		{
		tool_file_act(1);
		}
		if(strstr(filete,".BIN") != NULL)
		{
		tool_file_act(1);
		}
		
		if(strstr(filete,".txt") != NULL)
		{
		tool_file_act(2);
		}
		if(strstr(filete,".ini") != NULL)
		{
		tool_file_act(2);
		}
		if(strstr(filete,".conf") != NULL)
		{
		tool_file_act(2);
		}
	}
filpay = filete;
gui_init_argon_menu();
return 0;
}

//flie browser actions
int tool_file_act(u32 fil)
{
if (!sd_mount()){BootStrapNX();}//check sd

	//delete files
	if(fil == 0)
	{
		f_unlink(filete);
		free(filete);
		filete = "";
	}
	
	//launch payload 
	if(fil == 1)
	{
		launch_payload(filete);
	}
	
	//read text file
	if(fil == 2)
	{
		gfx_swap_buffer(&g_gfx_ctxt);
		g_gfx_con.scale = 2;
		gfx_con_setpos(&g_gfx_con, 1, 1);
		gfx_printf(&g_gfx_con, "\nText Reader\n",filete);
		gfx_con_setcol(&g_gfx_con, 0xFF008F39, 0xFF726F68, 0xFF191414);
		gfx_printf(&g_gfx_con, "\n%s...\n",filete);
		gfx_printf(&g_gfx_con, "\n-------------------------------------------------------------------------------------------------------------------\n",filete);
		gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0, 0xFF191414);
		g_gfx_con.scale = 2;
		gfx_printf(&g_gfx_con, "\n%s",sd_file_read(filete));
		gfx_swap_buffer(&g_gfx_ctxt);
		gui_menu_pool_init();
		gui_menu_t* menu = gui_menu_create("ArgonNX",main_menu);
		gui_menu_open3(menu);

	}
	
	//Go back directory
	if(fil == 99)
	{
	
	//identify last '/' caracter
	char *outFilename = directory;
	u32 sdPathLen = strlen(directory);
	char recip[0];
		while(true)
		{
		recip[0] = outFilename[sdPathLen--];
		if(strstr(recip,"/") != NULL){
		break;}
		
		if(sdPathLen <= 0){
		directory = "";
		break;}
		}
	
		if(strlen(directory) >= 1)
		{
			//take position of last / and contruct the variable before hem
			char name[100];
			gfx_printf(&g_gfx_con, "-\n\n\n\n\n\n\n");
			u32 i= 0;
			for (i = 0;sdPathLen >= i;i++)
			{
				name[i] = outFilename[i];
				gfx_printf(&g_gfx_con, "name-%s-%d-%d\n",name,i,sdPathLen);
			}
			name[sdPathLen+1] = '\0';
				gfx_printf(&g_gfx_con, "name-%s-%d-%d\n",name,i,sdPathLen);
			directory = name;
//			gfx_swap_buffer(&g_gfx_ctxt);
//			msleep(5000);
		}
	}
	
	
gui_init_argon_menu();
return 0;
}

//safe boot
static int tool_menu_rem(void* param)
{
if (!sd_mount()){BootStrapNX();}//check sd
 gfx_printf(&g_gfx_con, "loading\n");
//gui_menu_pool_cleanup();
	f_unlink("/atmosphere/contents/0100000000001000/fsmitm.flag");
	f_unlink("/atmosphere/contents/0100000000001000/romfs_metadata.bin");
	deleteall("/atmosphere/contents/0100000000001000/romfs", "*","");
	f_unlink("/SXOS/titles/0100000000001000/fsmitm.flag");
	f_unlink("/sxos/titles/0100000000001000/romfs_metadata.bin");
	f_unlink("/sxos/titles/0100000000000034-OFF/exefs.nsp");
	f_unlink("/sxos/titles/0100000000000034-OFF");
	f_rename("/sxos/titles/0100000000000034", "/sxos/titles/0100000000000034-OFF");
	f_unlink("/sxos/titles/0100000000000034/exefs.nsp");
	//this is a panic option so i will disable the servises also
	char* folder_ams = listfol("/atmosphere/contents", "*", true);
			u32 r = 0;
		while(folder_ams[r * 256])
		{
			if(strlen(&folder_ams[r * 256]) <= 100){			
			char* source_ams = (char*)malloc(256);
			strcpy(source_ams, "/atmosphere/contents/");
			strcat(source_ams, &folder_ams[r * 256]);
			strcat(source_ams, "/flags/boot2.flag");
			f_unlink(source_ams);
			}
		r++;
		}
/*
	char* folder_rei = listfol("/ReiNX/titles", "*", true);
			u32 w = 0;
		while(folder_rei[w * 256])
		{
			if(strlen(&folder_rei[w * 256]) <= 100){			
			char* source_rei = (char*)malloc(256);
			strcpy(source_rei, "/ReiNX/titles/");
			strcat(source_rei, &folder_rei[w * 256]);
			strcat(source_rei, "/boot2.flag");
			f_unlink(source_rei);
			}
		w++;
		}
*/		
	char* folder_sxos = listfol("/sxos/titles", "*", true);
			u32 e = 0;
		while(folder_sxos[e * 256])
		{
			if(strlen(&folder_sxos[e * 256]) <= 100){			
			char* source_sxos = (char*)malloc(256);
			strcpy(source_sxos, "/sxos/titles/");
			strcat(source_sxos, &folder_sxos[e * 256]);
			strcat(source_sxos, "/flags/boot2.flag");
			f_unlink(source_sxos);
			}
		e++;
		}

	//reactivate ams critical serv
	sd_save_to_file("", 0, "/atmosphere/contents/0100000000000032/flags/boot2.flag");
	sd_save_to_file("", 0, "/atmosphere/contents/0100000000000037/flags/boot2.flag");
    sd_save_to_file("", 0, "fixer.del");
launch_payload("payload.bin");
return 0;
}

//Themes///// copy
int tool_theme(char* param)
{
if (!sd_mount()){BootStrapNX();}//check sd
Ajbrillo(1);
gfx_swap_buffer(&g_gfx_ctxt);
copyarall(param, "/StarDust", "*", "Applying Theme");
gui_init_argon_menu();
return 0;
}

//Memloader
int memloader(u32 fil)
{
if (!sd_mount()){BootStrapNX();}//check sd
	//boot0
	if(fil == 0)
	{
			FIL fp;
			f_open(&fp, "auto.bak", FA_WRITE | FA_CREATE_ALWAYS);
			f_puts("[load:PH_0]\n", &fp);
			f_puts("if=StarDust/uboot/u-boot.elf\n", &fp);
			f_puts("skip=0x00010000\n", &fp);
			f_puts("count=0x0006e13f\n", &fp);
			f_puts("dst=0x80110000\n", &fp);
			f_puts("\n", &fp);
			f_puts("[load:script]\n", &fp);
			f_puts("if=StarDust/uboot/ums_emmc_boot0.scr.img\n", &fp);
			f_puts("dst=0x80100000\n", &fp);
			f_puts("\n", &fp);
			f_puts("[boot:ENTRY]\n", &fp);
			f_puts("pc=0x80110000\n", &fp);
			f_puts("maxMemoryFreq=200\n", &fp);
			f_puts("pwroffHoldTime=4\n", &fp);
			f_puts("\n", &fp);
			f_close(&fp);
			launch_payload("StarDust/payback/memloader.bin");
	}

	//boot1
	if(fil == 1)
	{
			FIL fp;
			f_open(&fp, "auto.bak", FA_WRITE | FA_CREATE_ALWAYS);
			f_puts("[load:PH_0]\n", &fp);
			f_puts("if=StarDust/uboot/u-boot.elf\n", &fp);
			f_puts("skip=0x00010000\n", &fp);
			f_puts("count=0x0006e13f\n", &fp);
			f_puts("dst=0x80110000\n", &fp);
			f_puts("\n", &fp);
			f_puts("[load:script]\n", &fp);
			f_puts("if=StarDust/uboot/ums_emmc_boot1.scr.img\n", &fp);
			f_puts("dst=0x80100000\n", &fp);
			f_puts("\n", &fp);
			f_puts("[boot:ENTRY]\n", &fp);
			f_puts("pc=0x80110000\n", &fp);
			f_puts("maxMemoryFreq=-1600\n", &fp);
			f_puts("pwroffHoldTime=4\n", &fp);
			f_puts("\n", &fp);
			f_close(&fp);
			launch_payload("StarDust/payback/memloader.bin");
	}

	//emmc
	if(fil == 2)
	{
			FIL fp;
			f_open(&fp, "auto.bak", FA_WRITE | FA_CREATE_ALWAYS);
			f_puts("[load:PH_0]\n", &fp);
			f_puts("if=StarDust/uboot/u-boot.elf\n", &fp);
			f_puts("skip=0x00010000\n", &fp);
			f_puts("count=0x0006e13f\n", &fp);
			f_puts("dst=0x80110000\n", &fp);
			f_puts("\n", &fp);
			f_puts("[load:script]\n", &fp);
			f_puts("if=StarDust/uboot/ums_emmc.scr.img\n", &fp);
			f_puts("dst=0x80100000\n", &fp);
			f_puts("\n", &fp);
			f_puts("[boot:ENTRY]\n", &fp);
			f_puts("pc=0x80110000\n", &fp);
			f_puts("pwroffHoldTime=4\n", &fp);
			f_puts("\n", &fp);
			f_close(&fp);
			launch_payload("StarDust/payback/memloader.bin");
	}
	
	if(fil == 3)
	{
	//sd
			FIL fp;
			f_open(&fp, "auto.bak", FA_WRITE | FA_CREATE_ALWAYS);
			f_puts("[load:PH_0]\n", &fp);
			f_puts("if=StarDust/uboot/u-boot.elf\n", &fp);
			f_puts("skip=0x00010000\n", &fp);
			f_puts("count=0x0007c1bf\n", &fp);
			f_puts("dst=0x80110000\n", &fp);
			f_puts("\n", &fp);
			f_puts("[load:script]\n", &fp);
			f_puts("if=StarDust/uboot/ums_sd.scr.img\n", &fp);
			f_puts("dst=0x80100000\n", &fp);
			f_puts("\n", &fp);
			f_puts("[boot:ENTRY]\n", &fp);
			f_puts("pc=0x80110000\n", &fp);
			f_puts("pwroffHoldTime=4\n", &fp);
			f_puts("\n", &fp);
			f_close(&fp);
			launch_payload("StarDust/payback/memloader.bin");
	}
return 0;
}

int bat_show(u32 percent)
{
		gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0xFFCCCCCC, 0xFF191414);
		gfx_con_setpos(&g_gfx_con, 1200, 15);
        gfx_printf(&g_gfx_con, "%k%d%%%k", 0xFF00FF22,percent,0xFFCCCCCC);
//      gfx_printf(&g_gfx_con, "%k%d%%%k", 0xFF00FF22,percent,0xFFCCCCCC);
		gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0, 0xFF191414);
		return 0;
}

void serv_display(gui_menu_t* menu,char* titleid,char* name) 
{
if (!sd_mount()){BootStrapNX();}//check sd
		char* path = (char*)malloc(256);
		strcpy(path, "atmosphere/contents/");
		strcat(path, titleid);
		strcat(path, "/exefs.nsp");

		char* flagpath = (char*)malloc(256);
		strcpy(flagpath, "atmosphere/contents/");
		strcat(flagpath, titleid);
		strcat(flagpath, "/flags/boot2.flag");

	if (sd_file_exists(path) && servstep <= 9)
	{
		if (sd_file_exists(flagpath))
		{
	gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sw-on.bmp"),servX, servY, 200, 75,(int (*)(void *))tool_servises_off, (void*)titleid));
		}else{
	gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sw-off.bmp"),servX, servY, 200, 75,(int (*)(void *))tool_servises_on, (void*)titleid));
		}
	gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(name,servX+30, servY+30, 150, 100, NULL, NULL));
		servstep++;
		if(servstep == 2 || servstep == 4 || servstep == 6 || servstep == 8|| servstep == 10)
		{
			servY = servYF;
			servX = servX + 205;
//			servstep = 0;
		}else{
		servY = servY + sepaserv;		
		}
	}
}

void help_display(void) 
{
if (!sd_mount()){BootStrapNX();}//check sd
help_switch++;
gui_init_argon_menu();
}

int Incognito(char* order)
{
if (!sd_mount()){BootStrapNX();}//check sd
if(strstr(order,"B") != NULL){
		
	if(strstr(order,"BS") != NULL)
	{
		buffer_blk = str_replace(buffer_blk, "blank_prodinfo_sysmmc=0", "blank_prodinfo_sysmmc=1");
	}

	if(strstr(order,"DBS") != NULL)
	{
		buffer_blk = str_replace(buffer_blk, "blank_prodinfo_sysmmc=1", "blank_prodinfo_sysmmc=0");
	}

	if(strstr(order,"BE") != NULL)
	{
		buffer_blk = str_replace(buffer_blk, "blank_prodinfo_emummc=0", "blank_prodinfo_emummc=1");
	}

	if(strstr(order,"DBE") != NULL)
	{
		buffer_blk = str_replace(buffer_blk, "blank_prodinfo_emummc=1", "blank_prodinfo_emummc=0");
	}

	u32 size = strlen(buffer_blk)-1;
	sd_save_to_file(buffer_blk, size, "exosphere.ini");
	gui_init_argon_menu();
return 0;
}



		sd_save_to_file(order, 1, "StarDust/autoboot.inc");
		launch_payload("StarDust/payback/Incognito_RCM.bin");
	return 0;
}
int Autoboot(u32 fil)
{
	if (!sd_mount()){BootStrapNX();}//check sd
	if(fil == 0)
		f_unlink("StarDust/autobootecho.txt");
	
	if(fil == 1)
		sd_save_to_file("", 0, "StarDust/autobootecho.txt");
	
	gui_init_argon_menu();
return 0;
};

int AThemes_list(gui_menu_t* menu, u32 gridX, u32 gridY)
{
		//generate themes Dinamy
	u32 tm_ajust = 90;
	char* tm_directory = "StarDust/Atheme";
	char* folder = listfol(tm_directory, "*", true);
			u32 r = 0;
			u32 w = 0;
		while(folder[r * 256])
		{
			if(strlen(&folder[r * 256]) <= 100){			
			char* source_fol = (char*)malloc(256);
			strcpy(source_fol, tm_directory);
			strcat(source_fol, "/");
			strcat(source_fol, &folder[r * 256]);
			char* source_icon = (char*)malloc(256);
			strcpy(source_icon, source_fol);
			strcat(source_icon, "/icon.bmp");
			if(sd_file_exists(source_icon)){
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read(source_icon),gridX, gridY, 70, 70, (int (*)(void *))tool_theme, (void*)source_fol)); 
			gridX = gridX+tm_ajust;}
			w++;
			}
		if(w == 11)
		break;
		r++;
		}
return 0;
}

void medislay(char *flags)
{
if (!sd_mount()){BootStrapNX();}//check sd
	if (sd_file_exists("StarDust/flags/b50.flag"))
	f_unlink("StarDust/flags/b50.flag");
		else
	sd_save_to_file("", 0, "StarDust/flags/b50.flag");
	Ajbrillo(0);
	gui_init_argon_menu();
}

int uLaunch(u32 fil)
{
	if (!sd_mount()){BootStrapNX();}//check sd
	if(fil == 0)
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


	if(fil == 1)
	{
		copyarall("/StarDust/ulaunch/ulaunch", "/ulaunch", "*","Installing ulaunch");
		copyarall("/StarDust/ulaunch/titles", "/atmosphere/contents", "*","Installing ulaunch");
//		copyarall("/StarDust/ulaunch/titles", "/ReiNX/titles", "*","Installing ulaunch");
		copyarall("/StarDust/ulaunch/titles", "/sxos/titles", "*","Installing ulaunch");
		f_mkdir("StarDust/flags");
		sd_save_to_file("", 0, "StarDust/flags/ulaunch.flag");
	}
	
	gui_init_argon_menu();
return 0;
}

void hekateOFW(u32 tipo)
{
if (!sd_mount()){BootStrapNX();}//check sd
	if(tipo == 0)
		copyfile("bootloader/hekate_ipl.conf","bootloader/hekate_ipl.ini");

	if(tipo == 1)
		copyfile("bootloader/stock","bootloader/hekate_ipl.ini");
	
launch_payload("/StarDust/payloads/zHekate.bin");
}

void HBhide(char* folder){
if (!sd_mount()){BootStrapNX();}//check sd
	char* source_fol = (char*)malloc(256);
	strcpy(source_fol, "/switch");
	strcat(source_fol, "/");
	strcat(source_fol, folder);
	strcat(source_fol, "/hide.flag");
	if (sd_file_exists (folder))
	f_unlink(folder);
	else
	sd_save_to_file("", 0, folder);
gui_init_argon_menu();
}


/*
void update_script(gui_menu_t* menu,char* titleid,char* name) 
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



		char* path = (char*)malloc(256);
		strcpy(path, "atmosphere/contents/");
		strcat(path, titleid);
		strcat(path, "/exefs.nsp");

		char* flagpath = (char*)malloc(256);
		strcpy(flagpath, "atmosphere/contents/");
		strcat(flagpath, titleid);
		strcat(flagpath, "/flags/boot2.flag");

	if (sd_file_exists(path))
	{
		if (sd_file_exists(flagpath))
		{
	gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sw-on.bmp"),servX, servY, 200, 75,(int (*)(void *))tool_servises_off, (void*)titleid));
		}else{
	gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sw-off.bmp"),servX, servY, 200, 75,(int (*)(void *))tool_servises_on, (void*)titleid));
		}
	gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(name,servX+30, servY+30, 150, 100, NULL, NULL));
	}
	servY = servY + sepaserv;
}

*/
/*
//	char *sd_path = "makunga/mitril/tormente";
	char *outFilename = sd_path;
	u32 sdPathLen = strlen(sd_path);
	char recip[0];
	while(true)
	{

	recip[0] = outFilename[sdPathLen--];
//	gfx_printf(&g_gfx_con, "-%s\n",recip);

	if(strstr(recip,"/") != NULL){
//	gfx_printf(&g_gfx_con, "1-%s-%d\n",recip,sdPathLen);
	break;}
	
	if(sdPathLen <= 0){
//	gfx_printf(&g_gfx_con, "2\n",recip);
	return 0;
	break;}

	}
	char name[100];
	u32 i = 0;
	
		while(true)
	{
		name[i] = outFilename[i];
		i++;
		if(sdPathLen+1 <= i)
		break;
	}
//	gfx_printf(&g_gfx_con, "%s",name);
	sd_path = name;
	return sd_path;
}






        // Traverse from right 
        for (int i = strlen(str) - 1; i >= 0; i--) 
 //           if (str[i] == x)
			if(strstr(str[i],x) != NULL)
                return i; 
        return -1; 


static int zbackup(char* param)
{
if (!sd_mount()){BootStrapNX();}//check sd
Ajbrillo(1);
sd_save_to_file("", 0, param);
launch_payload("StarDust/payloads/zbackup.bin");
return 0;
}


static int tool_menu_rSD(void* param)
{
	sd_mount();
		if (g_sd_mounted)
		{
		launch_payload("payload.bin");
		}

return 0;
}
*/

