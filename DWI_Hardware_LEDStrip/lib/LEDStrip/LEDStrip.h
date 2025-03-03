#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <Adafruit_NeoPixel.h>
#include <vector>
#include <ArduinoJson.h>
#include <Arduino.h>
class LEDStrip {
public:
    LEDStrip(uint16_t numLEDs=60, uint8_t pin=13) : strip(numLEDs, pin, NEO_GRB + NEO_KHZ800) {}

    void begin() {
        strip.begin();
        strip.show();
    }

    void setRangeColor(const std::vector<uint16_t>& pixels, uint8_t red, uint8_t green, uint8_t blue) {
        for (uint16_t pixel : pixels) {
            if (pixel < strip.numPixels()) {
                strip.setPixelColor(pixel, strip.Color(red, green, blue));
            }
        }
        strip.show();
    }

    void setRangeRed(const std::vector<uint16_t>& pixels) {
        setRangeColor(pixels, 255, 0, 0); // Red color
    }

    void setRangeGreen(const std::vector<uint16_t>& pixels) {
        setRangeColor(pixels, 0, 255, 0); // Green color
    }

    void clear() {
        for (uint16_t i = 0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, 0);
        }
        strip.show();
    }

    void show() {
        strip.show();
    }

    void processMQTTMessage(const JsonDocument& doc) {
        Serial.println(doc.as<String>());
        
        if (!doc["leds"].is<JsonArrayConst>()) return;
    
        JsonArrayConst leds = doc["leds"].as<JsonArrayConst>();
        for (JsonObjectConst led : leds) {
            String rangeStr = led["range"];
            int startIdx, endIdx;
    
            sscanf(rangeStr.c_str(), "%d-%d", &startIdx, &endIdx);
    
            if (startIdx < 0 || endIdx >= strip.numPixels() || startIdx > endIdx) continue;
    
            uint8_t r = led["color"]["r"] | 0;
            uint8_t g = led["color"]["g"] | 0;
            uint8_t b = led["color"]["b"] | 0;
    
            uint8_t brightness = led["brightness"].is<uint8_t>() ? led["brightness"].as<uint8_t>() : 255;
            String status = led["status"].is<String>() ? led["status"].as<String>() : "on";
    
            for (int i = startIdx; i <= endIdx; i++) {
                if (status == "on") {
                    uint8_t adj_r = (r * brightness) / 255;
                    uint8_t adj_g = (g * brightness) / 255;
                    uint8_t adj_b = (b * brightness) / 255;
                    strip.setPixelColor(i, strip.Color(adj_r, adj_g, adj_b));
                } else {
                    strip.setPixelColor(i, 0);
                }
            }
        }
        
        strip.show();
    }
  
private:
    Adafruit_NeoPixel strip;
};



#endif
