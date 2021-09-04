#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "gfx/di.h"
#include "gfx/gfx.h"

#include "utils/util.h"
#include "utils/fs_utils.h"

char* type="*";
void lineHandler(char line[])
{
	/* If line is a comment return */
	if (line[0] == '#')
		return;

	/* If line is a message print it */
	if (line[0] == '>')
	{
		memmove(line, line + 1, strlen(line));
		printerCU(line, "", 0);
		return;
	}
	/* If line is a Title print it */
	if (line[0] == '-')
	{
		memmove(line, line + 1, strlen(line));
		printerCU("", line, 0);
		return;
	}

	/* If line is a ArchBit */
	if (line[0] == '+')
	{
		memmove(line, line + 1, strlen(line));
		if (HasArchBit(line)){
			printerCU(line, "", 0);
			Killflags(line);
		}
		return;
	}
	
	/* If line is a type */
	if (line[0] == '*')
	{
		memmove(line, line + 1, strlen(line));
		type=line;
		return;
	}
 
	printerCU(line,"",2);
	
	/* If line is a directory delete it all */
	if (line[strlen(line) - 2] == '/')
	{
		line[strlen(line) - 2] = 0;
		deleteall(line, type, "");
		return;
	}
	f_unlink(line);
}

void clean_up()
{
	printerCU("Clean old files", "CleanUP...", 0);

	FIL delet;
	f_open(&delet, "fixer.del", FA_READ);
	FILINFO stats;

	f_stat("fixer.del", &stats);
	__off_t size = stats.fsize;

	char buff[size];
	f_read(&delet, buff, size, &size);

	f_close(&delet);

	char *p = strtok(buff, "\n");
	while (p != NULL)
	{
		lineHandler(p);
		p = strtok(NULL, "\n");
	}
	
	//Pequeña correccion
	f_rename("/Backup/prodinfo.bin", "/prodinfo_sysnand.bin");

	//not pegascape units
	if (fusesB() > 5)
	{
		printerCU("Remove PegaScape Files", "CleanUP...", 0);
		deleteall("/pegascape", "*", "");
		f_unlink("/switch/fakenews-injector.nro");
	}

	//Fix old Emunand transfer
	if (sd_file_exists("sxos/eMMC/00") & sd_file_exists("sxos/eMMC/boot0") & sd_file_exists("sxos/eMMC/boot1"))
	{
		printerCU("Fix Emu Transfer", "CleanUP...", 0);
		f_mkdir("emuMMC");
		f_mkdir("emuMMC/EF00");
		f_rename("/sxos/eMMC", "/emuMMC/EF00/eMMC");
		f_rename("/Emutendo", "/emuMMC/EF00/Nintendo");
		if (sd_file_exists("emuMMC/EF00/eMMC/00") & sd_file_exists("emuMMC/EF00/eMMC/boot0") & sd_file_exists("emuMMC/EF00/eMMC/boot1"))
		{
			f_unlink("emummc/emummc.bak");
			f_rename("emummc/emummc.ini", "emummc/emummc.bak");
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

	f_unlink("/fixer.del");
	printerCU("", "", 1); //flush print
}

/*
						g_gfx_con.scale = 3;
						gfx_con_setpos(&g_gfx_con, 470, 250);
						gfx_printf(&g_gfx_con, "\n---%s---\n",line);
						gfx_printf(&g_gfx_con, "\n---%c---\n",line[strlen(line) - 2]);
						gfx_printf(&g_gfx_con, "\n---%s---\n",line);
						gfx_swap_buffer(&g_gfx_ctxt);
						msleep(25000);

*/