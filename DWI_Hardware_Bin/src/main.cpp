#include <Arduino.h>
#include <ArduinoJson.h>

#include "WiFiManager.h"
#include "MQTTClient.h"
#include "IR_KY032.h"
#include "LED_Cluster.h"
#include "SevenSegment_Display.h"

// WiFi and MQTT Credentials
const char* SSID = "Hotspot van Vinz";
const char* PASSWORD = "Vinz1512.";
const char* MQTT_SERVER = "0f158df0574242429e54c7458f9f4a37.s1.eu.hivemq.cloud";
const int MQTT_PORT = 8883;
const char* MQTT_USERNAME = "dwi_map";
const char* MQTT_PASSWORD = "wRYx&RK%l5vsflnN";

const uint8_t id = 2;

bool lastObstacleState = false;

void messageCallback(char* topic, byte* payload, unsigned int length);

WiFiManager wifiManager(SSID, PASSWORD);
MQTTClient mqttClient(MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD, messageCallback);
IR_KY032 obstacleSensor(11, 6); // outPin=11, enPin=6
LEDCluster ledCluster(7, 9);   // numLEDs=7, pin=9
SevenSegmentDisplay display(13, 12); // clkPin=13, dioPin=12

void messageCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("📩 Received message on topic: ");
    Serial.println(topic);

    String message = "";
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println("📜 Payload: " + message);

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
        Serial.println("❌ Failed to parse JSON");
        return;
    }

    if (!doc["id"].is<int>() || doc["id"].as<int>() != id) {
        Serial.println("⛔ Ignoring message: ID mismatch");
        return;
    }

    if (strcmp(topic, "Output/Bin/LED") == 0){
        ledCluster.processMQTTMessage(doc);
    } 

    else if (strcmp(topic, "Output/Bin/Display") == 0){
        display.processMQTTMessage(doc);
    }

    else {
        Serial.println("⛔ Ignoring message: Unknown topic");
    }
}

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(1000);

    // Connect to Wi-Fi (Blocking)
    wifiManager.connectWiFi();

    ledCluster.flashAll(0, 255, 0, 500, 2); // 2x Green

    mqttClient.connect();
    mqttClient.subscribeTopic("Output/Bin/LED");
    mqttClient.subscribeTopic("Output/Bin/Display");
    ledCluster.flashAll(0, 255, 0, 500, 2); // 2x Green
}


void loop() {
    mqttClient.loop(); // Remember when you forgot this and it took 3u to find out

    if (wifiManager.getStatus() != WL_CONNECTED) {
        ledCluster.flashAll(255, 0, 0, 500, 3); // 3xRed
        Serial.println("❌ WiFi Disconnected!");
        return;
    }

    if (!mqttClient.isConnected()){
        ledCluster.flashAll(255, 0, 0, 500, 3); // 3xRed
        Serial.println("❌ MQTT Disconnected!");
        mqttClient.reconnect();
        return;
    }

    bool obstacleDetected = obstacleSensor.isObstacleDetected();
    if (obstacleDetected != lastObstacleState) {
        lastObstacleState = obstacleDetected;

        JsonDocument doc;
        doc["id"] = id;
        doc["obstacle"] = obstacleDetected;

        char buffer[128];
        serializeJson(doc, buffer);
        mqttClient.publishMessage("Input/Bin/Obstacle", buffer);
    }

    delay(100); // Small delay to reduce CPU usage
}