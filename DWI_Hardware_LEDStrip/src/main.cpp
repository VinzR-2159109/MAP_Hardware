#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

#include "WiFiManager.h"
#include "MQTTClient.h"
#include "LEDStrip.h"

// WiFi and MQTT Credentials
const char* SSID = "Hotspot van Vinz";
const char* PASSWORD = "Vinz1512.";
const char* MQTT_SERVER = "0f158df0574242429e54c7458f9f4a37.s1.eu.hivemq.cloud";
const int MQTT_PORT = 8883;
const char* MQTT_USERNAME = "dwi_map";
const char* MQTT_PASSWORD = "wRYx&RK%l5vsflnN";

void messageCallback(char* topic, byte* payload, unsigned int length);

WiFiManager wifiManager(SSID, PASSWORD);
MQTTClient mqttClient(MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD, messageCallback);
LEDStrip ledStripX(60, 14);
LEDStrip ledStripY(29, 12);

void messageCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("📩 Received message on topic: ");
    Serial.println(topic);

    String message = "";
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println("📜 Payload: \n" + message);

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
        Serial.println("❌ Failed to parse JSON");
        return;
    }

    if (!doc["id"].is<const char*>() || strcmp(doc["id"].as<const char*>(), "x") == 0) {
        ledStripX.processMQTTMessage(doc);
        return;
    } 
    
    if (!doc["id"].is<const char*>() || strcmp(doc["id"].as<const char*>(), "y") == 0) {
        ledStripY.processMQTTMessage(doc);
        return;
    }  

    doc.clear();
}

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(1000);
    
    wifiManager.connectWiFi();

    ledStripY.flash(500, 2, 0, 255, 0); // 2xGreen

    mqttClient.connect();
    mqttClient.subscribeTopic("Output/LEDStrip");

    ledStripY.flash(500, 2, 0, 255, 0); // 2xGreen
}



void loop() {
    mqttClient.loop();
    
    if (wifiManager.getStatus() != WL_CONNECTED) {
        ledStripY.flash(500, 2, 255, 0, 0); // 2xRed
        Serial.println("❌ WiFi Disconnected!");
        return;
    }

    if (!mqttClient.isConnected()){
        ledStripY.flash(500, 2, 255, 0, 0); // 2xRed
        Serial.println("❌ MQTT Disconnected!");
        mqttClient.reconnect();
        return;
    }

    delay(10);
}
