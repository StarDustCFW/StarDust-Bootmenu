@echo off
del build\Bootmenu\gui_menu.o
make
echo completado
echo %cd%
copy payload.bin "C:\Users\Administrador\Documents\GitHub\StarDustCFWPack\SD_card_root\atmosphere\reboot_payload.bin"
copy payload.bin "C:\Users\Administrador\Documents\GitHub\StarDustCFWPack\SD_card_root\payload.bin"
%systemroot%\system32\timeout.exe 10

