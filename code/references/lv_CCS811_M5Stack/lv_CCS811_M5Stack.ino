//=====================================================================
// CCS811 CO2 VOC Sensor M5Stack : 2021.06.27 : macsbug
// LVGL    : v7.11.0-dev : https://docs.lvgl.io/7.11/
// Document: https://docs.lvgl.io/latest/en/html/widgets/
// Tabview : https://docs.lvgl.io/latest/en/html/widgets/tabview.html
// Label   : https://docs.lvgl.io/latest/en/html/widgets/label.html
// Guage   : https://docs.lvgl.io/latest/en/html/widgets/gauge.html
// L_Meter : https://docs.lvgl.io/latest/en/html/widgets/linemeter.html
// Forum   : https://forum.lvgl.io
// ESP32   : LVGL : https://programmersought.com/article/10128234373/
// LIBRARY : LGFX : https://github.com/lovyan03/LovyanGFX
// HARD    : M5Stack.h
//    https://github.com/m5stack/M5Stack/blob/master/src/M5Stack.h#L19   
// BLOG    : How to run LVGL on M5Stack ( ESP32 )
//         : https://macsbug.wordpress.com/2021/06/18/
// BLOG    : ButtonA on M5Stack does not work properly
//         : https://macsbug.wordpress.com/2021/05/02/
// BLOG    : Display CO2 and TVOC in LVGL on M5Stack
//         : https://macsbug.wordpress.com/2021/07/05/
//---------------------------------------------------------------------
// SENSOR  : CO2, VOC : CCS811 : https://revspace.nl/CJMCU-811C
//  https://cdn.sparkfun.com/assets/2/c/c/6/5/
//          CN04-2019_attachment_CCS811_Datasheet_v1-06.pdf
//  https://github.com/maarten-pennings/CCS811/blob/master/src/ccs811.h
//  https://www.bountysource.com/teams/ccs811/issues
//  https://strawberry-linux.com/pub/ccs811-manual.pdf
// CCS811           Mode
// CCS811_MODE_IDLE  0
// CCS811_MODE_1SEC  1  sensor surface temperature is approx 40°C
// CCS811_MODE_10SEC 2
// CCS811_MODE_60SEC 3
// Sens  : CO2（二酸化炭素）, TVOCs（総揮発性有機化合物）
// Range : CO2  ：400ppm～8192ppm
// Range : TVOCs：  0ppb～1187ppb
// Arduino IDE : ESP32 : Flush Frequency = 80MHz
//---------------------------------------------------------------------
// WIRING  : CCS811  VCC , GND , SCL , SDA , WAK
//         : M5Stack 3V3 , GND , 22  , 21  , GND
//=====================================================================

#pragma GCC optimize ("Ofast")
#include <M5Stack.h>
#include <lvgl.h>
int screenWidth  = 320;
int screenHeight = 240;
#define LVGL_TICK_PERIOD 30
#include <Ticker.h>
Ticker tick;
#include <Wire.h>
#include "ccs811.h"
CCS811 ccs811(23); // Value at ESP32
#include <driver/adc.h>
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];
static lv_obj_t * page;
static lv_obj_t * txt2;
static lv_obj_t * txt5;
static lv_obj_t * label;
static lv_obj_t * gauge;
static lv_obj_t * chart1;
static lv_obj_t * chart2;
static lv_obj_t * tabview;
static lv_chart_series_t *ser1;
static lv_chart_series_t *ser2;
int tb = 4;           // Number of tabs
int tab_pos = 0;
int time_count = 0;
uint32_t period = 10; // css811 Mode Time
bool auto_man = true;

//=====================================================================
// Interrupt driven periodic handler ----------------------------------
static void lv_tick_handler(void){lv_tick_inc(LVGL_TICK_PERIOD);}
//=====================================================================
int mx = screenWidth/(tb*4), my = 10, md = screenWidth/tb;
// Read M5Stack A B,C buttons as touchpad : Tabview select ------------
static bool my_touchpad_read(lv_indev_drv_t * indev_driver,
                             lv_indev_data_t * data){
  // A Button : Leftward ----------------------------------------------
  if (M5.BtnA.wasPressed()){ 
    mx = mx - md; if (mx < 0){ mx = screenWidth - (md / 4);}
    data->point.x = mx; data->point.y = my;
    data->state =LV_INDEV_STATE_PR; M5.update(); return true;
  }
  // C Button : Rightward ---------------------------------------------
  if (M5.BtnC.wasPressed()){ 
    mx = mx + md; if (mx > screenWidth){ mx = md / 4;}
    data->point.x = mx; data->point.y = my;
    data->state =LV_INDEV_STATE_PR; M5.update(); return true;
  }
  // B Button : Auto / Manual Display ---------------------------------
  if (M5.BtnB.wasPressed()){
    auto_man = !auto_man;          M5.update(); return false;
  }
  // No button operation ----------------------------------------------
  data->state =LV_INDEV_STATE_REL; M5.update(); return false;
}
//=====================================================================
