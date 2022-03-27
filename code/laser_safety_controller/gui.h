

#define LV_HOR_RES (320)
#define LV_VER_RES (240)

#define MONITOR_HOR_RES LV_HOR_RES
#define MONITOR_VER_RES LV_VER_RES

#include <lvgl.h>

#ifndef SIMULATOR
    #include "data.h"
#endif

void setup_gui(void) {
    /*Create a Tab view object*/
    lv_obj_t *tabview;
    tabview = lv_tabview_create(lv_scr_act(), NULL);
    // Reduce the margin around the tab titles
    lv_obj_set_style_local_pad_top(
                   tabview, LV_TABVIEW_PART_TAB_BG, LV_STATE_DEFAULT,1);
    lv_obj_set_style_local_pad_bottom(
                   tabview, LV_TABVIEW_PART_TAB_BG, LV_STATE_DEFAULT,1);
    lv_obj_set_style_local_pad_top(
                   tabview, LV_TABVIEW_PART_TAB_BTN,LV_STATE_DEFAULT,1);
    lv_obj_set_style_local_pad_bottom(
                   tabview, LV_TABVIEW_PART_TAB_BTN,LV_STATE_DEFAULT,1);

    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t *status_tab = lv_tabview_add_tab(tabview, "Status");
    lv_obj_t *io_config_tab = lv_tabview_add_tab(tabview, "IO Config");
    lv_obj_t *net_config_tab = lv_tabview_add_tab(tabview, "Net Config");

    lv_obj_t *sensors_table = lv_table_create(status_tab, NULL);
    // LV_TABLE_PART_BG
    lv_obj_set_style_local_bg_color(sensors_table, LV_TABLE_PART_BG, LV_STATE_DEFAULT, LV_COLOR_GRAY);

#ifndef SIMULATOR
    sensors.register_table(sensors_table);
#endif

    /*Add content to the tabs*/
    lv_obj_t * label = lv_label_create(NULL, NULL);
    static lv_anim_path_t path_overshoot;
    lv_anim_path_init(&path_overshoot);
    lv_anim_path_set_cb(&path_overshoot, lv_anim_path_overshoot);

    static lv_anim_path_t path_ease_out;
    lv_anim_path_init(&path_ease_out);
    lv_anim_path_set_cb(&path_ease_out, lv_anim_path_ease_out);

    static lv_anim_path_t path_ease_in_out;
    lv_anim_path_init(&path_ease_in_out);
    lv_anim_path_set_cb(&path_ease_in_out, lv_anim_path_ease_in_out);


    lv_obj_set_style_local_text_color(sensors_table, LV_TABLE_PART_CELL3, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
    lv_obj_set_style_local_text_color(sensors_table, LV_TABLE_PART_CELL4, LV_STATE_DEFAULT, LV_COLOR_RED);

    lv_table_set_col_cnt(sensors_table, 2);
    lv_table_set_row_cnt(sensors_table, 4);
    lv_table_set_col_width(sensors_table, 0, 170);
    lv_table_set_col_width(sensors_table, 1, 100);

    // Mock some values so we can simulate this table without any actual sensors involved.
    lv_table_set_cell_value(sensors_table, 0, 0, "Resevoir Temp");
    lv_table_set_cell_value(sensors_table, 0, 1, "31.2 °C");
    lv_table_set_cell_value(sensors_table, 1, 0, "Laser Outlet Temp");
    lv_table_set_cell_value(sensors_table, 1, 1, "60.1 °C");
    lv_table_set_cell_type(sensors_table, 1, 0, LV_TABLE_PART_CELL3);
    lv_table_set_cell_type(sensors_table, 1, 1, LV_TABLE_PART_CELL3);
    lv_table_set_cell_value(sensors_table, 2, 0, "Coolant Flow Rate");
    lv_table_set_cell_value(sensors_table, 2, 1, "1.1 L/min");
    lv_table_set_cell_type(sensors_table, 2, 0, LV_TABLE_PART_CELL4);
    lv_table_set_cell_type(sensors_table, 2, 1, LV_TABLE_PART_CELL4);
    lv_table_set_cell_value(sensors_table, 3, 0, "Laser Current");
    lv_table_set_cell_value(sensors_table, 3, 1, "20 mA");

    for (int i = 0; i < 4; i++) {
        lv_table_set_cell_align(sensors_table, i, 0, LV_LABEL_ALIGN_LEFT);
        lv_table_set_cell_align(sensors_table, i, 1, LV_LABEL_ALIGN_LEFT);
    }

    // /*Gum-like button*/
    // static lv_style_t style_gum;
    // lv_style_init(&style_gum);
    // lv_style_set_transform_width(&style_gum, LV_STATE_PRESSED, 10);
    // lv_style_set_transform_height(&style_gum, LV_STATE_PRESSED, -10);
    // lv_style_set_value_letter_space(&style_gum, LV_STATE_PRESSED, 5);
    // lv_style_set_transition_path(&style_gum, LV_STATE_DEFAULT, &path_overshoot);
    // lv_style_set_transition_path(&style_gum, LV_STATE_PRESSED, &path_ease_in_out);
    // lv_style_set_transition_time(&style_gum, LV_STATE_DEFAULT, 250);
    // lv_style_set_transition_delay(&style_gum, LV_STATE_DEFAULT, 100);
    // lv_style_set_transition_prop_1(&style_gum, LV_STATE_DEFAULT, LV_STYLE_TRANSFORM_WIDTH);
    // lv_style_set_transition_prop_2(&style_gum, LV_STATE_DEFAULT, LV_STYLE_TRANSFORM_HEIGHT);
    // lv_style_set_transition_prop_3(&style_gum, LV_STATE_DEFAULT, LV_STYLE_VALUE_LETTER_SPACE);

    // lv_obj_t * btn1 = lv_btn_create(status_tab, NULL);
    // lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, -20);
    // lv_obj_add_style(btn1, LV_BTN_PART_MAIN, &style_gum);

    // /*Instead of creating a label add a values string*/
    // lv_obj_set_style_local_value_str(btn1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "Gum");

    // /*Halo on press*/
    // static lv_style_t style_halo;
    // lv_style_init(&style_halo);
    // lv_style_set_transition_time(&style_halo, LV_STATE_PRESSED, 400);
    // lv_style_set_transition_time(&style_halo, LV_STATE_DEFAULT, 0);
    // lv_style_set_transition_delay(&style_halo, LV_STATE_DEFAULT, 200);
    // lv_style_set_outline_width(&style_halo, LV_STATE_DEFAULT, 0);
    // lv_style_set_outline_width(&style_halo, LV_STATE_PRESSED, 20);
    // lv_style_set_outline_opa(&style_halo, LV_STATE_DEFAULT, LV_OPA_COVER);
    // lv_style_set_outline_opa(&style_halo, LV_STATE_FOCUSED, LV_OPA_COVER);   /*Just to be sure, the theme might use it*/
    // lv_style_set_outline_opa(&style_halo, LV_STATE_PRESSED, LV_OPA_TRANSP);
    // lv_style_set_transition_prop_1(&style_halo, LV_STATE_DEFAULT, LV_STYLE_OUTLINE_OPA);
    // lv_style_set_transition_prop_2(&style_halo, LV_STATE_DEFAULT, LV_STYLE_OUTLINE_WIDTH);

    // lv_obj_t * btn2 = lv_btn_create(status_tab, NULL);
    // lv_obj_align(btn2, NULL, LV_ALIGN_CENTER, 0, 60);
    // lv_obj_add_style(btn2, LV_BTN_PART_MAIN, &style_halo);
    // lv_obj_set_style_local_value_str(btn2, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "Halo");

    // /*Ripple on press*/
    // static lv_style_t style_ripple;
    // lv_style_init(&style_ripple);
    // lv_style_set_transition_time(&style_ripple, LV_STATE_PRESSED, 300);
    // lv_style_set_transition_time(&style_ripple, LV_STATE_DEFAULT, 0);
    // lv_style_set_transition_delay(&style_ripple, LV_STATE_DEFAULT, 300);
    // lv_style_set_bg_opa(&style_ripple, LV_STATE_DEFAULT, 0);
    // lv_style_set_bg_opa(&style_ripple, LV_STATE_PRESSED, LV_OPA_80);
    // lv_style_set_border_width(&style_ripple, LV_STATE_DEFAULT, 0);
    // lv_style_set_outline_width(&style_ripple, LV_STATE_DEFAULT, 0);
    // lv_style_set_transform_width(&style_ripple, LV_STATE_DEFAULT, -20);
    // lv_style_set_transform_height(&style_ripple, LV_STATE_DEFAULT, -20);
    // lv_style_set_transform_width(&style_ripple, LV_STATE_PRESSED, 0);
    // lv_style_set_transform_height(&style_ripple, LV_STATE_PRESSED, 0);

    // lv_style_set_transition_path(&style_ripple, LV_STATE_DEFAULT, &path_ease_out);
    // lv_style_set_transition_prop_1(&style_ripple, LV_STATE_DEFAULT, LV_STYLE_BG_OPA);
    // lv_style_set_transition_prop_2(&style_ripple, LV_STATE_DEFAULT, LV_STYLE_TRANSFORM_WIDTH);
    // lv_style_set_transition_prop_3(&style_ripple, LV_STATE_DEFAULT, LV_STYLE_TRANSFORM_HEIGHT);

    // lv_obj_t * btn3 = lv_btn_create(status_tab, NULL);
    // lv_obj_align(btn3, NULL, LV_ALIGN_CENTER, 0, 140);
    // lv_obj_add_style(btn3, LV_BTN_PART_MAIN, &style_ripple);
    // lv_obj_set_style_local_value_str(btn3, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "Ripple");

    label = lv_label_create(io_config_tab, NULL);
    lv_label_set_text(label, "Second tab");

    label = lv_label_create(net_config_tab, NULL);
    lv_label_set_text(label, "Third tab");
}
