@echo off
set PATH=c:\devkitPro\msys2\usr\bin;%PATH%

del build\gui_menu.o
make -j7
echo completado
echo %cd%
move /y output\payload.bin .\
copy payload.bin "C:\Users\Kronos2308\Documents\GitHub\StarDustCFWPack\SD_card_root\StarDust.bin"
copy payload.bin "C:\Users\Kronos2308\Documents\GitHub\StarDustCFWPack\SD_card_root\atmosphere\reboot_payload.bin"
copy payload.bin "E:\payload.bin"
copy payload.bin "E:\atmosphere\reboot_payload.bin"
%systemroot%\system32\timeout.exe 10

