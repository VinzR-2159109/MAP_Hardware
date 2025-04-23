#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <Adafruit_NeoPixel.h>
#include <vector>
#include <ArduinoJson.h>
#include <Arduino.h>

class LEDStrip {
public:
    LEDStrip(uint16_t numLEDs = 60, uint8_t pin = 13) : strip(numLEDs, pin, NEO_GRB + NEO_KHZ800) {}

    void begin() {
        strip.begin();
        strip.show();
    }

    void setListColors(const std::vector<uint8_t>& indices, uint8_t red, uint8_t green, uint8_t blue) {
        for (uint8_t idx : indices) {
            if (idx < strip.numPixels()) {
                strip.setPixelColor(idx, strip.Color(red, green, blue));
            }
        }
        strip.show();
    }

    void flashAll(uint16_t duration, uint8_t cycles, uint8_t red, uint8_t green, uint8_t blue) {
        for (uint8_t i = 0; i < cycles; i++) {
            for (uint16_t j = 0; j < strip.numPixels(); j++) {
                strip.setPixelColor(j, strip.Color(red, green, blue));
            }
            strip.show();
            delay(duration);

            for (uint16_t j = 0; j < strip.numPixels(); j++) {
                strip.setPixelColor(j, 0);
            }
            strip.show();
            delay(duration);
        }
    }

    void clear() {
        for (uint16_t i = 0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, 0);
        }
        strip.show();
    }

    void processMQTTMessage(const JsonDocument& doc) {
        if (!doc["list"].is<JsonArrayConst>()) {
            Serial.println("❌ 'list' is not an array or missing");
            return;
        }

        JsonArrayConst list = doc["list"].as<JsonArrayConst>();
        for (JsonObjectConst config : list) {
            if (!config["list_indices"].is<JsonArrayConst>()) {
                Serial.println("❌ Missing or invalid 'list_indices'");
                return;
            }

            std::vector<uint8_t> indices;
            for (JsonVariantConst val : config["list_indices"].as<JsonArrayConst>()) {
                int index = val.as<int>();
                if (index >= 0 && index < strip.numPixels()) {
                    indices.push_back(static_cast<uint8_t>(index));
                }
            }

            if (indices.empty()) {
                Serial.println("❌ No valid indices provided");
                return;
            }

            String status = config["status"].as<String>();
            uint8_t r = config["color"]["r"];
            uint8_t g = config["color"]["g"];
            uint8_t b = config["color"]["b"];
            uint8_t brightness = config["brightness"].is<uint8_t>() ? config["brightness"].as<uint8_t>() : 255;
            if (brightness > 255) brightness = 255;

            uint8_t adj_r = (r * brightness) / 255;
            uint8_t adj_g = (g * brightness) / 255;
            uint8_t adj_b = (b * brightness) / 255;

            if (status == "on") {
                setListColors(indices, adj_r, adj_g, adj_b);
            } else if (status == "off") {
                setListColors(indices, 0, 0, 0);
            } else {
                Serial.println("❌ Unknown status: " + status);
            }
        }
    }

private:
    Adafruit_NeoPixel strip;
};

#endif
