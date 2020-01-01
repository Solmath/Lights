#pragma once

#include <NeoPixelBus.h>

#include "Generic_Light.h"

class Generic_WS2812_Strip : public Generic_Light
{

public:
    char lightName[LIGHT_NAME_MAX_LENGTH] = "Hue WS2812 strip";

    uint8_t lightsCount = 3;
    uint16_t pixelCount = 60;
    //uint16_t lightLedsCount;
    uint8_t transitionLeds = 6; // pixelCount must be divisible by this value

    struct state {
        uint8_t colors[3], bri = 100, sat = 254, colorMode = 2;
        bool lightState;
        int ct = 200, hue;
        float stepLevel[3], currentColors[3], x, y;
    };

    state lights[10]; // TODO: This will crash for lightsCount > 10

    RgbColor red = RgbColor(255, 0, 0);
    RgbColor green = RgbColor(0, 255, 0);
    RgbColor white = RgbColor(255);
    RgbColor black = RgbColor(0);

    NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1Ws2812xMethod>* strip = NULL;
    
    /*
    void convert_hue();
    void convert_xy();
    void convert_ct();
    void apply_scene(uint8_t new_scene);
    void saveState();
    void restoreState();
*/
    void infoLight(RgbColor color);
    void showConnectionStatus(ESP8266WiFiClass WiFi);
    void initWebServer();
    void lightEngine();
    void processLightdata(float transitiontime);
    void entertainment();
    void startup();
};