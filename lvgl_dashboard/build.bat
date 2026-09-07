@echo off
setlocal

set SDL2_PATH=F:/myproject/LVGL/SDL2/SDL2-2.30.8/x86_64-w64-mingw32

if not exist build mkdir build
cd build

cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%SDL2_PATH%" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
if errorlevel 1 goto :error

mingw32-make -j4
if errorlevel 1 goto :error

copy /Y "%SDL2_PATH%\bin\SDL2.dll" "bin\SDL2.dll" >nul

echo.
echo Build successful! Run: build\bin\lvgl_dashboard.exe
goto :eof

:error
echo.
echo Build failed.
exit /b 1
