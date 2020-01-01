#include <Arduino.h>

#include <WiFiUdp.h>

#include "Generic_RGB_Light.h"

void Generic_RGB_Light::entertainment()
{
  byte packetBuffer[8];
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    if (!entertainmentRun) {
      entertainmentRun = true;
    }
    lastEPMillis = millis();
    Udp.read(packetBuffer, packetSize);
    for (uint8_t color = 0; color < PWM_CHANNELS; color++) {
      light.currentColors[color] = packetBuffer[color + 1];
      analogWrite(pins[color], (int)(packetBuffer[color + 1] * 4));
    }
  }
}

void Generic_RGB_Light::showConnectionStatus(ESP8266WiFiClass WiFi){
  if (!light.lightState)  {
    while (WiFi.status() != WL_CONNECTED) {
      analogWrite(pins[0], 100);
      delay(500);
      analogWrite(pins[0], 0);
    }
    // Show that we are connected
    analogWrite(pins[1], 100);
    delay(500);
    analogWrite(pins[1], 0);
  }
}

void Generic_RGB_Light::initWebServer(){}
void Generic_RGB_Light::lightEngine(){}
void Generic_RGB_Light::processLightdata(float transitiontime){}
void Generic_RGB_Light::startup(){}
