

#include "utils/util.h"
#include "utils/btn.h"
#include "utils/fs_utils.h"
#include "soc/t210.h"
#include "soc/fuse.h"
#include "power/max77620.h"
#include "soc/pmc.h"
#include "soc/i2c.h"
#include "panic/panic.h"
#include "gfx/di.h"
#include "gfx/gfx.h"
#include "mem/heap.h"
#include <string.h>
#include "core/launcher.h"
#include "core/payloads.h"
#include "core/custom-gui.h"
#include "power/max17050.h"
#include "menu/tools/tools.h"
#define REVI_VERSION LOAD_BUILD_VER

__attribute__((noreturn)) void wait_for_button_and_reboot(void) {
    u32 button;
    while (true) {
        button = btn_read();
        if (button & BTN_POWER) {
            reboot_rcm();
        }
    }
}


char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

void strrep(char *orig, char *rep, char *with)
{
	char *result = str_replace(orig, rep, with);
	memset(orig,0,strlen(orig));
	strcat(orig,result);
}

u32 fusesB()
{
	u32 burntFuses = 0;
	for (u32 i = 0; i < 32; i++)
	{
		if ((fuse_read_odm(7) >> i) & 1)
			burntFuses++;
	}
return burntFuses;
}

char *fusesM()
{
	u32 burntFuses = 0;
	burntFuses = fusesB();
	char *mine;
	switch (burntFuses)
	{
            case 1:mine = "1.0.0-PegaScape"; break;
            case 2:mine = "2.0.0-PegaScape";break;
            case 3:mine = "3.0.0-PegaScape";break;
            case 4:mine = "3.0.1-PegaScape";break;
            case 5:mine = "4.0.0-PegaScape";break;
            case 6:mine = "5.0.0";break;
            case 7:mine = "6.0.0";break;
            case 8:mine = "6.2.0";break;
            case 9:mine = "7.0.0";break;
            case 10:mine = "8.1.0";break;
            case 11:mine = "9.0.0";break;
            case 12:mine = "9.2.0";break;
            case 13:mine = "10.0.0";break;
            case 14:mine = "11.0.0";break;
            case 15:mine = "12.1.0";break;
            case 16:mine = "12.2.0";break;//just guess
            default:mine = " -.-";
	}
return mine;
}

void SDStrap(){
//check sd
	if (!sd_mount())
	{
		BootStrapNX();
	} 
}

void BootStrapNX()
{
gfx_clear_buffer(&g_gfx_ctxt);
sd_unmount();
u32 battPercent = 0;
u32 letX = 20;
u32 letY = 380;

u32 a = 0;
u32 b = 100;
u32 c = 50;

u32 count = 0;
u32 countfull = 7*5;

u32 burntFuses = fusesB();
char* mindowngrade = fusesM();
g_gfx_con.mute = 0;
gfx_swap_buffer(&g_gfx_ctxt);
gfx_clear_buffer(&g_gfx_ctxt);
while (btn_read() & BTN_POWER);
display_backlight_brightness(a, 1000);
reboot_normal();
	while (true)
	{
		max17050_get_property(MAX17050_RepSOC, (int *)&battPercent);
		battPercent = (battPercent >> 8) & 0xFF;
		u32 res = 0, btn = btn_read();
		do
		{
			res = btn_read();
			if (battPercent < 5){power_off();}
			msleep(200);
			if (count>0){
				count--;
				if (count==0){display_backlight_brightness(a, 1000);}
				if (count==countfull/3){display_backlight_brightness(c, 1000);}
			}
		} while (btn == res);
		if (res & BTN_POWER) {
			if (btn_read() & BTN_VOL_UP){reboot_rcm();}
			
			//if hold power buton then power off
			if (btn_read() & BTN_POWER)
				msleep(1000);
			if (btn_read() & BTN_POWER){
				msleep(1000);
			}
			if (btn_read() & BTN_POWER)
				power_off();
			
			display_backlight_brightness(b, 1000);
			gfx_con_setpos(&g_gfx_con, 250, 230);
			if (sd_mount())
			{
				gfx_swap_buffer(&g_gfx_ctxt);
				gfx_clear_buffer(&g_gfx_ctxt);
				launch_payload("payload.bin");
				sd_unmount();
				gfx_printf(&g_gfx_con, "%kpayload.bin%k missing%k\n",0xFF008F39,0xFFea2f1e,0xFFF9F9F9);
			}else{
				gfx_printf(&g_gfx_con, "%kSD card Mount failed...%k\n",0xFFea2f1e,0xFFF9F9F9);
			}
			//if hold power buton then power off
			if (btn_read() & BTN_POWER)
			display_backlight_brightness(b, 1000);
			if (btn_read() & BTN_POWER)
			msleep(1000);
			if (btn_read() & BTN_POWER)
			power_off();
		}

		g_gfx_con.scale = 2;
		gfx_con_setpos(&g_gfx_con, 1,1);
		gfx_printf(&g_gfx_con,"%k%d%k%k%d%k%k%s%k\n\n",0xFF00FF22, REVI_VERSION ,0xFFCCCCCC, 0XFFEA2F1E, burntFuses ,0xFFCCCCCC ,0XFF331AD8 ,mindowngrade ,0xFFCCCCCC);
		g_gfx_con.scale = 3;
		gfx_con_setpos(&g_gfx_con, 1, 20);
		gfx_con_setcol(&g_gfx_con, 0xFF008F39, 0xFF726F68, 0xFF191414);
		gfx_printf(&g_gfx_con, "BootStrapNX\n");
		gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0, 0xFF191414);
		gfx_printf(&g_gfx_con, "StarDustCFW\n");
		gfx_con_setpos(&g_gfx_con, 1200, 10);
		gfx_printf(&g_gfx_con, "%d%%", battPercent);
		
		
		//Draw Footer
		g_gfx_con.scale = 3;
		gfx_con_setpos(&g_gfx_con, letX, letY+250);
		gfx_printf(&g_gfx_con, "Press %kPOWER%k To Boot %kpayload.bin%k\n",0xFF331ad8,0xFFF9F9F9,0xFF008F39,0xFFF9F9F9);
		gfx_con_setpos(&g_gfx_con, letX, letY+280);
		gfx_printf(&g_gfx_con, "Hold %kVol+ POWER%k To Reboot RCM\n",0xFF331ad8,0xFFF9F9F9);
		gfx_con_setpos(&g_gfx_con, letX, letY+310);
		gfx_printf(&g_gfx_con, "Hold %kPOWER%k To Full Power Off\n",0xFF331ad8,0xFFF9F9F9);
		
		display_backlight_brightness(b, 1000);
		gfx_swap_buffer(&g_gfx_ctxt);
		count=countfull;
	}
}

void printerCU(char *text,const char *title,int clean)
{
		static char titw[999] = "-.-";
		static char buff[999] = "\0";
		static int count = 0;
		if(strlen(title) <= 0){
			title=titw;
		}

		
		if (clean == 1){
			count = 0;
			strcpy(buff, "\0");
			strcpy(titw, "\0");
			return;
		}
		//if (clean != 2) gfx_swap_buffer(&g_gfx_ctxt);
		
		g_gfx_con.scale = 3;
		gfx_con_setpos(&g_gfx_con, 10, 5);
		gfx_con_setcol(&g_gfx_con, 0xFF008F39, 0xFF726F68, 0xFF191414);
		gfx_printf(&g_gfx_con, "%s\n",title);
		gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0, 0xFF191414);
		g_gfx_con.scale = 2;
		gfx_printf(&g_gfx_con, "%s\n",buff);
		gfx_printf(&g_gfx_con, "\n> %s\n",text);
		gfx_swap_buffer(&g_gfx_ctxt);
		if (clean > 100){msleep(clean);clean=0;}
		if (clean == 0)
		{
			if(strlen(title) > 0){
				strcpy(titw, "\0");
				strcpy(titw, title);
			}
			if (count > 38)
			{
				count = 0;
				strcpy(buff, "\0");
			}
			if(strlen(text) > 0){
				strcat(buff, text);
				strcat(buff, "\n");
				count++;
			}
		}
}

/*
void keys(){
	if (!sd_file_exists ("/bootloader/hekate_keys.ini"))
	{		
		printerCU("Save Mariko keys","CleanUP...",0);
		#include "protect/ofuscated.h"
		FIL fp;
		f_open(&fp, "/bootloader/hekate_keys.ini", FA_WRITE | FA_CREATE_ALWAYS);
		f_puts(ret, &fp);
		f_puts("\n", &fp);
		f_close(&fp);
	}
}
*/