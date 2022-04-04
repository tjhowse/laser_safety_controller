


#define LV_TABLE_CELL_GREY LV_TABLE_PART_CELL1
#define LV_TABLE_CELL_YELLOW LV_TABLE_PART_CELL3
#define LV_TABLE_CELL_RED LV_TABLE_PART_CELL4

#include <lvgl.h>

#ifndef SIMULATOR
    #include "data.h"
#else
    #define LV_HOR_RES (320)
    #define LV_VER_RES (240)
#endif

#define MONITOR_HOR_RES LV_HOR_RES
#define MONITOR_VER_RES LV_VER_RES

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
    lv_obj_set_click(sensors_table, false);
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

    // lv_style_set_pad_top(LV_TABLE_PART_CELL1, LV_STATE_DEFAULT, 2);
    // lv_style_set_pad_top(LV_TABLE_PART_CELL2, LV_STATE_DEFAULT, 2);
    // lv_style_set_pad_top(LV_TABLE_PART_CELL3, LV_STATE_DEFAULT, 2);
    // lv_style_set_pad_top(LV_TABLE_PART_CELL4, LV_STATE_DEFAULT, 2);
    const int table_cell_pad = 4;
    lv_obj_set_style_local_pad_top(sensors_table, LV_TABLE_CELL_GREY, LV_STATE_DEFAULT, table_cell_pad);
    lv_obj_set_style_local_pad_top(sensors_table, LV_TABLE_CELL_YELLOW, LV_STATE_DEFAULT, table_cell_pad);
    lv_obj_set_style_local_pad_top(sensors_table, LV_TABLE_CELL_RED, LV_STATE_DEFAULT, table_cell_pad);
    lv_obj_set_style_local_pad_bottom(sensors_table, LV_TABLE_CELL_GREY, LV_STATE_DEFAULT, table_cell_pad);
    lv_obj_set_style_local_pad_bottom(sensors_table, LV_TABLE_CELL_YELLOW, LV_STATE_DEFAULT, table_cell_pad);
    lv_obj_set_style_local_pad_bottom(sensors_table, LV_TABLE_CELL_RED, LV_STATE_DEFAULT, table_cell_pad);
    lv_obj_set_style_local_text_color(sensors_table, LV_TABLE_CELL_YELLOW, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
    lv_obj_set_style_local_text_color(sensors_table, LV_TABLE_CELL_RED, LV_STATE_DEFAULT, LV_COLOR_RED);

    lv_table_set_col_cnt(sensors_table, 2);
    lv_table_set_row_cnt(sensors_table, 4);
    lv_table_set_col_width(sensors_table, 0, 170);
    lv_table_set_col_width(sensors_table, 1, 100);

    // Mock some values so we can simulate this table without any actual sensors involved.
    uint8_t row = 0;

    lv_table_set_cell_value(sensors_table, row, 0, "Coolant Flow Rate");
    lv_table_set_cell_value(sensors_table, row, 1, "1.1 L/min");
    lv_table_set_cell_type(sensors_table, row, 0, LV_TABLE_CELL_RED);
    lv_table_set_cell_type(sensors_table, row, 1, LV_TABLE_CELL_RED);
    row++;
    lv_table_set_cell_value(sensors_table, row, 0, "Laser Outlet Temp");
    lv_table_set_cell_value(sensors_table, row, 1, "60.1 °C");
    lv_table_set_cell_type(sensors_table, row, 0, LV_TABLE_CELL_YELLOW);
    lv_table_set_cell_type(sensors_table, row, 1, LV_TABLE_CELL_YELLOW);
    row++;

    lv_table_set_cell_value(sensors_table, row, 0, "Resevoir Temp");
    lv_table_set_cell_value(sensors_table, row, 1, "31.2 °C");
    row++;

    lv_table_set_cell_value(sensors_table, row, 0, "Laser Current");
    lv_table_set_cell_value(sensors_table, row, 1, "20 mA");
    row++;

    lv_table_set_cell_value(sensors_table, row, 0, "Exhaust Flow");
    lv_table_set_cell_value(sensors_table, row, 1, "Yes");
    row++;

    lv_table_set_cell_value(sensors_table, row, 0, "Chiller Output");
    lv_table_set_cell_value(sensors_table, row, 1, "On");
    row++;

    lv_table_set_cell_value(sensors_table, row, 0, "Chiller Temp 1");
    lv_table_set_cell_value(sensors_table, row, 1, "65.3 °C");
    row++;

    lv_table_set_cell_value(sensors_table, row, 0, "Chiller Temp 2");
    lv_table_set_cell_value(sensors_table, row, 1, "55.8 °C");
    row++;

    lv_table_set_cell_value(sensors_table, row, 0, "Chiller Temp 3");
    lv_table_set_cell_value(sensors_table, row, 1, "55.8 °C");
    row++;

    lv_table_set_cell_value(sensors_table, row, 0, "Poop Temp");
    lv_table_set_cell_value(sensors_table, row, 1, "22.5 °C");
    row++;

    lv_table_set_cell_value(sensors_table, row, 0, "Ambient Temp");
    lv_table_set_cell_value(sensors_table, row, 1, "26.0 °C");
    row++;

    lv_table_set_cell_value(sensors_table, row, 0, "Ambient Temp");
    lv_table_set_cell_value(sensors_table, row, 1, "26.0 °C");
    row++;


    for (int i = 0; i < row-1; i++) {
        lv_table_set_cell_align(sensors_table, i, 0, LV_LABEL_ALIGN_LEFT);
        lv_table_set_cell_align(sensors_table, i, 1, LV_LABEL_ALIGN_LEFT);
    }


    label = lv_label_create(io_config_tab, NULL);
    lv_label_set_text(label, "Second tab");

    label = lv_label_create(net_config_tab, NULL);
    lv_label_set_text(label, "Third tab");
}

#ifndef SIMULATOR

void update_sensor_table_display() {
    if (sensors.sensors.size() != lv_table_get_row_cnt(sensors.table)) {
        lv_table_set_row_cnt(sensors.table, sensors.sensors.size());
    }
    for (int i = 0; i < sensors.sensors.size(); i++) {
        lv_table_set_cell_value(sensors.table, i, 0, sensors.sensors[i].name.c_str());
        lv_table_set_cell_value(sensors.table, i, 1, std::to_string(sensors.sensors[i].value).c_str());
    }

}

#endif