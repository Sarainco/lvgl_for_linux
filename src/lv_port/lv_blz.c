#include "lvgl/lvgl.h"



// 在显示视频前隐藏LVGL层
void show_video_layer(bool show) 
{
    static lv_obj_t *screen_backup = NULL;
    static lv_layer_t *layer_backup = NULL;
    
    if (show) 
    {
        // 显示视频时隐藏LVGL
        screen_backup = lv_scr_act();
        layer_backup = lv_layer_top();
        lv_obj_add_flag(layer_backup, LV_OBJ_FLAG_HIDDEN);
    } 
    else 
    {
        // 恢复LVGL显示
        if (screen_backup && layer_backup) 
        {
            lv_obj_clear_flag(layer_backup, LV_OBJ_FLAG_HIDDEN);
            lv_scr_load(screen_backup);
        }
    }
}

