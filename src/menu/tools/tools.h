#include "utils/types.h"


__attribute__((noreturn)) void wait_for_button_and_reboot(void);

//Replace a pattern of string for another string
char *str_replace(char *orig, char *rep, char *with);
//tools
void strrep(char *orig, char *rep, char *with);
void SDStrap();
void BootStrapNX();
void printerCU(char *text,const char *title,int clean);
u32 fusesB();
char *fusesM();
