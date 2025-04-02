#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>
using namespace websockets;

class WebSocketClient {
public:
    WebSocketClient(void (*onMessageCallback)(const char*)) : ws_url(nullptr), messageCallback(onMessageCallback) {}

    void connect(const char* url) {
        ws_url = url;

        client.onMessage([this](WebsocketsMessage message) {
            this->onMessage(message);
        });

        client.onEvent([this](WebsocketsEvent event, String data) {
            this->onEvent(event, data);
        });

        client.connect(ws_url);
    }

    void sendMessage(const String& message) {
        if (client.available()) {
            client.send(message);
        }
    }

    void loop() {
        client.poll();
    }

    void processMQTTMessage(const JsonDocument& doc) {
        if (strcmp(doc["action"].as<const char*>(), "connect") == 0) {
            if (doc["data"].is<const char*>()) {
                const char* uri = doc["data"];
                Serial.print("🔌 WebSocket connect to: ");
                Serial.println(uri);
                connect(uri);
            } else {
                Serial.println("⚠️ Unknown or malformed WebSocket command");
            }
        }
    }

private:
    WebsocketsClient client;
    const char* ws_url;
    void (*messageCallback)(const char*);

    void onMessage(WebsocketsMessage message) {
        String msg = message.data();
        Serial.println("📨 Received from WebSocket: " + msg);
        if (messageCallback) {
            messageCallback(msg.c_str());
        }
    }

    void onEvent(WebsocketsEvent event, String data) {
        switch (event) {
            case WebsocketsEvent::ConnectionOpened:
                Serial.println("✅ WebSocket connected");
                break;
            case WebsocketsEvent::ConnectionClosed:
                Serial.println("❌ WebSocket disconnected");
                break;
            case WebsocketsEvent::GotPing:
                Serial.println("📡 Received Ping");
                break;
            case WebsocketsEvent::GotPong:
                Serial.println("📡 Received Pong");
                break;
        }
    }
};

#endif
