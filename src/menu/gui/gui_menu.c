
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

#define MINOR_VERSION 3
#define MAJOR_VERSION 0
#define REVI_VERSION 9
	char minorversion[3];
	char mayorversion[2];


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
        gfx_printf(&g_gfx_con, "ArgonNX v%d.%d-%d", MAJOR_VERSION, MINOR_VERSION,REVI_VERSION);
		
       //StarDust version

if (g_sd_mounted){

    char *str;
	void *buf;


	buf = sd_file_read("StarDust/StarDustV.txt");
	str = buf;
	minorversion[0] = str[2];
	minorversion[1] = str[3];
	mayorversion[0] = str[0];
	mayorversion[1] = 0;
}	
	gfx_con_setpos(&g_gfx_con, 1050, 10);
	gfx_printf(&g_gfx_con, "StarDust v%s.%s", mayorversion,minorversion);
		
		//battery
		u32 battPercent = 0;
		max17050_get_property(MAX17050_RepSOC, (int *)&battPercent);
		gfx_con_setpos(&g_gfx_con, 1050, 700);
        gfx_printf(&g_gfx_con, "Battery: -%d%-", (battPercent >> 8) & 0xFF, (battPercent & 0xFF));
    }

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
	while (gui_menu_update(menu))
    ;

	return 0;
}

int gui_menu_open2(gui_menu_t *menu)
{   
	sd_unmount();
    gfx_con_setcol(&g_gfx_con, 0xFF008F39, 0xFF726F68, 0xFF191414);
    /* 
     * Render and flush at first render because blocking input won't allow us 
     * flush buffers
     */
    gui_menu_render_menu(menu);
//u32 tog = 0;
		msleep(3000);
		display_backlight_brightness(1, 1000);
	while (gui_menu_update(menu))
	{
	display_backlight_brightness(1, 1000);
	gfx_con_setcol(&g_gfx_con, 0xFF008F39, 0xFF726F68, 0xFF191414);
/*		if (tog == 0){
		display_backlight_brightness(1, 1000);
		tog = 1;
		}else{
		display_backlight_brightness(100, 1000);
		tog = 0;
		}*/
		;
	}
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
