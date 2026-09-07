cmake_minimum_required(VERSION 3.15)
project(lvgl_dashboard C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra)
endif()

set(LV_CONF_PATH ${CMAKE_SOURCE_DIR}/lv_conf.h CACHE STRING "" FORCE)
add_subdirectory(lvgl)

find_package(SDL2 REQUIRED)

add_executable(lvgl_dashboard
    main.c
    sdl_port.c
    app/app.c
    app/lang.c
    app/font_cn_12.c
    app/font_cn_14.c
    app/font_cn_16.c
    app/font_cn_20.c
    app/font_cn_24.c
    ui/ui.c
    ui/dashboard.c
    ui/statusbar.c
    ui/control_page.c
    ui/media_page.c
    ui/nav_page.c
)

target_include_directories(lvgl_dashboard PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/app
    ${CMAKE_SOURCE_DIR}/ui
)

target_link_libraries(lvgl_dashboard PRIVATE
    lvgl
    SDL2::SDL2main
    SDL2::SDL2
)
