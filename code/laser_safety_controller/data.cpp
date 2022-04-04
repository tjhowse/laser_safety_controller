#include "data.h"

void Sensors::update_table(void) {
    // Iterate over the sensors and update the values displayed in the table.
    // for (Sensor s : sensors) {

    // }
}

void Sensors::update_values() {
    // Iterate over the sensors and update their values.

    Serial.println("About to request temperatures");
    for (auto b : busses) {
        b.dt.requestTemperatures();
    }

    Serial.println("About to update sensors");
    for (Sensor s : sensors) {
        s.update();
    }
}

void Sensors::discover_new_sensors_on_bus(uint8_t pin) {
    // Scan the onewire bus report any found sensors.
    auto bus = &(busses[get_onewire_bus_index(pin)].dt);
    std::vector<DeviceAddress> addresses;
    Serial.println("Scanning bus");

    uint8_t deviceCount=bus->getDeviceCount();
    Serial.print("Found this many sensors: ");
    Serial.println(deviceCount);
    bus->requestTemperatures();
    for (uint8_t i=0; i<deviceCount; i++) {
        DeviceAddress address;
        bus->getAddress(address, i);
        bool new_sensor = true;
        for (auto s : sensors) {
            if (s.pin != pin) continue;
            if (s.address == address) {
                // We already have this sensor
                new_sensor = false;
                continue;
            }
        }
        if (new_sensor) {
            // We don't have this sensor yet, add it
            Serial.print("Found new sensor on pin ");
            Serial.print(pin);
            Serial.print(" at address ");
            for (int j=0; j < 8; j++) {
                Serial.print(address[j], HEX);
                Serial.print(" ");
            }
            Serial.println();
            Serial.print(" on bus ");
            Serial.println((uint32_t)bus);
            add_sensor("New Sensor", pin, address, SENSOR_TYPE_ONEWIRE);
        }
    }
}

uint8_t Sensors::get_onewire_bus_index(uint8_t pin) {
    for (uint8_t i=0; i<busses.size(); i++) {
        if (busses[i].pin == pin) {
            return i;
        }
    }
    busses.push_back(OneWireBus(pin));
    return busses.size()-1;
}

void Sensors::add_sensor(std::string name, uint8_t pin, DeviceAddress address, uint8_t type) {
    Sensor sensor(name, pin, address, type);

    if (type == SENSOR_TYPE_ONEWIRE) {
        sensor.dt_bus = &(busses[get_onewire_bus_index(pin)].dt);
    }

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
    memcpy(this->address, address, sizeof(DeviceAddress));
    this->type = type;
    if (type == SENSOR_TYPE_DIGITAL) {
        pinMode(pin, INPUT);
    } else if (type == SENSOR_TYPE_ANALOG) {
        pinMode(pin, INPUT);
    }
}

void Sensor::update() {
    switch (type) {
        case SENSOR_TYPE_DIGITAL:
            Serial.println("Updating digital sensor");
            value = digitalRead(pin);
            break;
        case SENSOR_TYPE_ANALOG:
            Serial.println("Updating analogue sensor");
            value = analogRead(pin);
            break;
        case SENSOR_TYPE_ONEWIRE:
            // TODO Handle an error state if this sensor isn't visible
            // on this bus anymore
            Serial.print("Updating onewire sensor on bus ");
            Serial.println((uint32_t)dt_bus);
            value = dt_bus->getTempC(address);
            break;
        default:
            break;
    }
    if (value < thresholds[SENSOR_STATE_ALARM_LOW_INDEX]) {
        state = SENSOR_STATE_ALARM;
    } else if (value < thresholds[SENSOR_STATE_WARN_LOW_INDEX]) {
        state = SENSOR_STATE_WARN;
    } else if (value > thresholds[SENSOR_STATE_WARN_HIGH_INDEX]) {
        state = SENSOR_STATE_WARN;
    } else if (value > thresholds[SENSOR_STATE_ALARM_HIGH_INDEX]) {
        state = SENSOR_STATE_ALARM;
    } else {
        state = SENSOR_STATE_NORMAL;
    }
}
