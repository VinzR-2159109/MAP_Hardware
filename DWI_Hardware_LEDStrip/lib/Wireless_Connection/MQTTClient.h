#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <vector>

const char* root_ca = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFBjCCAu6gAwIBAgIRAIp9PhPWLzDvI4a9KQdrNPgwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjQwMzEzMDAwMDAw
WhcNMjcwMzEyMjM1OTU5WjAzMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg
RW5jcnlwdDEMMAoGA1UEAxMDUjExMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB
CgKCAQEAuoe8XBsAOcvKCs3UZxD5ATylTqVhyybKUvsVAbe5KPUoHu0nsyQYOWcJ
DAjs4DqwO3cOvfPlOVRBDE6uQdaZdN5R2+97/1i9qLcT9t4x1fJyyXJqC4N0lZxG
AGQUmfOx2SLZzaiSqhwmej/+71gFewiVgdtxD4774zEJuwm+UE1fj5F2PVqdnoPy
6cRms+EGZkNIGIBloDcYmpuEMpexsr3E+BUAnSeI++JjF5ZsmydnS8TbKF5pwnnw
SVzgJFDhxLyhBax7QG0AtMJBP6dYuC/FXJuluwme8f7rsIU5/agK70XEeOtlKsLP
Xzze41xNG/cLJyuqC0J3U095ah2H2QIDAQABo4H4MIH1MA4GA1UdDwEB/wQEAwIB
hjAdBgNVHSUEFjAUBggrBgEFBQcDAgYIKwYBBQUHAwEwEgYDVR0TAQH/BAgwBgEB
/wIBADAdBgNVHQ4EFgQUxc9GpOr0w8B6bJXELbBeki8m47kwHwYDVR0jBBgwFoAU
ebRZ5nu25eQBc4AIiMgaWPbpm24wMgYIKwYBBQUHAQEEJjAkMCIGCCsGAQUFBzAC
hhZodHRwOi8veDEuaS5sZW5jci5vcmcvMBMGA1UdIAQMMAowCAYGZ4EMAQIBMCcG
A1UdHwQgMB4wHKAaoBiGFmh0dHA6Ly94MS5jLmxlbmNyLm9yZy8wDQYJKoZIhvcN
AQELBQADggIBAE7iiV0KAxyQOND1H/lxXPjDj7I3iHpvsCUf7b632IYGjukJhM1y
v4Hz/MrPU0jtvfZpQtSlET41yBOykh0FX+ou1Nj4ScOt9ZmWnO8m2OG0JAtIIE38
01S0qcYhyOE2G/93ZCkXufBL713qzXnQv5C/viOykNpKqUgxdKlEC+Hi9i2DcaR1
e9KUwQUZRhy5j/PEdEglKg3l9dtD4tuTm7kZtB8v32oOjzHTYw+7KdzdZiw/sBtn
UfhBPORNuay4pJxmY/WrhSMdzFO2q3Gu3MUBcdo27goYKjL9CTF8j/Zz55yctUoV
aneCWs/ajUX+HypkBTA+c8LGDLnWO2NKq0YD/pnARkAnYGPfUDoHR9gVSp/qRx+Z
WghiDLZsMwhN1zjtSC0uBWiugF3vTNzYIEFfaPG7Ws3jDrAMMYebQ95JQ+HIBD/R
PBuHRTBpqKlyDnkSHDHYPiNX3adPoPAcgdF3H2/W0rmoswMWgTlLn1Wu0mrks7/q
pdWfS6PJ1jty80r2VKsM/Dj3YIDfbjXKdaFU5C+8bhfJGqU3taKauuz0wHVGT3eo
6FlWkWYtbt4pgdamlwVeZEW+LM7qZEJEsMNPrfC03APKmZsJgpWCDWOKZvkZcvjV
uYkQ4omYCTX5ohy+knMjdOmdH9c7SpqEWBDC86fiNex+O0XOMEZSa8DA
-----END CERTIFICATE-----
)EOF";

class MQTTClient {
public:
    using CallbackFunction = void (*)(char*, byte*, unsigned int);

    MQTTClient(const char* server, int port, const char* user, const char* pass, CallbackFunction cb)
        : mqtt_server(server), mqtt_port(port), mqtt_user(user), mqtt_pass(pass), client(espClient), userCallback(cb) {
        espClient.setCACert(root_ca);
        client.setServer(mqtt_server, mqtt_port);
        client.setCallback([this](char* topic, byte* payload, unsigned int length) {
            if (userCallback) userCallback(topic, payload, length);
        });
    }

    bool connect(int maxRetries = 10) {
        if (client.connected()) return true;

        Serial.println("🔄 Connecting to MQTT...");
        String clientName = "ESP32_" + String(ESP.getEfuseMac(), HEX);
        
        int attempts = 0;
        while (!client.connected() && attempts < maxRetries) {
            if (client.connect(clientName.c_str(), mqtt_user, mqtt_pass)) {
                Serial.println("✅ MQTT Connected!");
                return true;
            }
            Serial.printf("❌ MQTT Connection Failed (rc=%d), retrying...\n", client.state());
            delay(2000);
            attempts++;
        }

        Serial.println("❌ MQTT Connection failed after max retries.");
        return false;
    }

    bool reconnect() {
        if (client.connected()) return true;

        Serial.println("🔄 MQTT Reconnecting...");
        String clientName = "ESP32_" + String(ESP.getEfuseMac(), HEX);

        if (client.connect(clientName.c_str(), mqtt_user, mqtt_pass)) {
            Serial.println("✅ MQTT Reconnected!");
            resubscribe();
            return true;
        } else {
            Serial.printf("❌ MQTT Reconnect Failed (rc=%d)\n", client.state());
            return false;
        }
    }

    void subscribeTopic(const char* topic) {
        if (isTopicSubscribed(topic)) {
            Serial.println("🚫 Already subscribed to: " + String(topic));
            return;
        }

        subscribedTopics.push_back(topic);
        Serial.println("📩 Subscribing to: " + String(topic));
        client.subscribe(topic);
    }

    void resubscribe() {
        if (subscribedTopics.empty()) return;

        Serial.println("📩 Resubscribing to topics...");
        for (const auto& topic : subscribedTopics) {
            client.subscribe(topic.c_str());
            Serial.println("✅ Resubscribed to: " + topic);
        }
    }

    void publishMessage(const char* topic, const char* message) {
        Serial.print("📤 Publishing: " + String(message) + " to " + String(topic));
        if (!client.publish(topic, message)){
            Serial.println(" ❌ Message publish failed!");
        } else {
            Serial.println(" ✅ Message sent successfully!");
        }
    }

    void loop() {
        client.loop();
    }

    bool isConnected() {
        return client.connected();
    }

private:
    const char* mqtt_server;
    int mqtt_port;
    const char* mqtt_user;
    const char* mqtt_pass;
    WiFiClientSecure espClient;
    PubSubClient client;
    CallbackFunction userCallback;
    std::vector<String> subscribedTopics;

    bool isTopicSubscribed(const char* topic) {
        for (const auto& t : subscribedTopics) {
            if (t.equals(topic)) return true;
        }
        return false;
    }
};

#endif // MQTTCLIENT_H