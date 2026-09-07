# LVGL 车载仪表盘 — 从原理到移植全流程

ESP32-S3 + ST7789V 车载仪表盘项目，包含 PC SDL 模拟器和 ESP32 嵌入式两个版本，完整覆盖 LVGL 内核原理、显示驱动移植、UI 实战和踩坑复盘。

## 功能特性

- **270度速度表**：lv_arc 弧形速度表，0-180km/h，角度映射公式
- **五档切换**：P/R/N/D/S 档位高亮切换
- **实时数据**：油量、水温、续航里程、瞬时油耗
- **多页面**：Dashboard 仪表盘、Control 控制面板、Media 媒体、Nav 导航
- **状态栏**：时间、蓝牙、WiFi、电池图标、转向灯/大灯 LED
- **数据驱动动画**：不同频率更新不同数据，速度200ms、档位3s、油量2s
- **中英文切换**：内置多语言支持

## 两个版本

| 版本 | 目录 | 平台 | 分辨率 | 用途 |
|------|------|------|--------|------|
| PC 模拟器 | `lvgl_dashboard/` | Windows + SDL2 | 1024x600 | UI 开发调试，无需硬件 |
| ESP32 嵌入式 | `lvgl_esp32/` | ESP32-S3 + ST7789V | 320x240 横屏 | 硬件实机运行 |

## 硬件连接（ESP32-S3 + ST7789V）

| ST7789V 引脚 | ESP32-S3 GPIO | 说明 |
|---------------|---------------|------|
| VCC | 3.3V | 电源，必须3.3V，5V会烧屏 |
| GND | GND | 地 |
| SCL | GPIO9 | SPI 时钟 |
| SDA | GPIO10 | SPI 数据（MOSI） |
| RESET | GPIO11 | 复位 |
| DC | GPIO13 | 数据/命令选择 |
| CS | GPIO14 | 片选 |
| BL | GPIO8 | 背光控制 |

SPI 时钟 40MHz，屏幕只写不读（MISO 不接）。

## 快速开始

### PC 模拟器

```bash
cd lvgl_dashboard
build.bat
build\bin\lvgl_dashboard.exe
```

需要：CMake、MinGW、SDL2。VSCode 中 Ctrl+Shift+B 构建，F5 调试。

### ESP32 嵌入式

```bash
cd lvgl_esp32
idf.py set-target esp32s3
idf.py build
idf.py -p COMx flash monitor
```

需要：ESP-IDF v5.5.5。

### 中文字体生成（PC模拟器必需）

PC 模拟器版本使用自定义中文字体（`app/font_cn_*.c`），由于字体文件较大（70-200KB），未纳入仓库，需自行生成：

```bash
npm install -g lv_font_conv

lv_font_conv --font C:/Windows/Fonts/simhei.ttf --size 12 --bpp 4 --no-compress \
  --symbols "中主乐五亮仪体余侧保光入公关冲出到制剩压变口古右号合向园围在地均城境增复外夜大天媒子室导屏左市幕平并度开心态总成持控播放文时月松树梦榆橡正氛水油沿波海涡温灯状电的盘目直瞬程空第箱线续耗脉航虹行街表言计语调赛路车转轮轻辆达进速道都里量队际限霓预风驰驶高" \
  --format lvgl --output lvgl_dashboard/app/font_cn_12.c
```

对 14/16/20/24 字号重复上述命令，修改 `--size` 和 `--output` 参数即可。

## 项目结构

```
LVGL-Car-Dashboard/
├── README.md                    # 本文件
├── lvgl学习路线.md               # LVGL 系统学习路线（2-3周）
├── lvgl_dashboard/              # PC SDL 模拟器版本
│   ├── CMakeLists.txt
│   ├── lv_conf.h                # LVGL 配置
│   ├── main.c                   # 入口 + 主循环
│   ├── sdl_port.c/h             # SDL 显示/输入驱动
│   ├── build.bat                # 一键构建
│   ├── app/                     # 数据层（VehicleState 单例 + 多语言 + 中文字体）
│   │   ├── app.c/h              # 应用状态管理
│   │   ├── lang.c/h             # 中英文切换
│   │   └── font_cn_*.c          # 中文字体（需自行生成，见上方说明）
│   └── ui/                      # 界面层
│       ├── ui.c/h               # UI 总入口 + TabView
│       ├── dashboard.c          # 仪表盘（速度表/转速表/档位/卡片）
│       ├── statusbar.c          # 顶部状态栏
│       ├── control_page.c       # 控制面板（灯光/空调/亮度）
│       ├── media_page.c         # 媒体播放器
│       └── nav_page.c           # 导航页面
└── lvgl_esp32/                  # ESP32-S3 嵌入式版本
    ├── CMakeLists.txt
    ├── sdkconfig.defaults
    ├── setup.bat                # 一键设置（克隆LVGL + 复制配置）
    ├── main/
    │   ├── main.c               # 入口
    │   ├── lvgl_port.c/h        # LVGL 移植层（四步走）
    │   ├── lv_conf.h            # ESP32 版 LVGL 配置
    │   ├── st7789v.c/h          # ST7789V SPI 驱动
    │   └── ui/                   # 嵌入式版 UI（ui_mini.c）
    └── components/lvgl/          # LVGL 源码（setup.bat 自动克隆）
```

## 核心踩坑总结

| 问题 | 根因 | 解决方案 |
|------|------|----------|
| 颜色反相（蓝变黄、红变青） | RGB565 字节序，ESP32 小端先发低字节 | 手动拆字节高字节先发，或开 LV_COLOR_16_SWAP |
| 界面不动（初始画面亮但数据不更新） | tick 不前进，LVGL 认为时间停止 | 确保 esp_timer 初始化，或用 lv_tick_inc 手动推进 |
| 花屏 | 显示缓冲区行数不够 | draw_buf 至少 40 行，使用双缓冲 |
| 闪烁 | 单缓冲 | 申请两个缓冲区交替使用 |
| 坐标错位 | MADCTL 没设对 | 横屏设 0xC0 或 0xA0，根据屏幕方向试 |
| 内存泄漏 | 动态对象没删 | 用完 lv_obj_del() 释放 |
| 编译报错字体未声明 | lv_conf.h 中未启用对应字号 | 开启 LV_FONT_MONTSERRAT_XX = 1 |
| lv_color_hex 非常量初始化 | 全局数组用函数初始化 | 移到函数内初始化，或用 LV_COLOR_MAKE 宏 |

## LVGL 核心原理

1. **三层架构**：应用层（你的代码）→ 核心层（对象/样式/事件/渲染，纯C硬件无关）→ 驱动接口层（移植部分）
2. **渲染管线**：标记脏矩形 → 渲染到缓冲区 → flush 到屏幕 → 通知完成
3. **事件循环**：lv_timer_handler 推进定时器、执行回调、处理输入事件
4. **四个核心概念**：对象树、样式、事件、布局

## 学习资源

- [LVGL 官方文档](https://docs.lvgl.io)
- [LVGL 源码](https://github.com/lvgl/lvgl)
- 本仓库 `lvgl学习路线.md` — 2-3周系统学习路线
- 配套教学视频（B站/抖音搜索"LVGL车载仪表盘"）

## 许可证

MIT License
