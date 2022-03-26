/*
// Note: For bad and dumb reasons the arduino IDE does not currently let you set
// -DLV_CONF_INCLUDE_SIMPLE in the linker config... so we have to manually copy
// lv_conf.h into wherever your lvgl library is installed.
// #define LV_CONF_INCLUDE_SIMPLE
// #include "lv_conf.h"
#include <lvgl.h>
// #include <TFT_eSPI.h>
#include <M5Display.h>
#include <M5Touch.h>
// #include <M5Core2.h>
// #include <WiFi.h>
// #include <PubSubClient.h>

// These defines are drawn from the M5Core2 library to configure the screen.
// #define TFT_LED_PIN 32
// #define TFT_DC_PIN 27
// #define TFT_CS_PIN 14
// #define TFT_MOSI_PIN 23
// #define TFT_CLK_PIN 18
// #define TFT_RST_PIN 33
// #define TFT_MISO_PIN 19
// #define TFT_eSPI_TOUCH_EMULATION

// Define the dimensions of the screen
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;

// The screen buffer
static lv_disp_draw_buf_t draw_buf;

// Not sure what this is.
static lv_color_t buf[ screenWidth * 10 ];

// Initialise the TFT driver. The M5Display library uses this under the hood
// TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

M5Display Lcd;
// #include "tj_screen.h"

void setup() {
    lv_init();
    Lcd.begin();

    // screen_init(&tft);
    // screen_init();
    lv_obj_t *label = lv_label_create( lv_scr_act() );
    lv_label_set_text( label, "Hello world!" );
    lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );

}

void loop() {
    lv_timer_handler();
    delay( 5 );
}*/

/*
*******************************************************************************
* Copyright (c) 2021 by M5Stack
*                  Equipped with M5Core2 sample source code
*                          配套  M5Core2 示例源代码
* Visit the website for more information：https://docs.m5stack.com/en/core/core2
* 获取更多资料请访问：https://docs.m5stack.com/zh_CN/core/core2
*
* describe：Display Example.  显示屏示例
* date：2021/7/21
*******************************************************************************
*/
#include <M5Core2.h>
#include <lvgl.h>

/* After M5Core2 is started or reset
the program in the setUp () function will be run, and this part will only be run once.
在 M5Core2 启动或者复位后，即会开始执行setup()函数中的程序，该部分只会执行一次。 */
void setup() {
  M5.begin(); //Init M5Core2.  初始化 M5Core2
  M5.Lcd.fillScreen(WHITE); // Set the screen background.  设置屏幕底色为白色
    lv_obj_t *label = lv_label_create( lv_scr_act() );
    lv_label_set_text( label, "Hello world!" );
    lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );
}

/* After the program in setup() runs, it runs the program in loop()
The loop() function is an infinite loop in which the program runs repeatedly
在setup()函数中的程序执行完后，会接着执行loop()函数中的程序
loop()函数是一个死循环，其中的程序会不断的重复运行 */
void loop(){


  M5.update();  //Read the press state of the key.  读取按键 A, B, C 的状态
    lv_timer_handler();
}