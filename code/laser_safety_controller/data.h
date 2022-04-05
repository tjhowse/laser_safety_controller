

#define SENSOR_TYPE_DIGITAL 0
#define SENSOR_TYPE_ONEWIRE   1
#define SENSOR_TYPE_ANALOG  2

#define SENSOR_STATE_NORMAL 0
#define SENSOR_STATE_WARN 1
#define SENSOR_STATE_ALARM 2

#define SENSOR_STATE_ALARM_LOW_INDEX 0
#define SENSOR_STATE_WARN_LOW_INDEX 1
#define SENSOR_STATE_WARN_HIGH_INDEX 2
#define SENSOR_STATE_ALARM_HIGH_INDEX 3

#define ARDUINO 10813 // This tells imported libraries that we're running on a post-1.0 version of arduino.

#include <memory>
#include <ctype.h>
#include <string>
#include <vector>
#include <set>
#include <lvgl.h>
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#ifndef DATA_H
#define DATA_H

class OneWireBus {
    public:
        uint8_t pin;
        OneWire ow;
        DallasTemperature dt;

        OneWireBus(uint8_t newpin) {
            Serial.print("Starting a new onewire bus on pin ");
            Serial.println(newpin);
            pin = newpin;
            ow.begin(pin);
            dt.setOneWire(&ow);
            dt.begin();
            // dt.begin();
            // dt.requestTemperatures();
            // float tempC = dt.getTempCByIndex(0);
            // DeviceAddress address;
            // if (dt.getAddress(address, 0)) {
            //     for (uint8_t j=0; j<8; j++) {
            //         Serial.print(address[j], HEX);
            //         Serial.print(" ");
            //     }
            // }

            // // Check if reading was successful
            // if(tempC != DEVICE_DISCONNECTED_C)
            // {
            //     Serial.print("Temperature for the device 1 (index 0) is: ");
            //     Serial.println(tempC);
            // }
            // else
            // {
            //     Serial.println("Error: Could not read temperature data");
            // }
            // dt.begin();
            // this->dt.begin();
        };
};

class Sensor {
    public:
        std::string name = "Default";
        uint8_t pin = 0;
        uint8_t bus_index = 0;
        uint8_t type = SENSOR_TYPE_DIGITAL;
        float value = 0;
        DallasTemperature *dt_bus;
        // ALARM_LOW, WARN_LOW, WARN_HIGH, ALARM_HIGH
        int thresholds[4] = {1, 10, 50, 80};
        uint8_t state = SENSOR_STATE_NORMAL;

        Sensor(std::string name, uint8_t pin, uint8_t index, uint8_t type);
        void update();
};

class Sensors {
    public:
        std::vector<Sensor> sensors;
        lv_obj_t *table;
        std::vector<std::unique_ptr<OneWireBus>> busses;

        void add_sensor(std::string name, uint8_t pin, uint8_t index, uint8_t type);

        void register_table(lv_obj_t *new_table) {
            table = new_table;
        };

        void update();

        void update_table();
        void update_values();

        void discover_new_sensors_on_bus(uint8_t pin);

        uint8_t get_onewire_bus_index(uint8_t pin);


};

#endif