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
            Serial.println("Detected a sensor on the OneWire bus but couldn't read it, for some reason.");
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

Sensor* Sensors::getSensorByName(std::string name) {
    // Iterate over the sensors and return the one with the matching name.
    for (int i = 0; i < sensors.size(); i++) {
        if (sensors[i].name == name) {
            return &sensors[i];
        }
    }
    return NULL;
}

void Sensors::add_onewire_sensor(std::string name, DeviceAddress address) {
    Sensor sensor(name, ONEWIRE_PIN, address, SENSOR_TYPE_ONEWIRE);

    sensor.dt_bus = dt_bus;
    sensor.set_unit("degC");

    // I think this copies the object. I also think I hate C++.
    sensors.push_back(sensor);
}

void Sensors::add_analogue_sensor(std::string name, uint8_t pin) {
    Sensor sensor(name, pin, NULL, SENSOR_TYPE_ANALOGUE);

    // I think this copies the object. I also think I hate C++.
    sensors.push_back(sensor);
}

void Sensors::add_digital_sensor(std::string name, uint8_t pin) {
    Sensor sensor(name, pin, NULL, SENSOR_TYPE_DIGITAL);

    // I think this copies the object. I also think I hate C++.
    sensors.push_back(sensor);
}

void Sensors::add_digital_output(std::string name, uint8_t pin) {
    Sensor sensor(name, pin, NULL, SENSOR_TYPE_DIGITAL_OUTPUT);

    // I think this copies the object. I also think I hate C++.
    sensors.push_back(sensor);
}


void Sensors::add_encoder_sensor(std::string name, uint8_t pin) {
    Sensor sensor(name, pin, NULL, SENSOR_TYPE_ENCODER);

    // I think this copies the object. I also think I hate C++.
    sensors.push_back(sensor);
}

void Sensors::update() {
    update_table();
    update_values();
    update_logic();
}

void Sensors::update_logic() {
    // This is handwritten logic to set up things like driving relay outputs
    // on the basis of sensor states. It is very possible to build a structure
    // around this, but it would be more complex than just writing things out here.

    Sensor* reservoir_temp = getSensorByName("Coolant Reservoir");
    Sensor* compressor_control = getSensorByName("Compressor Control");
    Sensor* compressor_1_temp = getSensorByName("Compressor 1");
    Sensor* laser_control = getSensorByName("Laser Control");
    Sensor* coolant_flow = getSensorByName("Coolant Flow");


    if ((reservoir_temp == NULL) ||
        (compressor_control == NULL) ||
        (compressor_1_temp == NULL) ||
        (coolant_flow == NULL) ||
        (laser_control == NULL)) {
        laser_control->set_value(0);
        compressor_control->set_value(0);
        Serial.println("Error: one or more sensors not found. Cannot run logic.");
        // set_debug("Sensor null");
        return;
    }

    // Laser control
    if (!reservoir_temp->error &&
        !coolant_flow->error &&
        (reservoir_temp->state <= normal) &&
        (coolant_flow->state >= normal)) {
        laser_control->set_value(1);
    } else {
        laser_control->set_value(0);
    }

    if (!compressor_1_temp->error &&
        !reservoir_temp->error &&
        !coolant_flow->error &&
        (compressor_1_temp->state <= normal) &&
        (reservoir_temp->state >= normal) &&
        (coolant_flow->state >= normal)) {
        compressor_control->set_value(1);
    } else {
        compressor_control->set_value(0);
    }
}

Sensor::Sensor(std::string name, uint8_t pin, DeviceAddress address, uint8_t type)
{
    this->name = name;
    this->pin = pin;
    if (address != NULL) {
        memcpy(this->address, address, 8);
    }
    this->type = type;
    if (type == SENSOR_TYPE_DIGITAL) {
        pinMode(pin, INPUT);
        set_thresholds(-2,-1,2,3);
    } else if (type == SENSOR_TYPE_ANALOGUE) {
        pinMode(pin, INPUT);
    } else if (type == SENSOR_TYPE_DIGITAL_OUTPUT) {
        pinMode(pin, OUTPUT);
        set_thresholds(-2,-1,2,3);
    } else if (type == SENSOR_TYPE_ENCODER) {
        encoder = new ESP32Encoder();
        ESP32Encoder::useInternalWeakPullResistors=DOWN;

        // TODO confirm that providing pin 1 here doesn't cause problems.
        encoder->attachSingleEdge(pin, 1);
        encoder->clearCount();
    }
}

void Sensor::set_thresholds(float alarm_low, float warn_low, float warn_high, float alarm_high) {
    thresholds[SENSOR_STATE_ALARM_LOW_INDEX] = alarm_low;
    thresholds[SENSOR_STATE_WARN_LOW_INDEX] = warn_low;
    thresholds[SENSOR_STATE_WARN_HIGH_INDEX] = warn_high;
    thresholds[SENSOR_STATE_ALARM_HIGH_INDEX] = alarm_high;
}

void Sensor::set_unit(std::string new_unit) {
    unit = new_unit;
}


void Sensor::set_scalar(float new_scalar) {
    scalar = new_scalar;
}

void Sensor::set_offset(float new_offset) {
    // Note the offset is applied to the raw ADC reading, not the scaled one.
    offset = new_offset;
}

void Sensor::set_value(float new_value) {
    value = new_value;
    if (type == SENSOR_TYPE_DIGITAL_OUTPUT) {
        if (value > 0.5) {
            // Active low outputs.
            digitalWrite(pin, LOW);
        } else {
            digitalWrite(pin, HIGH);
        }
    }
}

void Sensor::update() {
    bool read_error;
    if (type == SENSOR_TYPE_DIGITAL ) {
        value = digitalRead(pin);
        read_error = false;
        last_update_ms = millis();
    } else if (type == SENSOR_TYPE_ANALOGUE) {
        value = (analogRead(pin)+offset) * scalar;
        read_error = false;
        last_update_ms = millis();
    } else if (type == SENSOR_TYPE_ONEWIRE) {
        float prev_value = value;
        value = dt_bus->getTempC(address);
        if (value == DEVICE_DISCONNECTED_C) {
            value = prev_value;
            read_error = true;
        } else {
            read_error = false;
        }
        last_update_ms = millis();
    } else if (type == SENSOR_TYPE_ENCODER) {
        int32_t temp = -encoder->getCount();
        if ((temp > 10) || ((millis()-last_update_ms) > SENSOR_ENCODER_TIMEOUT_MS)) {
            // Rescale the raw counts to counts/second.
            value = temp/((millis()-last_update_ms)/1000);
            value *= scalar;
            encoder->clearCount();
            last_update_ms = millis();
        }
        read_error = false;
    }

    if (!read_error) {
        error = false;
        error_deadline_ms = millis() + SENSOR_ERROR_TIMEOUT_MS;
        if (value < thresholds[SENSOR_STATE_ALARM_LOW_INDEX]) {
            state = low_alarm;
        } else if (value < thresholds[SENSOR_STATE_WARN_LOW_INDEX]) {
            state = low_warn;
        } else if (value > thresholds[SENSOR_STATE_ALARM_HIGH_INDEX]) {
            state = high_alarm;
        } else if (value > thresholds[SENSOR_STATE_WARN_HIGH_INDEX]) {
            state = high_warn;
        } else {
            state = normal;
        }
    } else {
        if (millis() > error_deadline_ms) {
            error = true;
        }
    }
}

std::string Sensor::get_printable() {
    char buffer[32];
    if (type == SENSOR_TYPE_DIGITAL) {
        if (value) {
            return "On";
        } else {
            return "Off";
        }
    } else if (type == SENSOR_TYPE_ANALOGUE) {
        sprintf(buffer, "%0.1f %s", value, unit.c_str());
        return std::string(buffer);
    } else if (type == SENSOR_TYPE_ONEWIRE) {
        sprintf(buffer, "%0.1f %s", value, unit.c_str());
        return std::string(buffer);
    } else if (type == SENSOR_TYPE_DIGITAL_OUTPUT) {
        if (value) {
            return "On";
        } else {
            return "Off";
        }
    } else if (type == SENSOR_TYPE_ENCODER) {
        sprintf(buffer, "%0.1f %s", value, unit.c_str());
        return std::string(buffer);
    }
    return "Unknown";
}
