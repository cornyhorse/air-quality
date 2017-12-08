#include "AQManager.hpp"

#include "OWTemp.hpp"
OWTemp ow(9);

#ifdef BME680
# else // BME280 + IAQ
#include <Adafruit_BME280.h>
#include "ams_iaq.h"

Adafruit_BME280 bme(10, 11, 12, 13);
Iaq iaq;
Iaq* gas = &iaq;
#endif

#define APIN_MQ135 A0
#define APIN_LIGHT A1

AQManager::AQManager() {
    pinMode(APIN_MQ135, INPUT);
    pinMode(APIN_LIGHT, INPUT);
    analogReadAveraging(32);
}

void AQManager::init() {
    bme.init();
    ow.search();
}

void AQManager::read() {
    bme.read();
    readAnalogSensors();
    ds_temperature = ow.get();
}

void AQManager::readAnalogSensors() {
#ifdef MQ135
    analog_mq135 = analogRead(APIN_MQ135);
#endif
    analog_light = analogRead(APIN_LIGHT);
}

const char* AQManager::format(int counter) {
    line[0] = '\0';
    snprintf(buf, BUFSZ, "cnt=%05u", counter);
    strlcat(line, buf, LINESZ);

    if (bme.valid()) {
        snprintf(buf, BUFSZ, ",temp=%04.2f,humi=%05.3f,pres=%07.2f",
                bme.temperature(),
                bme.humidity(),
                bme.pressure());
        strlcat(line, buf, LINESZ);
    }

    if (bme.valid()) {
        snprintf(buf, BUFSZ, ",gasr=%05.0f", bme.gasresistance());
        strlcat(line, buf, LINESZ);
    }

    if (analog_mq135) {
        snprintf(buf, BUFSZ, ",amq=%04u", analog_mq135);
        strlcat(line, buf, LINESZ);
    }

    if (analog_light) {
        snprintf(buf, BUFSZ, ",ali=%04u", analog_light);
        strlcat(line, buf, LINESZ);
    }

    if (ds_temperature) {
        snprintf(buf, BUFSZ, ",dst=%04.2f", ds_temperature);
        strlcat(line, buf, LINESZ);
    }

    // Calculate checksum for transmission
    unsigned sum = 0;
    for (size_t i = 0; i < strlen(line); i++) {
        sum += line[i] * (i+1);
    }
    sum = 0xFF & sum;

    snprintf(buf, BUFSZ, ",chk=%03u\r\n", sum);
    strlcat(line, buf, LINESZ);

    return &line[0];
}
