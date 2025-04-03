#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"

#include "lv_100ask_lesson_demos/lv_100ask_lesson_demos.h"

#include "include/lv_port/lv_port_indev.h"
#include "log/tapah_log.h"
#include "include/rkmedia/rkmedia.h"

#if LV_USE_WAYLAND
#include "backends/interface.h"
#endif

uint16_t window_width;
uint16_t window_height;
bool fullscreen;
bool maximize;

void* key_monitor_thread(void* arg);
static void configure_simulator(int argc, char **argv);

static const char *getenv_default(const char *name, const char *dflt)
{
    return getenv(name) ? : dflt;
}

#if LV_USE_EVDEV
static void lv_linux_init_input_pointer(lv_display_t *disp)
{
    /* Enables a pointer (touchscreen/mouse) input device
     * Use 'evtest' to find the correct input device. /dev/input/by-id/ is recommended if possible
     * Use /dev/input/by-id/my-mouse-or-touchscreen or /dev/input/eventX
     */
    //const char *input_device = getenv("LV_LINUX_EVDEV_POINTER_DEVICE");
    const char *input_device = getenv_default("LV_LINUX_EVDEV_POINTER_DEVICE", "/dev/input/event1");

    if (input_device == NULL) {
        fprintf(stderr, "please set the LV_LINUX_EVDEV_POINTER_DEVICE environment variable\n");
        printf("please set the LV_LINUX_EVDEV_POINTER_DEVICE environment variable\n");
        exit(1);
    }

    lv_indev_t *touch = lv_evdev_create(LV_INDEV_TYPE_POINTER, input_device);
    lv_indev_set_display(touch, disp);

    /* Set the cursor icon */
    LV_IMAGE_DECLARE(mouse_cursor_icon);
    lv_obj_t * cursor_obj = lv_image_create(lv_screen_active());
    lv_image_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(touch, cursor_obj);
}
#endif

#if LV_USE_LINUX_FBDEV
static void lv_linux_disp_init(void)
{
    const char *device = getenv_default("LV_LINUX_FBDEV_DEVICE", "/dev/fb0");
    lv_display_t * disp = lv_linux_fbdev_create();

#if LV_USE_EVDEV
    lv_linux_init_input_pointer(disp);
#endif

    lv_linux_fbdev_set_file(disp, device);
}
#elif LV_USE_LINUX_DRM
static void lv_linux_disp_init(void)
{
    const char *device = getenv_default("LV_LINUX_DRM_CARD", "/dev/dri/card0");
    lv_display_t * disp = lv_linux_drm_create();

#if LV_USE_EVDEV
    lv_linux_init_input_pointer(disp);
#endif

    lv_linux_drm_set_file(disp, device, -1);
}
#elif LV_USE_SDL
static void lv_linux_disp_init(void)
{

    lv_sdl_window_create(window_width, window_height);

}
#elif LV_USE_WAYLAND
    /* see backend/wayland.c */
#else
#error Unsupported configuration
#endif

#if LV_USE_WAYLAND == 0
void lv_linux_run_loop(void)
{
    uint32_t idle_time;

    /*Handle LVGL tasks*/
    while(1) 
    {
        //LV_LOG_USER("task Test");
        idle_time = lv_timer_handler(); /*Returns the time to the next timer execution*/
        usleep(idle_time * 1000);
    }
}
#endif

/*
 * Process command line arguments and environment
 * variables to configure the simulator
 */
static void configure_simulator(int argc, char **argv)
{

    int opt = 0;
    bool err = false;

    /* Default values */
    fullscreen = maximize = false;
    window_width = atoi(getenv("LV_SIM_WINDOW_WIDTH") ? : "800");
    window_height = atoi(getenv("LV_SIM_WINDOW_HEIGHT") ? : "480");

    /* Parse the command-line options. */
    while ((opt = getopt (argc, argv, "fmw:h:")) != -1) {
        switch (opt) {
        case 'f':
            fullscreen = true;
            if (LV_USE_WAYLAND == 0) {
                fprintf(stderr, "The SDL driver doesn't support fullscreen mode on start\n");
                exit(1);
            }
            break;
        case 'm':
            maximize = true;
            if (LV_USE_WAYLAND == 0) {
                fprintf(stderr, "The SDL driver doesn't support maximized mode on start\n");
                exit(1);
            }
            break;
        case 'w':
            window_width = atoi(optarg);
            break;
        case 'h':
            window_height = atoi(optarg);
            break;
        case ':':
            fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            exit(1);
        case '?':
            fprintf (stderr, "Unknown option -%c.\n", optopt);
            exit(1);
        }
    }
}

static void lv_100ask_lesson_demos(void)
{
    /* 课程示例代码，一般同时只运行一个
        运行前，请将上面的三行代码都注释掉 */
    //lesson_2_3_1();         // 基础对象(lv_obj)，"Hello, LVGL
    //lesson_2_4_1();         // 基础对象的大小(size)
    //lesson_2_5_1();         // 基础对象的位置(position)
    //lesson_2_6_1();         // 基础对象的盒子模型(border-box)
    //lesson_2_7_1();         // 基础对象的样式(styles)
    //lesson_2_8_1();         // 基础对象的事件(events)、事件冒泡
    //lesson_2_9_1();         // LVGL的定时器(lv_timer)-入门

    //lesson_3_1_1();         // 标签控件(lv_label)的用法
    lesson_3_2_1();         // 按钮控件(lv_button)的用法
    //lesson_3_3_1();         // 进度条控件(lv_bar)的用法
    //lesson_3_4_1();         // 滑动条控件(lv_slider)的用法
    //lesson_3_5_1();         // 圆弧控件(lv_arc)的用法
    //lesson_3_6_1();         // 环形加载器控件(lv_spinner)的用法
    //lesson_3_7_1();         // 矩阵按钮控件(lv_btnmatrix)的用法
    //lesson_3_8_1();         // 列表控件(lv_list)的用法
    //lesson_3_9_1();         // 开关控件(lv_switch)的用法
    //lesson_3_10_1();        // 复选框控件(lv_checkbox)的用法
    //lesson_3_11_1();        // 线条控件(lv_line)的用法
    //lesson_3_12_1();        // LED控件(lv_led)的用法
    //lesson_3_13_1();        // 下拉列表控件(lv_dropdown)的用法
    //lesson_3_14_1();        // 滚轮控件(lv_roller)的用法
    //lesson_3_15_1();        // 消息框控件lv_msgbox的用法
    //lesson_3_16_1();        // 富文本控件(lv_span)的用法
    //lesson_3_17_1();        // 文本框控件(lv_textarea)的用法
    //lesson_3_18_1();        // 键盘控件(lv_keyboard)的用法
    //lesson_3_19_1();        // 微调框控件(lv_spinbox)的用法
    //lesson_3_20_1();        // 表格控件(lv_table)的用法
    //lesson_3_21_1();        // 标尺控件lv_scale的用法
    //lesson_3_22_1();        // 图表控件(lv_chart)的用法
    //lesson_3_23_1();        // 日历控件(lv_calendar)的用法
    //lesson_3_24_1();        // 图像控件(lv_image)的用法
    //lesson_3_25_1();        // 图像按钮控件(lv_imagebutton)的用法
    //lesson_3_26_1();        // 动画图像控件(lv_animimg)用法
    //lesson_3_27_1();        // 画布控件(lv_canvas)的用法
    //lesson_3_28_1();        // 选项卡控件(lv_tabview)的用法
    //lesson_3_29_1();        // 平铺视图控件(lv_tileview)的用法
    //lesson_3_30_1();        // 菜单控件(lv_menu)的用法
    //lesson_3_31_1();        // 窗口控件(lv_win)的用法
    //lesson_3_32_1();        // Lottie控件(lv_lottie)的用法

    //lesson_4_1();           // Flex（弹性）布局的用法
    //lesson_4_2();           // Grid（弹性）布局的用法
    //lesson_4_3();           // Flex（弹性）和Grid（网格）布局的区别

    //【需要修改】lesson_5_1();           // LVGL文件系统模块的用法
    //【需要修改】lesson_5_2();           // 通过LVGL文件系统模块的显示图片文件
    //【需要修改】lesson_5_3();           // FreeType控件（lv_freetype）的用法
    //lesson_5_4();           // 二维码控件(lv_qrcode)的用法
    //lesson_5_5();           // 条形码控件(lv_barcode)的用法

    //【需要修改】lesson_6_1();           // 文件资源管理器控件（lv_file_explorer）的用法
    //lesson_6_2();           // 拼音输入法控件（lv_ime_pinyin）的用法
    //lesson_6_3();           // 快照控件（lv_snapshot）的用法
    //lesson_6_4();           // Monkey测试（压力测试）的用法

    //lesson_7_1();           // 更进一步理解LVGL的定时器
    //lesson_7_2();           // 动画的用法
    //lesson_7_3();           // 动画时间线的用法
    //lesson_7_4();           // 动画的本质是定时器处理
    //lesson_7_5();           // 为什么创建出来的不同控件会不一样？
    //【无法使用】lesson_7_6();           // 物理按键代替触摸（鼠标模仿编码器）
    //【无法使用】lesson_7_7();           // Windows HOOK拦截键盘输入模拟按键输入

    //lesson_8_1();           // 基于FontAwesome 添加新SYMBOLS
    //lesson_8_2();           // 调试方法之LV_LOG
    //lesson_8_3();           // 调试方法之LV_SYSMON(system monitor)

    //lesson_9_1();           // 移植适配 lv_lib_100ask
    //lesson_9_2();           // 移植适配 lv_100ask_ui

    //lesson_10_1();           // LVGL的编码风格
    //lesson_10_2();           // 如何阅读Changelog
    //lesson_10_3();           // 如何阅读Roadmap
    //lesson_10_4();           // 如何参与LVGL开发-为LVGL库做贡献
}


static void part3_lesson_3_6_3_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        
        uint32_t index = lv_dropdown_get_selected(obj);
        switch (index)
        {
        case 0:
            lv_display_set_rotation(NULL, LV_DISPLAY_ROTATION_0);
            break;
        case 1:
            lv_display_set_rotation(NULL, LV_DISPLAY_ROTATION_90);
            break;
        case 2:
            lv_display_set_rotation(NULL, LV_DISPLAY_ROTATION_180);
            break;
        case 3:
            lv_display_set_rotation(NULL, LV_DISPLAY_ROTATION_270);
            break;
        
        default:
            lv_display_set_rotation(NULL, LV_DISPLAY_ROTATION_0);
            break;
        }
    }
}

void part3_lesson_3_6_4(void)
{

    /*Create a normal drop down list*/
    lv_obj_t * dd = lv_dropdown_create(lv_screen_active());
    lv_dropdown_set_options(dd, "ROTATION_0\n"
                            "ROTATION_90\n"
                            "ROTATION_180\n"
                            "ROTATION_270");

    lv_obj_align(dd, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_event_cb(dd, part3_lesson_3_6_3_event_handler, LV_EVENT_ALL, NULL);
}


static void part3_lesson_3_6_5_event_handler(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);

    LV_LOG_USER("Set configure here!");

    lv_display_rotation_t rotation = lv_display_get_rotation(disp);

    switch (rotation)
    {
    case LV_DISPLAY_ROTATION_0:
        LV_LOG_USER("LV_DISPLAY_ROTATION_0");
        break;
    case LV_DISPLAY_ROTATION_90:
        LV_LOG_USER("LV_DISPLAY_ROTATION_90");
        break;
    case LV_DISPLAY_ROTATION_180:
        LV_LOG_USER("LV_DISPLAY_ROTATION_180");
        break;
    case LV_DISPLAY_ROTATION_270:
        LV_LOG_USER("LV_DISPLAY_ROTATION_270");
        break;
    
    default:
        break;
    }
    lv_display_set_resolution(NULL, 480, 480);
}

void part3_lesson_3_6_5(void)
{
    lv_display_add_event_cb(lv_display_get_default(), part3_lesson_3_6_5_event_handler, LV_EVENT_RESOLUTION_CHANGED, NULL);
}


int main(int argc, char **argv)
{
    uint32_t idle_time;
    pthread_t thread_id;
    pthread_t thread_id_rkmedia;
    

    // 创建按键监控线程
    if (pthread_create(&thread_id, NULL, key_monitor_thread, NULL) != 0) 
    {
        perror("无法创建线程");
        return -1;
    }

    if(pthread_create(&thread_id_rkmedia, NULL, rkmedia_vi_rockx_thread, NULL))
    {
        perror("无法创建线程");
        return -1;
    }

    configure_simulator(argc, argv);

    /* Initialize LVGL. */
    lv_init();

    /* Initialize the configured backend SDL2, FBDEV, libDRM or wayland */
    lv_linux_disp_init();

    //lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), 0);

    //todo
    //lv_port_indev_init();

    /*Create a Demo*/
    //lv_demo_widgets();
    //lv_demo_widgets_start_slideshow();

    //lv_demo_benchmark();
    //lv_demo_stress();

    //LV_LOG_USER("LV_LOG userTest");
    //LV_LOG("STAGING_DIR: %s\n", getenv("STAGING_DIR"));

    //part3_lesson_3_6_4();
    //part3_lesson_3_6_5();

    //lv_100ask_lesson_demos();

    /*Handle LVGL tasks*/
    while(1) 
    {
        //LV_LOG_USER("task Test");
        idle_time = lv_timer_handler(); /*Returns the time to the next timer execution*/
        usleep(idle_time * 1000);
    }


    return 0;
}
