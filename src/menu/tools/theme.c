#include "utils/fs_utils.h"
#include "utils/util.h"
#include <stdio.h>
#include <string.h>

char *them = "skins/xbox";

void loadTheme()
{
	if (!sd_file_exists("/StarDust/theme"))
		return;
	them = read_file_string("/StarDust/theme");
}

void saveTheme(char *param)
{
	them = param;
	sd_save_to_file(param, strlen(param), "/StarDust/theme");
}

void *theme(char *path)
{
	char root[] = "/StarDust/";
	char buff[strlen(path) + strlen(them) + strlen(root)];
	memset(buff, 0, sizeof buff);
	strcat(buff, root);
	strcat(buff, them);
	strcat(buff, path);
	if (sd_file_exists(buff))
		return sd_file_read(buff);
	memset(buff, 0, sizeof buff);
	strcpy(buff, root);
	strcat(buff, "skins/xbox/");
	strcat(buff, path);
	return sd_file_read(buff);
}