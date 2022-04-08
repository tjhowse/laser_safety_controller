

#define SENSOR_TYPE_DIGITAL 0
#define SENSOR_TYPE_ONEWIRE 1
#define SENSOR_TYPE_ANALOGUE 2

#define SENSOR_STATE_NORMAL 0
#define SENSOR_STATE_WARN 1
#define SENSOR_STATE_ALARM 2

#define SENSOR_STATE_ALARM_LOW_INDEX 0
#define SENSOR_STATE_WARN_LOW_INDEX 1
#define SENSOR_STATE_WARN_HIGH_INDEX 2
#define SENSOR_STATE_ALARM_HIGH_INDEX 3

#define ARDUINO 10813 // This tells imported libraries that we're running on a post-1.0 version of arduino.

#define ONEWIRE_PIN 19

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

class Sensor {
    public:
        std::string name = "Default";
        uint8_t pin = 0;
        DeviceAddress address;
        uint8_t type = SENSOR_TYPE_DIGITAL;
        float value = 0;
        DallasTemperature *dt_bus;
        // ALARM_LOW, WARN_LOW, WARN_HIGH, ALARM_HIGH
        int thresholds[4] = {1, 10, 50, 80};
        uint8_t state = SENSOR_STATE_NORMAL;
        bool read_error = false;
        uint8_t alarm_pin = 0;
        float scalar = 1.0f;
        std::string unit = "";


        Sensor(std::string name, uint8_t pin, DeviceAddress address, uint8_t type);
        void update();
        void set_thresholds(int alarm_low, int warn_low, int warn_high, int alarm_high);
        void set_alarm_pin(uint8_t pin);

        void set_unit(std::string unit);
        void set_scalar(float new_scalar);

        std::string get_printable();

};

class myDeviceAddress {
    public:
        DeviceAddress a;
        myDeviceAddress(DeviceAddress b) {
            memcpy(a, b, 8);
        }
};

class Sensors {
    public:
        std::vector<Sensor> sensors;
        lv_obj_t *table;
        lv_obj_t *unassigned_table;
        DallasTemperature* dt_bus;
        std::vector<myDeviceAddress> unassigned_addresses;

        void add_onewire_sensor(std::string name, DeviceAddress address);
        void add_digital_sensor(std::string name, uint8_t pin);
        void add_analogue_sensor(std::string name, uint8_t pin);

        void register_table(lv_obj_t *new_table) {
            table = new_table;
        };
        void register_unassigned_table(lv_obj_t *new_table) {
            unassigned_table = new_table;
        };

        Sensors(DallasTemperature* dt_bus);

        void update();

        void update_table();
        void update_values();

        void discover_new_sensors_on_bus();


};

#endif