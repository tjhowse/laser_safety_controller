#include <M5Core2.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "AXP192.h"
#include <Arduino.h>
#include <lvgl.h>
#include <Wire.h>
#include <SPI.h>
// #include "touch.h"
#include "data.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "sound.h"

//#include "secrets.h"
#include "secrets_real.h" // I haven't checked this in.

#define TABLE_UPDATE_INTERVAL_MS 500

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];


OneWire oneWire(ONEWIRE_PIN);
DallasTemperature dallas(&oneWire);
Sensors sensors(&dallas);

unsigned long table_update_deadline_ms;

WiFiClient espClient;
PubSubClient mqtt(espClient);

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

bool init_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);
  M5.Lcd.printf("Connecting to WIFI SSID: %s\n\n", wifi_ssid);
  unsigned long deadline_ms = millis() + 5000;
  while ((WiFi.status() != WL_CONNECTED) && (millis() < deadline_ms)) {
    delay(10);
  }
  if (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.println("\nFailed to connect to WIFI");
    M5.Lcd.println("\nCheck secrets.h");
    delay(2000);
    return false;
  } else {
    M5.Lcd.printf("\nConnected to WIFI");
    delay(200);
    return true;
  }
}

bool init_mqtt() {
  mqtt.setServer(mqtt_hostname, 1883);
  if (!mqtt.connect("LaserSafetyController", mqtt_username, mqtt_password)) {
    M5.Lcd.println("\nFailed to connect to MQTT");
    delay(2000);
    return false;
  } else {
    M5.Lcd.println("\nConnected to MQTT");
    mqtt.publish("laser/status", "online");
    delay(200);
    return true;
  }
  return true;
}

bool check_mqtt() {
  if (!mqtt.connected()) {
    if (!init_mqtt()) {
      return false;
    }
  }
  return true;
}

void setup()
{
    // Gotta turn off I2CEnable for pins 32 and 33 to work. ¯\_(ツ)_/¯
    M5.begin(true, true, true, false);

    tft.begin(); /* TFT init */
    tft.setRotation(1);         /* Landscape orientation */
    M5.Axp.SetLcdVoltage(2800);
    M5.Axp.SetLcdVoltage(3300);
    M5.Axp.SetBusPowerMode(0);
    M5.Axp.SetCHGCurrent(AXP192::kCHG_190mA);
    // M5.Axp.SetLDOEnable(3, true);
    // delay(150);
    // M5.Axp.SetLDOEnable(3, false);
    M5.Axp.SetLed(1);
    delay(100);
    M5.Axp.SetLed(0);
    M5.Axp.SetLDOVoltage(3, 3300);
    M5.Axp.SetLed(1);

    if (init_wifi()) {
      init_mqtt();
    }

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

    Serial.println("Setting up sensors");
    // Here is where we define all the sensors we know and care about
    // TODO fix this silly way of defining addresses.

    {
      DeviceAddress newAddress = {0x28,0xED,0xA2,0x79,0x97,0x14,0x3,0x91};
      sensors.add_onewire_sensor("Coolant Reservoir", newAddress);
      sensors.sensors.back().set_thresholds(10,15,20,25);
    }
    {
      DeviceAddress newAddress = {0x28,0x37,0x43,0x79,0x97,0x14,0x3,0x8D};
      sensors.add_onewire_sensor("Compressor 1", newAddress);
      sensors.sensors.back().set_thresholds(1,5,50,60);
    }
    {
      DeviceAddress newAddress = {0x28,0x16,0xA2,0x79,0x97,0x14,0x3,0x1B};
      sensors.add_onewire_sensor("Compressor 2", newAddress);
      sensors.sensors.back().set_thresholds(1,5,50,60);
    }
    // The following pins are input-only:
    //     ENSOR_VP (GPIO36), SENSOR_CAPP (GPIO37),
    // SENSOR_CAPN (GPIO38), SENSOR_VN (GPIO39), VDET_1 (GPIO34), VDET_2 (GPIO35).
    {
      sensors.add_digital_output("Compressor Control", 33);
    }
    {
      sensors.add_digital_output("Laser Control", 32);
      // We want 0 to be an error state for the laser.
      sensors.sensors.back().set_thresholds(1,1,2,3);
    }
    {
      sensors.add_analogue_sensor("Cooler Current", 36);
      sensors.sensors.back().set_thresholds(-1,-1,8,12);
      sensors.sensors.back().set_unit("Amps");
      // Sensor provides 66mV/A
      // Grr. I now realise the ACS712 won't run at 3.3v.
      //  0 amps, 2.5V
      // 30 amps, 2.5V + 30*0.066: 4.48v
      // Needs a voltage divider to bring it from 0-5v to 0-3.3v
      // I'm using a 5:3 ratio, so 4.48v is 2.688v, is
      // sensors.sensors.back().set_scalar(30/((4095/3.3)*1.98));
      // Measured:
      // 0A, 1776 ADC
      // 0.69A, 1740 ADC
      // 3.3A, 1609 ADC
      float scalar = -3.3/(1776-1609);
      float offset = -1776;
      sensors.sensors.back().set_offset(offset);
      sensors.sensors.back().set_scalar(scalar);

      //

    }
    {
      sensors.add_encoder_sensor("Coolant Flow", 35);
      // TODO change this unit when I add a scalar.
      sensors.sensors.back().set_thresholds(0.1,0.5,3,4);
      sensors.sensors.back().set_scalar(1/4.8);
      sensors.sensors.back().set_unit("L/min");
    }
    // water flow sensor:
    // https://www.ebay.com.au/itm/234183826875
    // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/pcnt.html
    // https://lochnerweb.de/frequency-measurement-on-esp32
    // https://github.com/DevX8000/ESP32-PCNT-Arduino-Example/blob/master/src/main.cpp
    // https://github.com/madhephaestus/ESP32Encoder/

    // {
    //   DeviceAddress newAddress = {0x28,0xFF,0x68,0x3E,0x82,0x16,0x5,0x6E};
    //   sensors.add_onewire_sensor("Ambient", newAddress);
    //   sensors.sensors.back().set_thresholds(1,5,30,60);
    // }
    sensors.discover_new_sensors_on_bus();
    sensors.update();
    sensors.set_mqtt_client(&mqtt);
    update_sensor_table_display();
    Serial.println("All done setting up sensors.");
    // Serial.println("Setting up the speaker.");

    // SpeakInit();
    // DingDong();

}

void loop()
{
  // TODO Uncomment this once it is made non-blocking. Reporting values via WIFI is a nice-to-have.
  // If it blocks the safety loop we don't want to bother reconnecting.
  // check_mqtt();
  sensors.update();
  lv_task_handler(); /* let the GUI do its work */

  if (table_update_deadline_ms < millis()) {
    update_sensor_table_display();
    table_update_deadline_ms = millis() + TABLE_UPDATE_INTERVAL_MS;
  }

  delay(5);
}