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
#ifndef _GUI_ARGON_MENU_H_
#define _GUI_ARGON_MENU_H_
#include "menu/gui/gui_menu.h"
#include "utils/types.h"

/* Init needed menus for ArgonNX */
void gui_init_argon_menu(void);
void gui_init_argon_boot(void);

int AThemes_list(gui_menu_t *menu, u32 gridX, u32 gridY);
void serv_display(gui_menu_t *menu, char *titleid, char *name);
static int tool_extr_rSD(void *param);
static int tool_reboot_rcm(void *param);
static int tool_power_off(void *param);
static int tool_menu_rem(void *param);
static int tool_theme(char *param);
static int tool_emu(u32 param);
static int tool_Menus(u32 param);
void tool_servises_on(char *title);
void tool_servises_off(char *title);
void tool_Themes_on(char *cfw);
void tool_Themes_off(char *cfw);
int memloader(u32 fil);
int Incognito(char *order);
int Autoboot(u32 fil);
int uLaunch(u32 fil);
void HBhide(char *folder);
void serv_CFW(int cfw);
int bat_show(u32 percent);
void Ajbrillo(u32 tipo);
void hekateOFW(u32 tipo);
void medislay(char *flags);
int static_menu_elements(gui_menu_t *menu);
void pre_load_menus(int menus, bool StarUp);

#endif