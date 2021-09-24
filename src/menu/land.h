#include "menu/tools/fs_utils.h"
#include "menu/tools/touch.h"
#include "menu/gui/gui_argon_menu.h"
#include "menu/gui/gui_menu_pool.h"
#include "menu/tools/update.h"
#include "menu/tools/tools.h"

void takeoff(){
	/* Mount Sd card */
	if (sd_mount())
	{
		/* Cofigure touch input */
		touch_power_on();
		
		Update_SDT();
		clean_up();
			
		gui_init_argon_boot();
		//gui_init_argon_menu();
	}
	else
	{
		gfx_printf(&g_gfx_con, "No sd card found...\n");
	}
}
