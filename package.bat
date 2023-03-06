for /f "tokens=2 delims==" %%I in ('wmic os get localdatetime /format:list') do set datetime=%%I
set datetime=%datetime:~0,8%-%datetime:~8,6%

cd /d %~dp0
del /q /s dist\infzoom
mkdir dist\infzoom

copy /y LICENSE dist\infzoom\LICENSE.txt
copy /y README.md dist\infzoom

copy /y inf_launch_ext.ps1 dist\infzoom
copy /y inf_launch_ext.ps1 dist\infzoom\inf_launch_ext_kr.ps1

copy /y infzoom\infzoom.ini dist\infzoom
copy /y infzoom\Release\infzoom.exe dist\infzoom

pushd dist
7z a -tzip -r infzoom-%datetime%.zip infzoom
popd
