#include "../../include/lv_port/lv_blz.h"
#include <time.h>



#if 0
// 屏幕尺寸定义
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define STATUS_BAR_HEIGHT 60
#define MODE_BAR_HEIGHT 60

// 创建状态栏
static void create_status_bar(lv_obj_t *parent) 
{
    // 创建状态栏容器
    lv_obj_t *status_bar = lv_obj_create(parent);
    lv_obj_set_size(status_bar, SCREEN_WIDTH, STATUS_BAR_HEIGHT);
    lv_obj_set_flex_flow(status_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(status_bar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_hor(status_bar, 20, 0);
    lv_obj_set_style_border_width(status_bar, 0, 0);
    lv_obj_set_style_bg_color(status_bar, lv_color_hex(0x2c3e50), 0);
    lv_obj_set_style_text_color(status_bar, lv_color_white(), 0);
    lv_obj_set_style_text_font(status_bar, &lv_font_montserrat_24, 0);
    
    // 左侧容器（电量）
    lv_obj_t *left_cont = lv_obj_create(status_bar);
    lv_obj_set_size(left_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(left_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(left_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(left_cont, LV_OPA_0, 0);
    lv_obj_set_style_border_opa(left_cont, LV_OPA_0, 0);
    
    // 电池图标
    lv_obj_t *battery_icon = lv_label_create(left_cont);
    lv_label_set_text(battery_icon, LV_SYMBOL_BATTERY_3);
    
    // 电量百分比
    lv_obj_t *battery_percent = lv_label_create(left_cont);
    lv_label_set_text(battery_percent, " 85%");
    
    // 中间容器（标题，可选）
    lv_obj_t *title = lv_label_create(status_bar);
    lv_label_set_text(title, "设备监控系统");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_28, 0);
    
    // 右侧容器（时间）
    lv_obj_t *right_cont = lv_obj_create(status_bar);
    lv_obj_set_size(right_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(right_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(right_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(right_cont, LV_OPA_0, 0);
    lv_obj_set_style_border_opa(right_cont, LV_OPA_0, 0);
    
    // 时间显示
    lv_obj_t *time_label = lv_label_create(right_cont);
    lv_label_set_text(time_label, "14:25:36");
}

// 创建主显示区域
static void create_main_display(lv_obj_t *parent) 
{
    // 创建主显示区域容器
    lv_obj_t *main_display = lv_obj_create(parent);
    lv_obj_set_size(main_display, SCREEN_WIDTH, 
                   SCREEN_HEIGHT - STATUS_BAR_HEIGHT - MODE_BAR_HEIGHT);
    lv_obj_set_style_border_width(main_display, 0, 0);
    lv_obj_set_style_bg_color(main_display, lv_color_hex(0x34495e), 0);
    lv_obj_set_style_radius(main_display, 0, 0);
    
    // 添加一个模拟的实时显像区域
    lv_obj_t *display_area = lv_obj_create(main_display);
    lv_obj_set_size(display_area, SCREEN_WIDTH - 40, 
                   (SCREEN_HEIGHT - STATUS_BAR_HEIGHT - MODE_BAR_HEIGHT) - 40);
    lv_obj_align(display_area, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(display_area, lv_color_black(), 0);
    lv_obj_set_style_border_color(display_area, lv_color_hex(0x7f8c8d), 0);
    lv_obj_set_style_border_width(display_area, 2, 0);
    
    // 添加显示区域标签
    lv_obj_t *label = lv_label_create(display_area);
    lv_label_set_text(label, "实时显像区域\n1280×600像素");
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_32, 0);
    lv_obj_center(label);
}

// 创建模式栏
static void create_mode_bar(lv_obj_t *parent)
{
    // 创建模式栏容器
    lv_obj_t *mode_bar = lv_obj_create(parent);
    lv_obj_set_size(mode_bar, SCREEN_WIDTH, MODE_BAR_HEIGHT);
    lv_obj_set_flex_flow(mode_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(mode_bar, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_width(mode_bar, 0, 0);
    lv_obj_set_style_bg_color(mode_bar, lv_color_hex(0x2c3e50), 0);
    lv_obj_set_style_text_color(mode_bar, lv_color_white(), 0);
    lv_obj_set_style_text_font(mode_bar, &lv_font_montserrat_24, 0);
    
    // 创建模式按钮
    const char *modes[] = {"标准模式", "夜间模式", "节能模式", "设置"};
    for(int i = 0; i < 4; i++) {
        lv_obj_t *btn = lv_btn_create(mode_bar);
        lv_obj_set_size(btn, 200, 50);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x3498db), 0);
        lv_obj_set_style_radius(btn, 10, 0);
        
        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text(label, modes[i]);
        lv_obj_center(label);
    }
}

void create_ui(void) 
{
    // 获取当前活动屏幕
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_size(scr, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(scr, 0, 0);
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    
    // 创建各个部分
    create_status_bar(scr);
    create_main_display(scr);
    create_mode_bar(scr);
}

#if 0
static lv_obj_t *time_label;
static lv_obj_t *date_label;

static void update_datetime(lv_timer_t *timer)
{
    time_t now;
    struct tm *timeinfo;
    char time_str[32];
    char date_str[32];
    
    time(&now);
    timeinfo = localtime(&now);
    
    // 更新时间
    strftime(time_str, sizeof(time_str), "%H:%M:%S", timeinfo);
    lv_label_set_text(time_label, time_str);
    
    // 更新日期
    strftime(date_str, sizeof(date_str), "%Y-%m-%d %A", timeinfo);
    lv_label_set_text(date_label, date_str);
}

void lv_example_datetime(void)
{
    // 设置屏幕背景
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), LV_PART_MAIN);
    
    // 创建时间标签
    time_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_color(time_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, 0);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, -40);
    
    // 创建日期标签
    date_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_color(date_label, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_style_text_font(date_label, &lv_font_montserrat_24, 0);
    lv_obj_align(date_label, LV_ALIGN_CENTER, 0, 20);
    
    // 创建定时器
    lv_timer_create(update_datetime, 1000, NULL);
    
    // 立即更新一次
    update_datetime(NULL);
}

#endif

#if 0
// 全局UI对象
static lv_obj_t *time_label;
static lv_obj_t *date_label;
static lv_obj_t *mode_btn_standard;
static lv_obj_t *mode_btn_vessel;
static lv_obj_t *mode_btn_3d;

// 当前选中模式
static int current_mode = 0; // 0-标准, 1-血管, 2-3D

// 更新时间显示
static void update_datetime(lv_timer_t *timer)
{
    time_t now;
    struct tm *timeinfo;
    char time_str[32];
    char date_str[32];
    
    time(&now);
    timeinfo = localtime(&now);
    
    strftime(time_str, sizeof(time_str), "%H:%M:%S", timeinfo);
    lv_label_set_text(time_label, time_str);
    
    strftime(date_str, sizeof(date_str), "%Y-%m-%d %A", timeinfo);
    lv_label_set_text(date_label, date_str);
}

// 模式按钮事件处理
static void mode_btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        lv_obj_t *btn = lv_event_get_target(e);
        
        // 重置所有按钮样式
        lv_obj_clear_state(mode_btn_standard, LV_STATE_CHECKED);
        lv_obj_clear_state(mode_btn_vessel, LV_STATE_CHECKED);
        lv_obj_clear_state(mode_btn_3d, LV_STATE_CHECKED);
        
        // 设置当前按钮为选中状态
        lv_obj_add_state(btn, LV_STATE_CHECKED);
        
        // 更新当前模式
        if(btn == mode_btn_standard) current_mode = 0;
        else if(btn == mode_btn_vessel) current_mode = 1;
        else if(btn == mode_btn_3d) current_mode = 2;
        
        // 这里可以添加模式切换的实际逻辑
    }
}

// 创建界面
void create_main_ui(void)
{
    // 设置屏幕背景
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x1A1A1A), LV_PART_MAIN);
    
    /* 创建时间日期区域 - 上方中间 */
    lv_obj_t *time_panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(time_panel, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(time_panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(time_panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(time_panel, 10, 0);
    lv_obj_align(time_panel, LV_ALIGN_TOP_MID, 0, 20);
    
    // 时间标签
    time_label = lv_label_create(time_panel);
    lv_obj_set_style_text_color(time_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_32, 0);
    lv_label_set_text(time_label, "00:00:00");
    lv_obj_align(time_label, LV_ALIGN_TOP_MID, 0, 0);
    
    // 日期标签
    date_label = lv_label_create(time_panel);
    lv_obj_set_style_text_color(date_label, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_style_text_font(date_label, &lv_font_montserrat_20, 0);
    lv_label_set_text(date_label, "2023-01-01 Monday");
    lv_obj_align(date_label, LV_ALIGN_TOP_MID, 0, 40);
    
    // /* 创建模式选择区域 - 下方 */
    // lv_obj_t *mode_panel = lv_obj_create(lv_scr_act());
    // lv_obj_set_size(mode_panel, LV_PCT(90), 120);
    // lv_obj_set_style_bg_color(mode_panel, lv_color_hex(0x2D2D2D), 0);
    // lv_obj_set_style_radius(mode_panel, 10, 0);
    // lv_obj_align(mode_panel, LV_ALIGN_BOTTOM_MID, 0, -20);
    
    // // 模式选择标题
    // lv_obj_t *mode_title = lv_label_create(mode_panel);
    // lv_label_set_text(mode_title, "显示模式");
    // lv_obj_set_style_text_color(mode_title, lv_color_hex(0xFFFFFF), 0);
    // lv_obj_align(mode_title, LV_ALIGN_TOP_MID, 0, 10);
    
    // // 创建按钮容器
    // lv_obj_t *btn_cont = lv_obj_create(mode_panel);
    // lv_obj_set_size(btn_cont, LV_PCT(90), 60);
    // lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
    // lv_obj_set_flex_align(btn_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    // lv_obj_set_style_bg_opa(btn_cont, LV_OPA_TRANSP, 0);
    // lv_obj_set_style_border_opa(btn_cont, LV_OPA_TRANSP, 0);
    // lv_obj_align(btn_cont, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    // // 标准模式按钮
    // mode_btn_standard = lv_btn_create(btn_cont);
    // lv_obj_set_size(mode_btn_standard, 100, 40);
    // lv_obj_add_event_cb(mode_btn_standard, mode_btn_event_cb, LV_EVENT_CLICKED, NULL);
    // lv_obj_add_flag(mode_btn_standard, LV_OBJ_FLAG_CHECKABLE);
    // lv_obj_add_state(mode_btn_standard, LV_STATE_CHECKED); // 默认选中
    
    // lv_obj_t *btn1_label = lv_label_create(mode_btn_standard);
    // lv_label_set_text(btn1_label, "标准显像");
    // lv_obj_center(btn1_label);
    
    // // 血管高亮按钮
    // mode_btn_vessel = lv_btn_create(btn_cont);
    // lv_obj_set_size(mode_btn_vessel, 100, 40);
    // lv_obj_add_event_cb(mode_btn_vessel, mode_btn_event_cb, LV_EVENT_CLICKED, NULL);
    // lv_obj_add_flag(mode_btn_vessel, LV_OBJ_FLAG_CHECKABLE);
    
    // lv_obj_t *btn2_label = lv_label_create(mode_btn_vessel);
    // lv_label_set_text(btn2_label, "血管高亮");
    // lv_obj_center(btn2_label);
    
    // // 3D立体按钮
    // mode_btn_3d = lv_btn_create(btn_cont);
    // lv_obj_set_size(mode_btn_3d, 100, 40);
    // lv_obj_add_event_cb(mode_btn_3d, mode_btn_event_cb, LV_EVENT_CLICKED, NULL);
    // lv_obj_add_flag(mode_btn_3d, LV_OBJ_FLAG_CHECKABLE);
    
    // lv_obj_t *btn3_label = lv_label_create(mode_btn_3d);
    // lv_label_set_text(btn3_label, "3D立体");
    // lv_obj_center(btn3_label);
    
    // 创建定时器更新时间
    lv_timer_create(update_datetime, 1000, NULL);
    update_datetime(NULL); // 立即更新一次
}

#endif


static lv_obj_t *screen1, *screen2;  // 两个屏幕
static lv_obj_t *time_label, *battery_label, *battery_icon, *system_time_label;
static lv_obj_t *screen2_label;      // 第二个屏幕的标签
static int battery_level = 80;       // 电池电量
static lv_indev_t *indev;            // 输入设备
static lv_group_t *group;            // 对象组

// 电池状态
static int battery_level = 78;       // 当前电量(78%)
static lv_timer_t *update_timer;     // 更新定时器

// 工作模式定义
typedef enum {
    MODE_STANDARD = 0,
    MODE_VESSEL_ENHANCE,
    MODE_3D_VIEW,
    MODE_COUNT
} WorkMode;

static WorkMode current_mode = MODE_STANDARD;
static const char* mode_names[MODE_COUNT] = {"mode", "qwer", "3d"};

// 更新界面数据
static void update_ui(lv_timer_t *timer) 
{
    // 1. 更新时间
    time_t now;
    struct tm *timeinfo;
    char time_str[16];
    char sys_time_str[32];
    
    time(&now);
    timeinfo = localtime(&now);
    
    // 上部时间显示(时分)
    strftime(time_str, sizeof(time_str), "%H:%M", timeinfo);
    lv_label_set_text(time_label, time_str);
    
    // 底部系统时间(完整格式)
    strftime(sys_time_str, sizeof(sys_time_str), "%Y-%m-%d %H:%M:%S", timeinfo);
    lv_label_set_text(system_time_label, sys_time_str);
    
    // 2. 更新电池状态(模拟电量变化)
    //battery_level = (battery_level + 1) % 100;
    lv_label_set_text_fmt(battery_label, "%d%%", battery_level);
    
    // 根据电量改变图标颜色
    if(battery_level < 20) {
        lv_obj_set_style_text_color(battery_icon, lv_color_hex(0xFF0000), 0);
    } else {
        lv_obj_set_style_text_color(battery_icon, lv_color_hex(0x00FF00), 0);
    }
}

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
        LV_LOG_USER("Option: %s", buf);
    }
}

// 创建界面
void create_status_ui() 
{
    // 设置黑色背景
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), LV_PART_MAIN);
    
    /* 1. 上部区域 - 时间+电量 */
    lv_obj_t *top_panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(top_panel, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(top_panel, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(top_panel, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(top_panel, 10, 0);
    lv_obj_set_style_bg_opa(top_panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(top_panel, LV_OPA_TRANSP, 0);
    lv_obj_align(top_panel, LV_ALIGN_TOP_MID, 0, 10);
    
    // 时间显示
    time_label = lv_label_create(top_panel);
    lv_obj_set_style_text_color(time_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_28, 0);
    lv_label_set_text(time_label, "00:00");
    
    // 电池组
    lv_obj_t *battery_group = lv_obj_create(top_panel);
    lv_obj_set_size(battery_group, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(battery_group, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(battery_group, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(battery_group, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(battery_group, LV_OPA_TRANSP, 0);
    
    // 电池图标(使用LVGL内置符号)
    battery_icon = lv_label_create(battery_group);
    lv_obj_set_style_text_color(battery_icon, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_text_font(battery_icon, &lv_font_montserrat_24, 0);
    lv_label_set_text(battery_icon, LV_SYMBOL_BATTERY_FULL);
    
    // 电量百分比
    battery_label = lv_label_create(battery_group);
    lv_obj_set_style_text_color(battery_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(battery_label, &lv_font_montserrat_20, 0);
    lv_label_set_text_fmt(battery_label, "%d%%", battery_level);

#if 0
    /* 2. 中部区域 - 工作模式 */
    lv_obj_t *mid_panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(mid_panel, LV_PCT(80), 100);
    lv_obj_align(mid_panel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(mid_panel, lv_color_hex(0x222222), 0);
    lv_obj_set_style_radius(mid_panel, 10, 0);
    lv_obj_set_flex_flow(mid_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(mid_panel, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // // 模式图标(可根据实际模式更换图标)
    mode_icon = lv_label_create(mid_panel);
    lv_obj_set_style_text_color(mode_icon, lv_color_hex(0x00BFFF), 0);
    lv_obj_set_style_text_font(mode_icon, &lv_font_montserrat_36, 0);
    lv_label_set_text(mode_icon, LV_SYMBOL_SETTINGS); // 默认使用设置图标
    
    // // 模式文字
    mode_label = lv_label_create(mid_panel);
    lv_obj_set_style_text_color(mode_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(mode_label, &lv_font_montserrat_24, 0);
    lv_label_set_text(mode_label, mode_names[current_mode]);
#endif
    lv_obj_t * dd = lv_dropdown_create(lv_screen_active());
    lv_obj_set_width(dd, lv_pct(50));  // 百分比宽度
    lv_obj_set_height(dd, 60);         // 固定高度
    lv_dropdown_set_options(dd, "Apple\n"
                                "Banana\n"
                                "Orange\n"
                                "Cherry\n"
                                "Grape\n"
                                "Raspberry\n"
                                "Melon\n"
                                "Orange\n"
                                "Lemon\n"
                                "Nuts");

    //lv_dropdown_add_option(dd, "New option", 0);
    //lv_dropdown_add_option(dd, "New option", LV_DROPDOWN_POS_LAST);
    //lv_dropdown_set_selected(dd, 2);
    //lv_dropdown_set_selected(dd, -1);

    lv_obj_align(dd, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_style_bg_color(dd, lv_color_hex(0xff3c3b), LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_bg_color(dd, lv_color_hex(0x222222), 0);

    lv_obj_t * dd_list = lv_dropdown_get_list(dd);
    lv_obj_set_style_bg_color(dd_list, lv_color_hex(0xff3c3b), LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_bg_color(dd_list, lv_color_hex(0x222222), 0);

    //lv_dropdown_set_dir(dd, LV_DIR_LEFT);
    lv_dropdown_set_symbol(dd, LV_SYMBOL_UPLOAD);

    //lv_dropdown_set_text(dd, "www.100ask.net");
    //lv_dropdown_open(dd);

    lv_obj_add_event_cb(dd, event_handler, LV_EVENT_ALL, NULL);

    /* 3. 底部区域 - 系统时间 */
    system_time_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_color(system_time_label, lv_color_hex(0xAAAAAA), 0);
    lv_obj_set_style_text_font(system_time_label, &lv_font_montserrat_18, 0);
    lv_obj_align(system_time_label, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_label_set_text(system_time_label, "2023-01-01 00:00:00");
    
    // 创建定时器(每秒更新一次)
    update_timer = lv_timer_create(update_ui, 1000, NULL);
    update_ui(NULL); // 立即更新一次
}


#endif


// 全局变量定义
static lv_obj_t *screen1, *screen2;
static lv_obj_t *time_label, *battery_label, *battery_icon, *system_time_label;
static lv_obj_t *screen2_label;
static int battery_level = 80;
static lv_timer_t *update_timer;

// 更新界面数据
static void update_ui(lv_timer_t *timer) 
{
    // 更新时间
    time_t now;
    struct tm *timeinfo;
    char time_str[16];
    char sys_time_str[32];
    
    time(&now);
    timeinfo = localtime(&now);
    
    // 上部时间显示(时分)
    strftime(time_str, sizeof(time_str), "%H:%M", timeinfo);
    lv_label_set_text(time_label, time_str);
    
    // 底部系统时间(完整格式)
    strftime(sys_time_str, sizeof(sys_time_str), "%Y-%m-%d %H:%M:%S", timeinfo);
    lv_label_set_text(system_time_label, sys_time_str);
    
    // 更新电池状态(模拟电量变化)
    //battery_level = (battery_level > 0) ? (battery_level - 1) % 100 : 100;
    lv_label_set_text_fmt(battery_label, "%d%%", battery_level);
    
    // 根据电量改变图标颜色
    if(battery_level < 20) {
        lv_obj_set_style_text_color(battery_icon, lv_color_hex(0xFF0000), 0);
    } else {
        lv_obj_set_style_text_color(battery_icon, lv_color_hex(0x00FF00), 0);
    }
}

// 事件处理回调
static void event_handler(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    
    if(code == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
        LV_LOG_USER("Option: %s", buf);
    }
}

// 手势事件处理
static void gesture_event_handler(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    
    if(code == LV_EVENT_GESTURE) {
        lv_indev_t * indev = lv_indev_get_act();
        if(indev == NULL) return;
        
        lv_dir_t dir = lv_indev_get_gesture_dir(indev);
        
        if(dir == LV_DIR_LEFT) {
            // 向左滑动切换到第二个界面
            lv_scr_load_anim(screen2, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
        }
        else if(dir == LV_DIR_RIGHT) {
            // 向右滑动返回第一个界面
            lv_scr_load_anim(screen1, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
        }
    }
}

// 按钮事件处理
static void btn_event_handler(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    
    if(code == LV_EVENT_CLICKED) {
        lv_scr_load_anim(screen1, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
    }
}

// 创建主界面
static void create_main_ui(lv_obj_t *parent) 
{
    // 设置黑色背景
    lv_obj_set_style_bg_color(parent, lv_color_hex(0x000000), LV_PART_MAIN);
    
    /* 1. 上部区域 - 时间+电量 */
    lv_obj_t *top_panel = lv_obj_create(parent);
    lv_obj_set_size(top_panel, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(top_panel, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(top_panel, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(top_panel, 10, 0);
    lv_obj_set_style_bg_opa(top_panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(top_panel, LV_OPA_TRANSP, 0);
    lv_obj_align(top_panel, LV_ALIGN_TOP_MID, 0, 10);
    
    // 时间显示
    time_label = lv_label_create(top_panel);
    lv_obj_set_style_text_color(time_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_28, 0);
    lv_label_set_text(time_label, "00:00");
    
    // 电池组
    lv_obj_t *battery_group = lv_obj_create(top_panel);
    lv_obj_set_size(battery_group, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(battery_group, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(battery_group, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(battery_group, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(battery_group, LV_OPA_TRANSP, 0);
    
    // 电池图标
    battery_icon = lv_label_create(battery_group);
    lv_obj_set_style_text_color(battery_icon, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_text_font(battery_icon, &lv_font_montserrat_24, 0);
    lv_label_set_text(battery_icon, LV_SYMBOL_BATTERY_FULL);
    
    // 电量百分比
    battery_label = lv_label_create(battery_group);
    lv_obj_set_style_text_color(battery_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(battery_label, &lv_font_montserrat_20, 0);
    lv_label_set_text_fmt(battery_label, "%d%%", battery_level);

    /* 2. 下拉菜单 */
    lv_obj_t * dd = lv_dropdown_create(parent);
    lv_obj_set_width(dd, lv_pct(50));
    lv_obj_set_height(dd, 60);
    lv_dropdown_set_options(dd, "Standard display\nVessel highlighting\n3D stereoscopic display");
    lv_obj_align(dd, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(dd, lv_color_hex(0xff3c3b), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_t * dd_list = lv_dropdown_get_list(dd);
    lv_obj_set_style_bg_color(dd_list, lv_color_hex(0xff3c3b), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(dd, event_handler, LV_EVENT_ALL, NULL);

    /* 3. 底部区域 - 系统时间 */
    system_time_label = lv_label_create(parent);
    lv_obj_set_style_text_color(system_time_label, lv_color_hex(0xAAAAAA), 0);
    lv_obj_set_style_text_font(system_time_label, &lv_font_montserrat_18, 0);
    lv_obj_align(system_time_label, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_label_set_text(system_time_label, "2023-01-01 00:00:00");
}

// 创建第二个界面
static void create_second_ui(lv_obj_t *parent) 
{
    // 设置蓝色背景以示区别
    lv_obj_set_style_bg_color(parent, lv_color_hex(0x000033), LV_PART_MAIN);
    
    // 添加一个大标签
    screen2_label = lv_label_create(parent);
    lv_obj_set_style_text_color(screen2_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(screen2_label, &lv_font_montserrat_36, 0);
    lv_label_set_text(screen2_label, "lalalalala\n");
    lv_obj_center(screen2_label);
    
    // 添加返回按钮
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_set_size(btn, 120, 50);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    
    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "return");
    lv_obj_center(btn_label);
    
    lv_obj_add_event_cb(btn, btn_event_handler, LV_EVENT_CLICKED, NULL);
}

// 创建界面
void create_status_ui(void) 
{
    // 创建两个屏幕
    screen1 = lv_obj_create(NULL);
    screen2 = lv_obj_create(NULL);
    
    // 创建两个界面的UI
    create_main_ui(screen1);
    create_second_ui(screen2);
    
    // 添加手势检测
    lv_obj_add_event_cb(screen1, gesture_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(screen2, gesture_event_handler, LV_EVENT_ALL, NULL);
    
    // 加载第一个屏幕
    lv_scr_load(screen1);
    
    // 创建定时器(每秒更新一次)
    update_timer = lv_timer_create(update_ui, 1000, NULL);
    update_ui(NULL); // 立即更新一次
}