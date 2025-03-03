#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "WiFiManager.h"
#include "MQTTClient.h"
#include "LEDStrip.h"
#include <ArduinoJson.h>

// WiFi and MQTT Credentials
const char* SSID = "Hotspot van Vinz";
const char* PASSWORD = "Vinz1512.";
const char* MQTT_SERVER = "0f158df0574242429e54c7458f9f4a37.s1.eu.hivemq.cloud";
const int MQTT_PORT = 8883;
const char* MQTT_USERNAME = "dwi_map";
const char* MQTT_PASSWORD = "wRYx&RK%l5vsflnN";

const char* id = "x";

void messageCallback(char* topic, byte* payload, unsigned int length);

WiFiManager wifiManager(SSID, PASSWORD);
MQTTClient mqttClient(MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD, messageCallback);
LEDStrip ledStrip(60, 13);

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

    if (!doc["id"].is<const char*>() || strcmp(doc["id"].as<const char*>(), id) != 0) {
        Serial.println("⛔ Ignoring message: ID mismatch");
        return;
    }  

    ledStrip.processMQTTMessage(doc);
    doc.clear();
}

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(1000);

    wifiManager.connectWiFi();

    mqttClient.connect();
    mqttClient.subscribeTopic("Output/LEDStrip");
}


void loop() {
    mqttClient.loop();

    delay(100);
}