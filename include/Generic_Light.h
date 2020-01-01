#pragma once

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define LIGHT_VERSION 3.1
#define LIGHT_NAME_MAX_LENGTH 32 // Longer name will get stripped
#define ENTERTAINMENT_TIMEOUT 1500 // millis

class Generic_Light
{
private:

public:
    WiFiUDP Udp;

    unsigned long lastEPMillis;
    bool entertainmentRun;

    void convert_hue();
    void convert_xy();
    void convert_ct();
    void apply_scene(uint8_t new_scene);
    void saveState();
    void restoreState();
};