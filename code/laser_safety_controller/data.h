

#define SENSOR_TYPE_DIGITAL 0
#define SENSOR_TYPE_1WIRE   1
#define SENSOR_TYPE_ANALOG  2

#include <ctype.h>
#include <string>
#include <vector>
#include <lvgl.h>

#ifndef DATA_H
    #define DATA_H

class Sensor {
    public:
        std::string name = "";
        uint8_t pin = 0;
        int guid = 0;
        uint8_t type = SENSOR_TYPE_DIGITAL;
        int value = 0;
};

class Sensors {
    public:
        std::vector<Sensor> sensors;
        lv_obj_t *table;

        void add_sensor(std::string name, uint8_t pin, int guid, uint8_t type) {
            Sensor sensor;
            sensor.name = name;
            sensor.pin = pin;
            sensor.guid = guid;
            sensor.type = type;
            sensors.push_back(sensor);
        }

        void register_table(lv_obj_t *new_table) {
            table = new_table;
        }

        void update();

        void update_table();
        void update_values();
};

#endif