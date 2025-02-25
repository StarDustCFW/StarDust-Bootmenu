/*
 * Copyright (c) 2018 naehrwert
 * Copyright (C) 2018 CTCaer
 * Copyright (C) 2018 Guillem96
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
#include "utils/dirlist.h"
#include "menu/tools/fs_utils.h"
#include "utils/util.h"
#include "gfx/di.h"
#include "mem/heap.h"
#include "gfx/gfx.h"
#include <string.h>
#include <stdlib.h>
#include "menu/tools/tools.h"

sdmmc_t g_sd_sdmmc;
sdmmc_storage_t g_sd_storage;
FATFS g_sd_fs;
bool g_sd_mounted;
bool sd_mount()
{
	if (g_sd_mounted)
		return true;
    u8 save = g_gfx_con.scale;
    g_gfx_con.scale = 2;
	gfx_con_setpos(&g_gfx_con, 1, 100);
	if (!sdmmc_storage_init_sd(&g_sd_storage, &g_sd_sdmmc, SDMMC_1, SDMMC_BUS_WIDTH_4, 11))
	{
		display_backlight_brightness(100, 1000);
        //gfx_printf(&g_gfx_con, "-\n", 0xFFFFDD00, 0xFFCCCCCC);
        gfx_printf(&g_gfx_con, "%kFallo al montar la SD.\nNo se detectar la SD .\nAsegurese este haciendo buen contacto y este puesta !%k\n", 0xFFFFDD00, 0xFFCCCCCC);
//		msleep(3000);
	}
	else
	{
		int res = 0;
		res = f_mount(&g_sd_fs, "", 1);
		if (res == FR_OK)
		{
			g_sd_mounted = 1;
			return true;
		}
		else
		{
        gfx_printf(&g_gfx_con, "-\n", 0xFFFFDD00, 0xFFCCCCCC);
        gfx_printf(&g_gfx_con, "%kFallo al montar la SD (FatFS Error %d).\nNo se encuentra una partición valida saca la sd\nAsegúrese que este en fat32 y haga buen contacto la sd en el slot %k\n", 0xFFFFDD00, res, 0xFFCCCCCC);
		}
	}
    g_gfx_con.scale = save;

	return false;
}

void sd_unmount()
{
	if (g_sd_mounted)
	{
		f_mount(NULL, "", 1);
		sdmmc_storage_end(&g_sd_storage);
		g_sd_mounted = false;
	}
}

void *sd_file_read(const char *path)
{
	FIL fp;
	if (f_open(&fp, path, FA_READ) != FR_OK)
		return NULL;

	u32 size = f_size(&fp);
	void *buf = malloc(size);

	u8 *ptr = buf;
	while (size > 0)
	{
		u32 rsize = MIN(size, 512 * 512);
		if (f_read(&fp, ptr, rsize, NULL) != FR_OK)
		{
			free(buf);
			return NULL;
		}

		ptr += rsize;
		size -= rsize;
	}

	f_close(&fp);

	return buf;
}

char *read_file_string(char *path)
{
	FIL file;
	f_open(&file, path, FA_READ);
	FILINFO stats;

	f_stat(path, &stats);
	unsigned int size = stats.fsize;

	char *buff = malloc(size + 1);
	buff[size] = '\0';
	f_read(&file, buff, size, &size);

	f_close(&file);
	return buff;
}

int sd_save_to_file(void *buf, u32 size, const char *filename)
{
	FIL fp;
	u32 res = 0;
	res = f_open(&fp, filename, FA_CREATE_ALWAYS | FA_WRITE);
	if (res)
	{
        gfx_printf(&g_gfx_con, "%kError (%d) creating file\n%s.\n%k\n", 0xFFFFDD00, res, filename, 0xFFCCCCCC);
		return 1;
	}

	f_sync(&fp);
	f_write(&fp, buf, size, NULL);
	f_close(&fp);

	return 0;
}

bool sd_file_exists(const char* filename)
{
    FRESULT fr;
    FILINFO fno;
    fr = f_stat(filename, &fno);
    switch (fr) {
    case FR_OK:
	return true;
        break;

    case FR_NO_FILE:
        return false;
        break;

    default:
        return false;
    }
}

bool sd_file_size(char *path)
{
	FIL fp;
	if (f_open(&fp, path, FA_READ) != FR_OK)
		return 0;

	u32 size = f_size(&fp);
	f_close(&fp);
	return size;
}

void copyfile(const char* source, const char* target)
{
    FIL fp;
    if (f_open(&fp, source, FA_READ) != FR_OK)
    {
        gfx_printf(&g_gfx_con, "file %s mising\n",source);
        //gfx_swap_buffer(&g_gfx_ctxt);
        //msleep(3000);
	}else{
        u32 size = f_size(&fp);
        f_close(&fp);
        sd_save_to_file(sd_file_read(source),size,target);
	}
}

void copyfileparam(char* param, char* source, char* target)
{
	char* path = (char*)malloc(256);
    strcpy(path, param);
    strcat(path, "/");
    strcat(path, source);
/*		
		g_gfx_con.scale = 2;
        gfx_con_setpos(&g_gfx_con, 15, 50);
		gfx_printf(&g_gfx_con, "--------------\n",path);
		gfx_printf(&g_gfx_con, "copy %s %s\n",path ,target);
	    gfx_swap_buffer(&g_gfx_ctxt);
*/		
    FIL fp;
    if (f_open(&fp, path, FA_READ) != FR_OK)
    {
		gfx_printf(&g_gfx_con, "file %s mising\n",path);
	    gfx_swap_buffer(&g_gfx_ctxt);
		msleep(3000);
    }else{
        u32 size = f_size(&fp);
        f_close(&fp);
        sd_save_to_file(sd_file_read(path),size,target);
	}
}

void copy_folder(char* sourse_folder, char* dest_folder)
{
    if (!sd_file_exists(sourse_folder)) return;
    char* Files = listfil(sourse_folder, "*", true);
    u32 i = 0;
    while(Files[i * 256])
    {
        char* source_file = (char*)malloc(256);
        if(strlen(&Files[i * 256]) <= 100){	
            strcpy(source_file, sourse_folder);
            strcat(source_file, "/");
            strcat(source_file, &Files[i * 256]);
        }
        char* dest_file = (char*)malloc(256);
        if(strlen(&Files[i * 256]) <= 100){		
            strcpy(dest_file, dest_folder);
            strcat(dest_file, "/");
            strcat(dest_file, &Files[i * 256]);
            gfx_con_setpos(&g_gfx_con, 10, 90);
            gfx_printf(&g_gfx_con, "\ncopy %s to %s\n",source_file,dest_file);
            gfx_swap_buffer(&g_gfx_ctxt);
            copyfile(source_file,dest_file);//action
        }
        i++;
    }
}

//move code
void moverall(char* directory, char* destdir, char* filet, char* coment)
{
if (!sd_file_exists(directory)) return;
char* files = listfil(directory, filet, true);
char* folder = listfol(directory, "*", true);
f_mkdir(destdir);
    u32 i = 0;
    while(files[i * 256])
    {
char* sourcefile = (char*)malloc(256);
			if(strlen(&files[i * 256]) <= 100){			
			strcpy(sourcefile, "\0");
			strcat(sourcefile, directory);
			strcat(sourcefile, "/");
			strcat(sourcefile, &files[i * 256]);
			
char* destfile = (char*)malloc(256);
			strcpy(destfile, "\0");
			strcat(destfile, destdir);
			strcat(destfile, "/");
			strcat(destfile, &files[i * 256]);
			if(strlen(coment) > 0){
				printerCU(destfile,coment,0);
			}
			f_unlink(destfile);
			f_rename(sourcefile,destfile);
			}
	i++;
    }

    u32 r = 0;
    while(folder[r * 256])
    {
char* folderpath = (char*)malloc(256);
			if((strlen(&folder[r * 256]) <= 100) & (strlen(&folder[r * 256]) > 0)){			
			strcpy(folderpath, "\0");
			strcat(folderpath, directory);
			strcat(folderpath, "/");
			strcat(folderpath, &folder[r * 256]);
//			deleteall(folderpath, "*","");

char* folderdest = (char*)malloc(256);
			strcpy(folderdest, "\0");
			strcat(folderdest, destdir);
			strcat(folderdest, "/");
			strcat(folderdest, &folder[r * 256]);
//			deleteall(folderpath, "*","");
			moverall(folderpath, folderdest, filet, coment);
			}
	r++;
    }
}

//move
void f_move (char* file1, char* file2)
{
	if (sd_file_exists(file1)){
		if (sd_file_exists(file2)){
			f_unlink(file2);			
		}
		f_rename(file1,file2);
	}
}

//copy code
void copyarall(char* directory, char* destdir, char* filet, char* coment)
{
if (!sd_file_exists(directory)) return;
char* files = listfil(directory, filet, true);
char* folder = listfol(directory, "*", true);
f_mkdir(destdir);
    u32 i = 0;
    while(files[i * 256])
    {
char* sourcefile = (char*)malloc(256);
			if(strlen(&files[i * 256]) <= 100){			
			strcpy(sourcefile, "\0");
			strcat(sourcefile, directory);
			strcat(sourcefile, "/");
			strcat(sourcefile, &files[i * 256]);
			
char* destfile = (char*)malloc(256);
			strcpy(destfile, "\0");
			strcat(destfile, destdir);
			strcat(destfile, "/");
			strcat(destfile, &files[i * 256]);
			if(strlen(coment) > 0){
				printerCU(destfile,coment,0);
			}
			f_unlink(destfile);
			copyfile(sourcefile,destfile);
			}
	i++;
    }

    u32 r = 0;
    while(folder[r * 256])
    {
char* folderpath = (char*)malloc(256);
			if((strlen(&folder[r * 256]) <= 100) & (strlen(&folder[r * 256]) > 0)){			
			strcpy(folderpath, "\0");
			strcat(folderpath, directory);
			strcat(folderpath, "/");
			strcat(folderpath, &folder[r * 256]);
//			deleteall(folderpath, "*","");

char* folderdest = (char*)malloc(256);
			strcpy(folderdest, "\0");
			strcat(folderdest, destdir);
			strcat(folderdest, "/");
			strcat(folderdest, &folder[r * 256]);
//			deleteall(folderpath, "*","");
			copyarall(folderpath, folderdest, filet, coment);
			}
	r++;
    }
}

//folder delete use with care
void deleteall(char* directory, char* filet, char* coment)
{
if (!sd_file_exists(directory)) return;
char* files = listfil(directory, filet, true);
char* folder = listfol(directory, "*", true);
    u32 i = 0;
    while(files[i * 256])
    {
char* destfile = (char*)malloc(256);
			if(strlen(&files[i * 256]) <= 100){			
			strcpy(destfile, "\0");
			strcat(destfile, directory);
			strcat(destfile, "/");
			strcat(destfile, &files[i * 256]);
			if(strlen(coment) > 0){
				printerCU(destfile,coment,2);
			} else {
				printerCU(destfile,"",2);
			}
				
			f_unlink(destfile);
			}
	i++;
    }

    u32 r = 0;
    while(folder[r * 256])
    {
char* folderpath = (char*)malloc(256);
			if((strlen(&folder[r * 256]) <= 100) & (strlen(&folder[r * 256]) > 0)){			
			strcpy(folderpath, "\0");
			strcat(folderpath, directory);
			strcat(folderpath, "/");
			strcat(folderpath, &folder[r * 256]);
			deleteall(folderpath, filet,coment);
			}
	r++;
    }
f_unlink(directory);
}

bool HasArchBit(const char *directory)
{
    FRESULT fr;
    FILINFO fno;
    fr = f_stat(directory, &fno);
    if  (fr == FR_OK)
	{
		if (fno.fattrib & AM_ARC)
		{
			return true;
		}
	}	
	return false;
}

void Killflags(char *directory)
{
	gfx_con_setpos(&g_gfx_con, 1, 10);
	printerCU(directory,"",2);
	f_chmod(directory, 0, AM_RDO | AM_ARC);
    if (strstr(directory, "//") != NULL){
        return;
    }
    
    char* folder = listfol(directory, "*", true);
    u32 r = 0;
    while(folder[r * 256])
    {
		char* folderpath = (char*)malloc(256);
			if((strlen(&folder[r * 256]) <= 200) & (strlen(&folder[r * 256]) > 1))
			{		
				strcpy(folderpath, directory);
				strcat(folderpath, "/");
				strcat(folderpath, &folder[r * 256]);
                if (strstr(directory, "//") != NULL) return;
				Killflags(folderpath);
			}
	r++;
    }
}
