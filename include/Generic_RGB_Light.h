#pragma once

#include "Generic_Light.h"

#define PWM_CHANNELS 3

class Generic_RGB_Light : public Generic_Light
{
private:
    
public:
    char lightName[LIGHT_NAME_MAX_LENGTH] = "New Hue RGB light";

    uint8_t pins[PWM_CHANNELS] = {12, 13, 14}; //red, green, blue

    struct state {
        uint8_t colors[PWM_CHANNELS], bri = 100, sat = 254, colorMode = 2;
        bool lightState;
        int ct = 200, hue;
        float stepLevel[PWM_CHANNELS], currentColors[PWM_CHANNELS], x, y;
    };

    state light;

/*
    void convert_hue();
    void convert_xy();
    void convert_ct();
    void apply_scene(uint8_t new_scene);
    void saveState();
    void restoreState();
*/
    void showConnectionStatus(ESP8266WiFiClass WiFi);
    void initWebServer();
    void lightEngine();
    void processLightdata(float transitiontime);
    void entertainment();
    void startup();
};