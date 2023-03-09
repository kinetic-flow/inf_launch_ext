cd /d %~dp0
copy /y infzoom\infzoom.ini infzoom\Debug
copy /y infzoom\infzoom.ini infzoom\Release

set outdir="G:\Games\beatmania IIDX INFINITAS"
copy /y inf_launch_ext.ps1 %outdir%
copy /y infzoom\infzoom.ini %outdir%
copy /y infzoom\Release\infzoom.exe %outdir%