#include <Arduino.h>

#include <FS.h>
#include <SPI.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

#include "Generic_RGB_Light.h"
#include "Generic_WS2812_Strip.h"

#ifdef GENERIC_RGB_LIGHT
Generic_RGB_Light Light;
// uint8_t pins[] = {12, 13, 14}; //red, green, blue
#endif
#ifdef GENERIC_WS2812_STRIP
Generic_WS2812_Strip Light;
#endif

//settings
bool useDhcp = true;
uint8_t scene = 0, startup = false;
bool hwSwitch = false;
uint8_t onPin = 4, offPin = 5; // hwSwitches are connected to this pins
uint8_t rgb_multiplier[] = {100, 100, 100}; // light multiplier in percentage /R, G, B/

IPAddress address ( 192,  168,   0,  95); // choose an unique IP Adress
IPAddress gateway ( 192,  168,   0,   1); // Router IP
IPAddress submask (255, 255, 255,   0);

byte mac[6];

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

bool saveConfig() {
  DynamicJsonDocument json(1024);
  json["name"] = Light.lightName;
  json["startup"] = startup;
  json["scene"] = scene;
  #ifdef GENERIC_RGB_LIGHT
  json["r"] = Light.pins[0];
  json["g"] = Light.pins[1];
  json["b"] = Light.pins[2];
  #endif
  #ifdef GENERIC_WS2812_STRIP
  json["lightsCount"] = Light.lightsCount;
  json["pixelCount"] = Light.pixelCount;
  json["transLeds"] = Light.transitionLeds;
  #endif
  json["rpct"] = rgb_multiplier[0];
  json["gpct"] = rgb_multiplier[1];
  json["bpct"] = rgb_multiplier[2];
  json["on"] = onPin;
  json["off"] = offPin;
  json["hw"] = hwSwitch;
  json["dhcp"] = useDhcp;
  JsonArray addr = json.createNestedArray("addr");
  addr.add(address[0]);
  addr.add(address[1]);
  addr.add(address[2]);
  addr.add(address[3]);
  JsonArray gw = json.createNestedArray("gw");
  gw.add(gateway[0]);
  gw.add(gateway[1]);
  gw.add(gateway[2]);
  gw.add(gateway[3]);
  JsonArray mask = json.createNestedArray("mask");
  mask.add(submask[0]);
  mask.add(submask[1]);
  mask.add(submask[2]);
  mask.add(submask[3]);
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    //Serial.println("Failed to open config file for writing");
    return false;
  }

  serializeJson(json, configFile);
  return true;
}

bool loadConfig() {
    File configFile = SPIFFS.open("/config.json", "r");
    if (!configFile) {
        //Serial.println("Create new file with default values");
        return saveConfig();
    }

    if (configFile.size() > 1024) {
        Serial.println("Config file size is too large");
        return false;
    }

    DynamicJsonDocument json(1024);
    DeserializationError error = deserializeJson(json, configFile.readString());

    if (error) {
        //Serial.println("Failed to parse config file");
        return false;
    }

    strcpy(Light.lightName, json["name"]);
    startup = (uint8_t) json["startup"];
    scene  = (uint8_t) json["scene"];
    #ifdef GENERIC_RGB_LIGHT
        Light.pins[0] = (uint8_t) json["r"];
        Light.pins[1] = (uint8_t) json["g"];
        Light.pins[2] = (uint8_t) json["b"];
    #endif
    if (json.containsKey("rpct")){
        rgb_multiplier[0] = (uint8_t) json["rpct"];
        rgb_multiplier[1] = (uint8_t) json["gpct"];
        rgb_multiplier[2] = (uint8_t) json["bpct"];
    }
    onPin = (uint8_t) json["on"];
    offPin = (uint8_t) json["off"];
    hwSwitch = json["hw"];
    useDhcp = json["dhcp"];
    address = {json["addr"][0], json["addr"][1], json["addr"][2], json["addr"][3]};
    submask = {json["mask"][0], json["mask"][1], json["mask"][2], json["mask"][3]};
    gateway = {json["gw"][0], json["gw"][1], json["gw"][2], json["gw"][3]};

    return true;
}

void setup() {
    // begin serial
    Serial.begin(115200);
    while (!Serial) continue;

    //Serial.println("mounting FS...");

    if (!SPIFFS.begin()) {
        //Serial.println("Failed to mount file system");
    return;
    }

    if (!loadConfig()) {
        //Serial.println("Failed to load config");
    } else {
        //Serial.println("Config loaded");
    }

    Light.startup();

    // connect to WiFi
    Serial.println("----------------------------");
    Serial.println("Connecting to wifi");
    Serial.println("----------------------------");

    WiFiManager wifiManager;

    if (!useDhcp) {
        wifiManager.setSTAStaticIPConfig(address, gateway, submask);
    }

    if (!wifiManager.autoConnect(Light.lightName)) {
        delay(3000);
        ESP.reset();
        delay(5000);
    }
    
    if (useDhcp) {
        address = WiFi.localIP();
        gateway = WiFi.gatewayIP();
        submask = WiFi.subnetMask();
    }

    Serial.println("----------------------------");
    Serial.println("WiFi connected");
    Serial.println("----------------------------");

    // OTA needed? see ESP8266HueEmulator
    // Update via web interface is already possible (httpUpdater)

    Light.showConnectionStatus(WiFi);

    WiFi.macAddress(mac);

    httpUpdater.setup(&server);

    Light.Udp.begin(2100);

    pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
    
    if (hwSwitch == true) {
        pinMode(onPin, INPUT);
        pinMode(offPin, INPUT);
    }

    Light.initWebServer();
}

void loop() {
    server.handleClient();
    if (!Light.entertainmentRun) {
        Light.lightEngine();
    } else {
        if ((millis() - Light.lastEPMillis) >= ENTERTAINMENT_TIMEOUT) {
            Light.entertainmentRun = false;
            Light.processLightdata(4); //return to original colors with 0.4 sec transition
        }
    }
    Light.entertainment();
}