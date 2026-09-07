@echo off
echo ========================================
echo   LVGL ESP32-S3 Project Setup
echo ========================================
echo.

cd /d "%~dp0"

if not exist "components\lvgl\lv_conf.h" (
    if not exist "components\lvgl" (
        echo [1/3] Cloning LVGL v8.3 into components\lvgl ...
        git clone --depth 1 --branch release/v8.3 https://github.com/lvgl/lvgl.git components\lvgl
        if errorlevel 1 (
            echo ERROR: Failed to clone LVGL. Check git and internet.
            pause
            exit /b 1
        )
    ) else (
        echo [1/3] components\lvgl already exists.
    )

    echo.
    echo [2/3] Copying lv_conf.h to components\lvgl\ ...
    copy /Y main\lv_conf.h components\lvgl\lv_conf.h
    if errorlevel 1 (
        echo ERROR: Failed to copy lv_conf.h!
        pause
        exit /b 1
    )
    echo   Success.
) else (
    echo [1/2] components\lvgl\lv_conf.h already exists.
    echo [2/2] Updating lv_conf.h from main\lv_conf.h ...
    copy /Y main\lv_conf.h components\lvgl\lv_conf.h >nul
)

echo.
echo [3/3] Verifying ...
if exist "components\lvgl\lv_conf.h" (
    echo   lv_conf.h: OK
) else (
    echo   lv_conf.h: MISSING!
)
if exist "components\lvgl\CMakeLists.txt" (
    echo   LVGL source: OK
) else (
    echo   LVGL source: MISSING!
)

echo.
echo ========================================
echo   Setup complete!
echo ========================================
echo.
echo IMPORTANT: If you already ran a build, clean first:
echo   idf.py fullclean
echo.
echo Then build:
echo   idf.py set-target esp32s3
echo   idf.py build
echo.
pause
