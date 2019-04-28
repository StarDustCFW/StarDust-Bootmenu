@echo off
make
echo completado
echo %cd%
copy %cd%\output\payload.bin %cd%\output\reboot_payload.bin
%systemroot%\system32\timeout.exe 55

