# LVGL 车载仪表盘（LVGL 8.3.11 LTS）

基于 LVGL 8.3.11 LTS 的 PC 模拟器项目，实现双表盘车载仪表盘。
选用 8.3 LTS 而非 9.x：lv_meter 稳定可用、API 成熟、车载领域实际用得最多、无 API 变动问题。

## 功能特性

- Dashboard：车速表（0-240）、转速表（0-8000 RPM）、档位、油量、水温
- Control：档位滚轮、亮度滑块、转向灯/大灯按钮
- 状态栏：实时时钟、转向灯 LED、大灯 LED
- 数据模拟：车速自动加减速，转速联动
- 深色车载主题

## 项目结构

```
lvgl_dashboard/
├── CMakeLists.txt
├── lv_conf.h
├── main.c              # 入口 + 主循环
├── sdl_port.c/h        # SDL 显示/输入驱动
├── build.bat           # 一键构建
├── app/                # 数据层（VehicleState 单例）
├── ui/                 # 界面层
│   ├── ui.c            # 页面管理 + 定时器
│   ├── dashboard.c     # 双 lv_meter 仪表盘
│   ├── statusbar.c     # 状态栏
│   └── control_page.c  # 控制面板
└── lvgl/               # LVGL 8.3.11 源码
```

## 构建运行

```bash
build.bat
build\bin\lvgl_dashboard.exe
```

或 VSCode 中 Ctrl+Shift+B 构建，F5 调试运行，Ctrl+F5 直接运行。

## 学习要点

| 知识点 | 文件 |
|---|---|
| LVGL 初始化 + 显示/输入驱动注册 | main.c, sdl_port.c |
| lv_meter 仪表盘 | dashboard.c |
| lv_bar 进度条 | dashboard.c |
| lv_roller 滚轮 | control_page.c |
| lv_slider 滑块 | control_page.c |
| lv_btn + 事件回调 | control_page.c |
| lv_led 指示灯 | statusbar.c |
| lv_tabview 多页面 | ui.c |
| 定时器数据刷新 | ui.c |
| 数据/UI 分离架构 | app/ + ui/ |
