#include <M5Core2.h>

#include "AXP192.h"
#include <Arduino.h>
#include <lvgl.h>
#include <Wire.h>
#include <SPI.h>
// #include "touch.h"
#include "data.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define TABLE_UPDATE_INTERVAL_MS 500

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];


OneWire oneWire(ONEWIRE_PIN);
DallasTemperature dallas(&oneWire);
Sensors sensors(&dallas);

unsigned long table_update_deadline_ms;

#include "gui.h"

/*Read the touchpad*/
bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
    TouchPoint_t pos = M5.Touch.getPressPoint();
    bool touched = ( pos.x == -1 ) ? false : true;

    if(!touched) {
      data->state = LV_INDEV_STATE_REL;
    } else {
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = pos.x;
      data->point.y = pos.y;
    }

    return false; /*Return `false` because we are not buffering and no more data to read*/
}

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors(&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

void setup()
{
    M5.begin(true, true, true, true);
    tft.begin(); /* TFT init */
    tft.setRotation(1);         /* Landscape orientation */
    M5.Axp.SetLcdVoltage(2800);
    M5.Axp.SetLcdVoltage(3300);
    M5.Axp.SetBusPowerMode(0);
    M5.Axp.SetCHGCurrent(AXP192::kCHG_190mA);
    M5.Axp.SetLDOEnable(3, true);
    delay(150);
    M5.Axp.SetLDOEnable(3, false);
    M5.Axp.SetLed(1);
    delay(100);
    M5.Axp.SetLed(0);
    M5.Axp.SetLDOVoltage(3, 3300);
    M5.Axp.SetLed(1);

    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);
    lv_init();

    /*Initialize the display*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 320;
    disp_drv.ver_res = 240;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the (dummy) input device driver*/
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
    Serial.println("Setting up gui");
    setup_gui();

    dallas.begin();
    dallas.begin();

    // Here is where we define all the sensors we know and care about
    // TODO fix this silly way of defining addresses.
    {
      DeviceAddress newAddress = {0x28,0xED,0xA2,0x79,0x97,0x14,0x3,0x91};
      sensors.add_onewire_sensor("Coolant Resevoir", newAddress);
      sensors.sensors.back().set_thresholds(1,5,20,30);
      sensors.sensors.back().set_alarm_pin(27);

    }
    {
      DeviceAddress newAddress = {0x28,0x37,0x43,0x79,0x97,0x14,0x3,0x8D};
      sensors.add_onewire_sensor("Compressor", newAddress);
      sensors.sensors.back().set_thresholds(1,5,50,60);
    }
    sensors.discover_new_sensors_on_bus();
    sensors.update();
    update_sensor_table_display();
}

void loop()
{
  sensors.update();
  lv_task_handler(); /* let the GUI do its work */

  if (table_update_deadline_ms < millis()) {
    update_sensor_table_display();
    table_update_deadline_ms = millis() + TABLE_UPDATE_INTERVAL_MS;
  }

  delay(5);
}