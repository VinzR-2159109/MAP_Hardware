#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <WiFi.h>

class WiFiManager {
public:
    WiFiManager(const char* ssid, const char* password) : ssid(ssid), password(password) {}

    bool connectWiFi() {
        Serial.print("🔄 Connecting to WiFi...");
        WiFi.mode(WIFI_STA);
        WiFi.setAutoReconnect(true);
        WiFi.persistent(true);
        WiFi.begin(ssid, password);

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n✅ Connected! IP: " + WiFi.localIP().toString());
            return true;
        } else {
            Serial.println("\n❌ Connection Failed!");
            return false;
        }
    }

    wl_status_t getStatus() {
        return WiFi.status();
    }

private:
    const char* ssid;
    const char* password;
};

#endif // WIFIMANAGER_H
