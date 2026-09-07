# LVGL 系统学习路线

> 适用对象：有 C 语言基础、做过嵌入式开发，从零系统掌握 LVGL。
> 预计周期：2~3 周（每天 2 小时）。

---

## 第一阶段：环境与核心概念（第 1~3 天）

### 1.1 环境搭建
- **PC 模拟器优先**：用 VSCode + CMake + SDL2 在电脑上跑 LVGL，不需要硬件，改完立刻看效果。
- 官方模拟器仓库：`lvgl/lv_port_pc_vscode`（VSCode 工程）或 `lvgl/lv_sim_eclipse_sdl`。
- 硬件移植放到最后学，先把 UI 逻辑吃透。

### 1.2 必须理解的 5 个核心概念
| 概念 | 通俗解释 |
|---|---|
| **对象（lv_obj_t）** | LVGL 里一切都是对象，按钮、标签、屏幕都是 `lv_obj_t`，类似面向对象里的基类 |
| **父子关系** | 每个对象都有父对象，子对象随父对象移动、裁剪，屏幕是根对象 |
| **样式（lv_style_t）** | 控制外观（颜色、边框、圆角、字体），一个样式可挂到多个对象 |
| **事件（lv_event_t）** | 点击、滑动、值变化都通过事件回调处理，类似中断服务函数 |
| **显示缓冲区** | LVGL 不直接操作显存，你给它一块 RAM 当画布，它画好后你再刷到屏幕上 |

### 1.3 最小运行骨架
```c
#include "lvgl.h"
#include "lvgl/demos/lv_demos.h"

int main(void) {
    lv_init();
    // 初始化显示驱动、输入驱动（模拟器已封装好）
    lv_demo_widgets();  // 跑官方示例
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }
}
```
**关键**：`lv_timer_handler()` 必须周期性调用，它负责刷新界面、处理事件、跑动画。

---

## 第二阶段：常用控件实战（第 4~8 天）

按使用频率排序，每个控件写一个小 demo：

### 2.1 基础控件
- `lv_btn` / `lv_label`：按钮 + 文字，最基础组合
- `lv_slider`：滑块，调音量、亮度
- `lv_switch`：开关
- `lv_checkbox`：复选框
- `lv_dropdown`：下拉列表
- `lv_roller`：滚轮选择器（车载常用，盲操作友好）

### 2.2 数据展示控件
- `lv_meter`：**仪表盘核心**，刻度、指针、弧形刻度，做车速表/转速表必学
- `lv_arc`：弧形进度条，可做油量、水温
- `lv_bar`：进度条
- `lv_chart`：折线图/柱状图，实时数据趋势
- `lv_spinbox`：数字调节框

### 2.3 容器与导航
- `lv_tabview`：标签页，多页面切换
- `lv_list`：列表
- `lv_menu`：菜单系统
- `lv_tileview`：瓦片视图，左右滑动翻页（手机风格）

### 2.4 输入控件
- `lv_keyboard`：虚拟键盘
- `lv_textarea`：文本输入框
- `lv_calendar`：日历

**学习方法**：每个控件先跑官方示例（`lv_demo_widgets` 里都有），再自己改参数、加事件回调，不要只看文档。

---

## 第三阶段：样式与布局（第 9~11 天）

### 3.1 样式系统
- 样式是**可复用**的，定义一次挂多个对象
- 状态样式：`LV_STATE_DEFAULT`、`LV_STATE_PRESSED`、`LV_STATE_CHECKED`、`LV_STATE_FOCUSED`
- 局部样式 vs 全局样式：`lv_obj_set_style_bg_color()` 直接改单个对象，`lv_style_t` + `lv_obj_add_style()` 批量复用
- 过渡动画：`lv_style_set_transition_time()` 让状态变化有动画

### 3.2 布局系统
- **Flex 布局**：`lv_obj_set_flex_flow()`，子对象自动排列（横排/竖排/换行），类似 CSS Flexbox
- **Grid 布局**：`lv_obj_set_grid_dsc_array()`，行列网格，适合复杂界面
- **绝对定位**：`lv_obj_set_pos()`，手动放坐标
- 推荐：能用 Flex/Grid 就不用绝对定位，自适应屏幕分辨率

### 3.3 字体与图片
- 字体：官方自带多种字号，也可用在线工具生成自定义字体（`lv_font_conv`）
- 图片：支持 PNG/JPG/SVG，用 `lv_img` 显示，大图片建议存在外部 Flash

---

## 第四阶段：事件与动画（第 12~14 天）

### 4.1 事件机制
- 注册事件：`lv_obj_add_event_cb(obj, callback, LV_EVENT_ALL, user_data)`
- 常用事件：`CLICKED`、`VALUE_CHANGED`、`PRESSING`、`SCROLL`、`FOCUSED`
- 事件里拿对象：`lv_event_get_target(e)`、`lv_event_get_code(e)`
- **关键原则**：事件回调里只做状态更新和 UI 刷新，不做耗时操作

### 4.2 动画系统
- `lv_anim_t`：通用动画，可对任意数值属性做过渡（位置、透明度、角度）
- 关键帧：`lv_anim_set_values()` 设置起始值和结束值
- 路径曲线：`lv_anim_path_ease_in_out` 等，控制加速减速
- 控件自带动画：`lv_obj_scroll_to_view()`、`lv_bar_set_value(anim=true)`

### 4.3 定时器
- `lv_timer_create(callback, period, user_data)`：周期性执行
- 用于数据刷新（比如每 100ms 读一次传感器更新仪表）
- 注意：定时器回调也在 `lv_timer_handler()` 里执行，别阻塞

---

## 第五阶段：进阶主题（第 15~18 天）

### 5.1 内存优化
- LVGL 用 `lv_mem` 自带内存管理，可配置堆大小（`LV_MEM_SIZE`）
- 图片用 `LV_IMG_CF_INDEXED` 索引色减少显存
- 显示缓冲区用双缓冲（`LV_DISP_RENDER_MODE_PARTIAL`）平衡性能和内存
- 动态创建的对象不用时 `lv_obj_del()` 释放

### 5.2 多页面与状态管理
- 用 `lv_scr_load()` 切换屏幕，旧屏幕不会自动销毁
- 推荐模式：每个页面一个创建函数 + 一个删除函数，手动管理生命周期
- 全局状态用一个结构体单例保存（类似你做 GameState 的思路）

### 5.3 输入设备
- 触摸：`lv_indev_t`，注册 `read_cb` 读取坐标
- 编码器/旋钮：`lv_indev_set_type(encoder)`，车载盲操神器
- 按键：物理按键映射到 LVGL 的 `LV_KEY_ENTER`、`LV_KEY_LEFT` 等

### 5.4 GPU 加速
- STM32 用 Chrom-ART（DMA2D）加速填充和混合
- ESP32-S3 用 PPA 外设
- NXP 用 PXP / VGLite
- LVGL 提供 `gpu_fill_cb` / `gpu_wait_cb` 回调接口，接上即可

---

## 第六阶段：硬件移植（第 19~21 天）

### 6.1 移植三件套
1. **显示驱动**：实现 `disp_flush()`，把 LVGL 画好的缓冲区数据写到屏幕（SPI/RGB/MIPI）
2. **输入驱动**：实现 `touchpad_read()`，读触摸坐标或按键
3. **时钟节拍**：`lv_tick_inc(1)` 每 1ms 调用一次，给 LVGL 提供时间基准

### 6.2 裸机 vs RTOS
- 裸机：主循环里调 `lv_timer_handler()`，定时器中断里调 `lv_tick_inc()`
- FreeRTOS：一个任务专门跑 `lv_timer_handler()`，互斥锁保护 LVGL 调用
- **坑点**：LVGL 不是线程安全的，多任务调用必须加锁

### 6.3 调试技巧
- 先用模拟器验证 UI 逻辑，再上硬件
- 硬件上先用单色/低分辨率屏跑通，再换高清屏
- `lv_obj_set_style_outline_color()` 给对象加边框，排查布局问题
- 内存不够时先关抗锯齿（`LV_ANTIALIAS`）和透明度

---

## 推荐资源

| 资源 | 用途 |
|---|---|
| docs.lvgl.io | 官方文档，最全最权威 |
| github.com/lvgl/lvgl | 源码，examples 目录里每个控件都有示例 |
| SquareLine Studio | 官方拖拽式 UI 设计器，生成 C 代码 |
| NXP GUI Guider | 免费替代，功能类似 |
| B站 "韦东山" / "正点原子" LVGL 教程 | 中文视频入门 |

---

## 学习检验标准

学完后你应该能：
1. 不看文档写出一个带按钮、滑块、仪表盘的界面
2. 理解事件回调机制，能自己封装自定义控件
3. 把 PC 模拟器项目移植到任意一块带屏的开发板
4. 看懂 LVGL 源码的对象系统和渲染流程
