# LVGL ESP32-S3 车载仪表盘

基于 LVGL v8.3 + ESP32-S3 + ST7789V SPI 显示屏的小型车载仪表盘项目。

## 硬件连接

| ST7789V 引脚 | ESP32-S3 GPIO | 说明 |
|---|---|---|
| VCC | 3.3V | 电源 |
| GND | GND | 地 |
| SCL | GPIO9 | SPI SCK |
| SDA | GPIO10 | SPI MOSI |
| RESET | GPIO11 | 复位 |
| DC | GPIO13 | 数据/命令选择 |
| CS | GPIO14 | 片选 |
| BL | GPIO8 | 背光控制 |

> SPI 主机：FSPI (SPI2_HOST)，时钟 40MHz，模式 0
> 分辨率：240x320（竖屏），颜色格式 RGB565

## 项目结构

```
lvgl_esp32/
├── CMakeLists.txt          # 项目根 CMake
├── sdkconfig.defaults      # 默认编译配置
├── setup.bat               # Windows 一键设置脚本
├── main/
│   ├── CMakeLists.txt      # main 组件配置
│   ├── main.c              # 程序入口
│   ├── lv_conf.h           # LVGL 配置
│   ├── st7789v.h           # ST7789V 驱动头文件
│   ├── st7789v.c           # ST7789V SPI 驱动实现
│   ├── lvgl_port.h         # LVGL 移植层头文件
│   ├── lvgl_port.c         # LVGL 显示接口 + FreeRTOS 任务
│   └── ui/
│       ├── ui_mini.h       # 小屏幕 UI 头文件
│       └── ui_mini.c       # 240x320 仪表盘 UI 实现
└── components/
    └── lvgl/               # LVGL 源码（setup.bat 自动克隆）
```

## 快速开始

### 方法一：一键设置（推荐 Windows）

```cmd
cd lvgl_esp32
setup.bat
```

脚本会自动：
1. 克隆 LVGL v8.3 到 `components/lvgl/`
2. 复制 `lv_conf.h` 到 LVGL 目录

### 方法二：手动设置

```cmd
cd lvgl_esp32
git clone --depth 1 --branch release/v8.3 https://github.com/lvgl/lvgl.git components\lvgl
copy main\lv_conf.h components\lvgl\lv_conf.h
```

## 编译烧录

### VS Code + ESP-IDF 插件

1. 用 VS Code 打开 `lvgl_esp32` 文件夹
2. 底部状态栏点击 `ESP-IDF: Set Espressif device target` → 选择 `ESP32-S3`
3. 点击 `ESP-IDF: Build your project`（或按 `Ctrl+E B`）
4. 连接开发板，点击 `ESP-IDF: Select port to use` 选择串口
5. 点击 `ESP-IDF: Flash your project`（或按 `Ctrl+E F`）
6. 点击 `ESP-IDF: Monitor your device` 查看串口输出

### 命令行

```cmd
idf.py set-target esp32s3
idf.py build
idf.py -p COMx flash monitor
```

（将 `COMx` 替换为实际串口号，如 `COM3`）

## UI 功能

240x320 竖屏仪表盘布局：

- **顶部状态栏**（28px）：时间、蓝牙、WiFi、电量图标
- **中心区域**：档位字母（P/R/N/D/S，彩色）+ 车速数字 + km/h 单位
- **信息卡片**（3个）：油量百分比、水温、续航里程
- **底部栏**（24px）：总里程、平均油耗

数据每 200ms 自动更新模拟（车速波动、档位切换、油量变化），方便验证显示效果。

## 内存占用

| 项目 | 大小 |
|---|---|
| 显示缓冲区 x2 | 38.4 KB (240x40x2x2) |
| LVGL 内存池 | 64 KB |
| FreeRTOS 任务栈 | 4 KB |
| 合计（内部 SRAM） | ~110 KB |

ESP32-S3 内部 SRAM 512KB，无需 PSRAM。

## 常见问题

### 1. 编译报错找不到 lv_conf.h

确保已执行 `setup.bat` 或手动复制了 `lv_conf.h` 到 `components/lvgl/` 目录。

### 2. 屏幕白屏/花屏

- 检查接线是否正确（尤其 SCL/SDA/DC/CS）
- 确认 VCC 接 3.3V 而非 5V
- 检查 SPI 引脚是否与其他功能冲突
- 尝试降低 SPI 时钟：修改 `st7789v.h` 中 `ST7789V_SPI_FREQ` 为 `20000000`

### 3. 屏幕显示偏移

部分 ST7789V 模块有行列偏移。修改 `st7789v.c` 中的 `X_OFFSET` 和 `Y_OFFSET`（常见值 0/20/40）。

### 4. 屏幕方向不对

修改 `st7789v.c` 初始化命令中 `0x36`（MADCTL）的值：
- `0x00`：竖屏 0°
- `0x60`：横屏 90°
- `0xC0`：竖屏 180°
- `0xA0`：横屏 270°

同时修改 `st7789v.h` 中的 `ST7789V_WIDTH` 和 `ST7789V_HEIGHT`。

### 5. 程序运行后无反应

- 打开串口监视器（115200 波特率）查看启动日志
- 确认 `ST7789V initialized` 和 `LVGL initialized` 日志出现
- 检查背光是否亮起（GPIO8 输出高电平）

## 扩展方向

- 添加触摸驱动（I2C/SPI 触摸屏）
- 添加物理按钮输入（档位切换、页面切换）
- 接入真实车速/转速传感器数据（CAN 总线 / ADC）
- 多页面切换（仪表盘、媒体、导航、设置）
- 启用 PSRAM 提升缓冲和帧率
- 中文字体支持（lv_font_conv 生成子集字体）
