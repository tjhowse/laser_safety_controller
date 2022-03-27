//=====================================================================
// メモ : memo : 2021.06.24 : macsbug : https://macsbug.wordpress.com
//---------------------------------------------------------------------
//=====================================================================
// LVGL v7.11.0-dev
// https://docs.lvgl.io/7.11/
//=====================================================================
// LVGL
// https://forum.lvgl.io/
//=====================================================================
// LVGL フォーラム；質問と回答。
// LVGL Forum
// https://forum.lvgl.io/t/welcome-to-the-lvgl-forum/7
//=====================================================================
// Label (lv_label)
// https://docs.lvgl.io/latest/en/html/widgets/label.html
//=====================================================================
// Gauge (lv_gauge)
// https://docs.lvgl.io/latest/en/html/widgets/gauge.html
//=====================================================================
// Chart (lv_chart)
// https://docs.lvgl.io/latest/en/html/widgets/chart.html
//=====================================================================
// Line meter (lv_lmeter)
// https://docs.lvgl.io/latest/en/html/widgets/linemeter.html
//=====================================================================
// chartの目盛り内に数値を表示する。
// https://forum.lvgl.io/t/on-chart-cant-see-x-tick-texts-lv-chart-set-margin-not-present-in-lvgl-version/5176/2
//=====================================================================
// タイトルビューでスクロールバーを非表示にする方法
// https://forum.lvgl.io/t/how-to-hide-the-scrollbar-in-titleview/4140
//  lv_page_set_scrollbar_mode(tileview, LV_SCROLLBAR_MODE_OFF);
//=====================================================================
// 新しいイベントを設定する方法-タブビューのコールバック方法
//  How to set a new event-callback for tabview
// // https://forum.lvgl.io/t/how-to-set-a-new-event-callback-for-tabview/872/13
//=====================================================================
// タブの高さを最小にする // Minimize tab height
//  lv_obj_set_style_local_pad_top(tabview,LV_TABVIEW_PART_TAB_BG, LV_STATE_DEFAULT,1);
//=====================================================================
// Style_set_text_fontが機能しない
// Style_set_text_font not working
// https://forum.lvgl.io/t/style-set-text-font-not-working/4994
//=====================================================================
// テキスト領域のフォントを変更する
// Change font in text area
// https://forum.lvgl.io/t/change-font-in-text-area/3804
//　change : lv_arduino/lv_conf.h/ #define LV_FONT_MONTSERRAT_48 1
//  lv_arduino Libraryを開き lv_conf.h に記載されている
//  #define LV_FONT_MONTSERRAT_48 0 を 1 にする。
//  static lv_style_t st;
//  lv_style_init(&st);
//  lv_style_set_text_font(&st,LV_STATE_DEFAULT,&lv_font_montserrat_48); 
//  48 point の Font が使用できます。
//=====================================================================
// タブビューでのLV_EVENT_VALUE_CHANGEDの使用
// Using of LV_EVENT_VALUE_CHANGED with tabview
// https://forum.lvgl.io/t/using-of-lv-event-value-changed-with-tabview/5317/3
//=====================================================================
// タブビューで切り替えるときの表示の時間を設定する。
// https://forum.lvgl.io/t/how-to-at-tabview-how-to-set-anim-in-the-same-rate-when-changing-between-tabs/1557
// lv_tabview_set_tab_act(tabview, tab_pos, LV_ANIM_ON);
// lv_tabview_set_anim_time(tabview, 2000);  // 2000msec
// lv_tabview_set_tab_act(tabview, 0, LV_ANIM_OFF);
// lv_obj_set_event_cb(tabview, tabview_cb);
//=====================================================================
// 新しいイベントを設定する方法-タブビューのコールバック方法
// How to set a new event-callback for tabview
// https://forum.lvgl.io/t/how-to-set-a-new-event-callback-for-tabview/872/9
//=====================================================================
// バックグラウンドカラー
// Backgroud colour
// https://forum.lvgl.io/t/backgroud-colour/2036/34
// https://forum.lvgl.io/t/backgroud-colour/2036/39
//=====================================================================
// COLOR PICKER
// https://trolsoft.ru/en/articles/rgb565-color-picker
//=====================================================================
// lv_color_rgb_to_hsv（..）で色を変換し、
// when transform color by lv_color_rgb_to_hsv(..) and
// LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)
// https://forum.lvgl.io/t/bug-when-transform-color-by-lv-color-rgb-to-hsv-and-transform-backward-by-lv-color-hsv-to-rgb-it-cant-display-the-same-lv-color-t-color/924/8
//=====================================================================
// グラフの背景を透明に設定し、軸にラベルを表示する方法
// How to set the background transparent of a chart and display labels in the axis
// https://forum.lvgl.io/t/how-to-set-the-background-transparent-of-a-chart-and-display-labels-in-the-axis/3981
//=====================================================================
// バースタイルの変更 / obj color : lv_obj_set_style_local_bg_color
// Change Bar Style
// https://forum.lvgl.io/t/change-bar-style/2537
//=====================================================================
// CCS811 の データー
// eCO2     400
// eTVOC    0
// ERRSTAT 152
// RAW     17824
//
// eCO2     461
// eTVOC    9
// ERRSTAT 152
// RAW     17804
//=====================================================================
/*
// Touchpad Read ------------------------------------------------------
bool my_touchpad_read(lv_indev_drv_t * indev_driver, 
                      lv_indev_data_t * data){
  uint16_t touchX, touchY;
//bool touched = M5.Lcd.getTouch(&touchX, &touchY, 600); // macsbug
  bool touched = M5.Lcd.getTouch(&touchX, &touchY);      // macsbug
  if(!touched){ return false;}
  if(touchX>screenWidth || touchY > screenHeight){
    Serial.println("Y or y outside of expected parameters..");
    Serial.print("y:" );Serial.print(touchX);
    Serial.print(" x:");Serial.print(touchY);
  } else {
    data->state = touched ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;  
    //Save the state and save the pressed coordinate
//if(data->state == LV_INDEV_STATE_PR) touchpad_get_xy(&last_x,&last_y);  
//Set the coordinates (if released use the last pressed coordinates)
    data->point.x = touchX;
    data->point.y = touchY; 
    Serial.print("Data x"); Serial.println(touchX);     
    Serial.print("Data y"); Serial.println(touchY);
  }
  return false; //Return `false` because we are not more data to read
}
*/
//=====================================================================
