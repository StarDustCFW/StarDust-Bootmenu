
#include "menu/gui/gui_menu.h"
#include "menu/gui/gui_menu_pool.h"
#include "utils/touch.h"
#include "utils/btn.h"
#include "utils/fs_utils.h"
#include "utils/util.h"
#include "gfx/di.h"
#include "gfx/gfx.h"
#include "mem/heap.h"
#include "power/max17050.h"
#include "core/custom-gui.h"
#include <string.h>
#include "soc/t210.h"
#include "soc/fuse.h"

#define MINOR_VERSION 3
#define MAJOR_VERSION 0
#define REVI_VERSION 38
	char Sversion[4];

/* Render the menu */
static void gui_menu_render_menu(gui_menu_t*);
static void gui_menu_draw_background(gui_menu_t*);
static void gui_menu_draw_entries(gui_menu_t*);

/* Update menu after an input */
static int gui_menu_update(gui_menu_t*);

/* Handle input */
static int handle_touch_input(gui_menu_t*);

gui_menu_t *gui_menu_create(const char *title)
{
	gui_menu_t *menu = (gui_menu_t *)malloc(sizeof(gui_menu_t));
    menu->custom_gui = custom_gui_load();
	strcpy(menu->title, title);
	menu->next_entry = 0;
	menu->selected_index = 0;
    gui_menu_push_to_pool((void*)menu);
	return menu;
}


void gui_menu_append_entry(gui_menu_t *menu, gui_menu_entry_t *menu_entry)
{
	if (menu->next_entry == MAX_ENTRIES)
		return;

	menu->entries[menu->next_entry] = menu_entry;
	menu->next_entry++;
}

static void gui_menu_draw_background(gui_menu_t* menu)
{
    if(!render_custom_background(menu->custom_gui))
        gfx_clear_color(&g_gfx_ctxt, 0xFF191414);
    
    /* Render title */
    if (!render_custom_title(menu->custom_gui)) 
    {
        g_gfx_con.scale = 2;
        gfx_con_setpos(&g_gfx_con, 15, 10);
        gfx_printf(&g_gfx_con, "ArgonNX v%d.%d %k%d%k", MAJOR_VERSION, MINOR_VERSION,0xFF00FF22, REVI_VERSION ,0xFFCCCCCC);
    }
       //StarDust version
		char *str;
		if (g_sd_mounted){
			void *buf;
			buf = sd_file_read("StarDust/StarDustV.txt");
			str = buf;
			Sversion[0] = str[0];
			Sversion[1] = str[1];
			Sversion[2] = str[2];
			Sversion[3] = str[3];
		}	
	gfx_con_setpos(&g_gfx_con, 1050, 10);
	gfx_printf(&g_gfx_con, "StarDust v%s", Sversion);
		 

		
		//Fuses
		gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0xFFCCCCCC, 0xFF191414);
			u32 burntFuses = 0;
		for (u32 i = 0; i < 32; i++)
		{
			if ((fuse_read_odm(7) >> i) & 1)
				burntFuses++;
		}
		char *mindowngrade = "unknow";
		if(burntFuses == 1){mindowngrade = "1.0.0";}
		if(burntFuses == 2){mindowngrade = "2.0.0";}
		if(burntFuses == 3){mindowngrade = "3.0.0";}
		if(burntFuses == 4){mindowngrade = "3.0.1";}
		if(burntFuses == 5){mindowngrade = "4.0.0";}
		if(burntFuses == 6){mindowngrade = "5.0.0";}
		if(burntFuses == 7){mindowngrade = "6.0.0";}
		if(burntFuses == 8){mindowngrade = "6.2.0";}
		if(burntFuses == 9){mindowngrade = "7.0.0";}
		if(burntFuses == 10){mindowngrade = "8.1.0";}
		if(burntFuses == 11){mindowngrade = "9.0.0";}
		//burntFuses = burntFuses +10;//doble number test
		g_gfx_con.scale = 2;
		gfx_con_setpos(&g_gfx_con, 1035, 30);
		gfx_printf(&g_gfx_con,"F%k%d %kLimit%k %s%k\n\n", 0xFFea2f1e, burntFuses ,0xFFCCCCCC ,0xFF00FF22 ,mindowngrade ,0xFFCCCCCC);
		gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0, 0xFF191414);
/* 
Rojo  0xFFea2f1e
azul  0xFF331ad8
      0xFFF9F9F9
Blank 0xFFCCCCCC
verde 0xFF00FF22
*/	
}

static void gui_menu_render_menu(gui_menu_t* menu) 
{
    gui_menu_draw_background(menu);
    gui_menu_draw_entries(menu);
    gfx_swap_buffer(&g_gfx_ctxt);
}

static void gui_menu_draw_entries(gui_menu_t *menu)
{
    for (s16 i = 0; i < menu->next_entry; i++)
        gui_menu_render_entry(menu->entries[i]);
}


static int gui_menu_update(gui_menu_t *menu)
{
    u32 res = 0;

    gui_menu_draw_background(menu);
    gui_menu_draw_entries(menu);

    res = handle_touch_input(menu);

    gfx_swap_buffer(&g_gfx_ctxt);

    return res;
}

int gui_menu_open(gui_menu_t *menu)
{

    gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0, 0xFF191414);
    /* 
     * Render and flush at first render because blocking input won't allow us 
     * flush buffers
     */
    gui_menu_render_menu(menu);
sd_unmount();
display_backlight_brightness(100, 1000);

	while (gui_menu_update(menu))
    ;

	return 0;
}

int gui_menu_open3(gui_menu_t *menu)
{

    gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0, 0xFF191414);
    /* 
     * Render and flush at first render because blocking input won't allow us 
     * flush buffers
     */
//    gui_menu_render_menu(menu);
display_backlight_brightness(100, 1000);

	gui_menu_update(menu);

	return 0;
}

int gui_menu_open2(gui_menu_t *menu)
{   
//	sd_unmount();
    gfx_con_setcol(&g_gfx_con, 0xFF008F39, 0xFF726F68, 0xFF191414);
    /* 
     * Render and flush at first render because blocking input won't allow us 
     * flush buffers
     */
    if(!render_custom_background(menu->custom_gui))
    gfx_clear_color(&g_gfx_ctxt, 0xFF191414);
//    gui_menu_draw_entries(menu);
		if (sd_file_exists("StarDust/autobootecho.txt"))
		{
		g_gfx_con.scale = 3;
		gfx_con_setpos(&g_gfx_con, 1070, 10);
		gfx_con_setcol(&g_gfx_con, 0xFF008F39, 0xFF726F68, 0xFF191414);
		gfx_printf(&g_gfx_con, "AutoBoot\n");
		gfx_con_setpos(&g_gfx_con, 500, 10);
		gfx_printf(&g_gfx_con, "Vol +: BootMenu\n");
		gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0, 0xFF191414);
		}
    gfx_swap_buffer(&g_gfx_ctxt);
	return 0;
}

void gui_menu_destroy(gui_menu_t *menu)
{
	for (int i = 0; i < menu->next_entry; i++)
		gui_menu_entry_destroy(menu->entries[i]);
    custom_gui_end(menu->custom_gui);
	free(menu->entries);
	free(menu);
}


static int handle_touch_input(gui_menu_t *menu)
{
    gui_menu_entry_t *entry = NULL;
    touch_event_t event = touch_wait();
    
    /* After touch input check if any entry has ben tapped */
    for(u32 i = 0; i < menu->next_entry; i++)
    {
        entry = menu->entries[i];

        if (entry->handler != NULL 
            && is_rect_touched(&event, entry->x, entry->y, entry->width, entry->height))
        {
            if (entry->handler(entry->param) != 0)
                return 0;
        }
    }

    return 1;
}

