#include "data.h"

Sensors::Sensors(DallasTemperature* dt_bus) {
    this->dt_bus = dt_bus;
}

void Sensors::update_table(void) {
    // Iterate over the sensors and update the values displayed in the table.
    // for (Sensor s : sensors) {

    // }
}

void Sensors::update_values() {
    // Iterate over the sensors and update their values.
    dt_bus->requestTemperatures();
    for (int i = 0; i < sensors.size(); i++) {
        sensors[i].update();
    }
}

void Sensors::discover_new_sensors_on_bus() {
    // Scan the onewire bus report any found sensors.
    Serial.println("Scanning bus");

    dt_bus->requestTemperatures();
    uint8_t deviceCount=dt_bus->getDeviceCount();
    Serial.print("Found this many sensors: ");
    Serial.println(deviceCount);
    for (uint8_t i=0; i<deviceCount; i++) {
        DeviceAddress address;
        if (!dt_bus->getAddress(address, i)) {
            Serial.print("Detected a sensor on the OneWire bus but couldn't read it, for some reason.");
        }

        bool new_sensor = true;
        for (int j = 0; j < sensors.size(); j++) {
            if (memcmp(sensors[j].address, address, 8) == 0) {
                new_sensor = false;
                break;
            }
        }
        if (new_sensor) {
            // We don't have this sensor yet, add it
            Serial.print("Found new sensor with address: {");
            for (uint8_t j=0; j<8; j++) {
                Serial.print("0x");
                Serial.print(address[j], HEX);
                if (j < 7) {
                    Serial.print(",");
                }
            }
            Serial.println("}");
            Serial.println("Add this to the setup function.");
            unassigned_addresses.push_back(myDeviceAddress(address));
            add_onewire_sensor("Unknown", address);

        }
    }
}

void Sensors::add_onewire_sensor(std::string name, DeviceAddress address) {
    Sensor sensor(name, ONEWIRE_PIN, address, SENSOR_TYPE_ONEWIRE);

    sensor.dt_bus = dt_bus;

    // I think this copies the object. I also think I hate C++.
    sensors.push_back(sensor);
}


void Sensors::update() {
    update_table();
    update_values();
}

Sensor::Sensor(std::string name, uint8_t pin, DeviceAddress address, uint8_t type)
{
    this->name = name;
    this->pin = pin;
    memcpy(this->address, address, 8);
    this->type = type;
    if (type == SENSOR_TYPE_DIGITAL) {
        pinMode(pin, INPUT);
    } else if (type == SENSOR_TYPE_ANALOG) {
        pinMode(pin, INPUT);
    }
}

void Sensor::set_thresholds(int alarm_low, int warn_low, int warn_high, int alarm_high) {
    thresholds[SENSOR_STATE_ALARM_LOW_INDEX] = alarm_low;
    thresholds[SENSOR_STATE_WARN_LOW_INDEX] = warn_low;
    thresholds[SENSOR_STATE_WARN_HIGH_INDEX] = warn_high;
    thresholds[SENSOR_STATE_ALARM_HIGH_INDEX] = alarm_high;
}

void Sensor::update() {
    switch (type) {
        case SENSOR_TYPE_DIGITAL:
            Serial.println("Updating digital sensor");
            value = digitalRead(pin);
            read_error = false;
            break;
        case SENSOR_TYPE_ANALOG:
            Serial.println("Updating analogue sensor");
            value = analogRead(pin);
            read_error = false;
            break;
        case SENSOR_TYPE_ONEWIRE:
            value = dt_bus->getTempC(address);
            read_error = (value == DEVICE_DISCONNECTED_C);
            break;
        default:
            break;
    }

    if (read_error) {
        state = SENSOR_STATE_ALARM;
    } else {
        if (value < thresholds[SENSOR_STATE_ALARM_LOW_INDEX]) {
            state = SENSOR_STATE_ALARM;
        } else if (value < thresholds[SENSOR_STATE_WARN_LOW_INDEX]) {
            state = SENSOR_STATE_WARN;
        } else if (value > thresholds[SENSOR_STATE_ALARM_HIGH_INDEX]) {
            state = SENSOR_STATE_ALARM;
        } else if (value > thresholds[SENSOR_STATE_WARN_HIGH_INDEX]) {
            state = SENSOR_STATE_WARN;
        } else {
            state = SENSOR_STATE_NORMAL;
        }
    }
    if (alarm_pin != 0) {
        if ((state == SENSOR_STATE_ALARM) && digitalRead(alarm_pin) == LOW) {
            digitalWrite(alarm_pin, HIGH);
        } else if ((state == SENSOR_STATE_NORMAL) && digitalRead(alarm_pin) == HIGH) {
            digitalWrite(alarm_pin, LOW);
        }
    }
}

std::string Sensor::get_printable() {
    switch(type) {
        case SENSOR_TYPE_DIGITAL:
            if (value) {
                return "On";
            } else {
                return "Off";
            }
            break;
        case SENSOR_TYPE_ANALOG:
            return std::to_string(value);
            break;
        case SENSOR_TYPE_ONEWIRE:
            char buffer[32];
            sprintf(buffer, "%0.1f degC", value);
            return std::string(buffer);
            break;
        default:
            return "Unknown";
            break;
    }
}

void Sensor::set_alarm_pin(uint8_t pin) {
    alarm_pin = pin;
    pinMode(pin, OUTPUT);
}