/*
 * Simple air quality monitor node
 *
 * Using the following sensors:
 * - BME280 (temperature, humidity, pressure)
 * - iAQ-core (CO2, TVOC)
 * Or alternatively:
 * - BME680 (temperature, humidity, pressure, gas)
 *
 * Required libs:
 * - duff2013/Snooze
 * - DFRobot/DFRobot_BME680
 * - adafruit/Adafruit_Sensor
 * - adafruit/Adafruit_BME280_Library
 *
 * Data is read periodically by a Teensy 3.2 and then printed over serial
 * console to USB and HM-11 BLE module.
 */

#include "AQManager.hpp"

#define SER1 Serial1 // Serial port 1
#define SERU Serial  // USB Serial

#ifndef SERU
#include <Snooze.h>
SnoozeTimer timer;
SnoozeBlock config(timer);
#endif

AQManager aq;
int counter = 0;

void setup() {
    SER1.begin(9600);
#ifdef SERU
    SERU.begin(9600);
#endif
    delay(2000);
    aq.init();
}

void loop() {
    aq.read();

    const char* buf = aq.format(counter++);
    SER1.print(buf);

    // Refresh interval
#ifdef SERU
    SERU.print(buf);
    delay(500);
#else
    timer.setTimer(500);
    Snooze.deepSleep(config);
#endif
}
