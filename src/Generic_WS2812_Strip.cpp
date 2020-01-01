#include <Arduino.h>

#include "Generic_WS2812_Strip.h"

void Generic_WS2812_Strip::infoLight(RgbColor color) {
  // Flash the strip in the selected color. White = booted, green = WLAN connected, red = WLAN could not connect
  for (int i = 0; i < pixelCount; i++)
  {
    strip->SetPixelColor(i, color);
    strip->Show();
    delay(10);
    strip->SetPixelColor(i, black);
    strip->Show();
  }
}

void Generic_WS2812_Strip::showConnectionStatus(ESP8266WiFiClass WiFi){
  // TODO: maybe use array of Generic_Light
if (!lights[0].lightState) {
    infoLight(white);
    while (WiFi.status() != WL_CONNECTED) {
      infoLight(red);
      delay(500);
    }
    // Show that we are connected
    infoLight(green);
  }
}

void Generic_WS2812_Strip::initWebServer(){}
void Generic_WS2812_Strip::lightEngine(){}
void Generic_WS2812_Strip::processLightdata(float transitiontime){}
void Generic_WS2812_Strip::entertainment(){}
void Generic_WS2812_Strip::startup(){}