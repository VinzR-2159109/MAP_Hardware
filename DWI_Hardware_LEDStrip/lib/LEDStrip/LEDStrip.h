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

    void setRangeColor(uint8_t start, uint8_t end, uint8_t red, uint8_t green, uint8_t blue) {
        for (uint8_t pixel = start; pixel <= end; pixel++) {
            if (pixel < strip.numPixels()) {
                strip.setPixelColor(pixel, strip.Color(red, green, blue));
            }
        }
        strip.show();
    }
    
    void flashAll(uint16_t duration, uint8_t cycles, uint8_t red, uint8_t green, uint8_t blue) {
        flashRange(0, strip.numPixels() - 1, duration, cycles, red, green, blue);
    }
    
    void flashRange(uint8_t start, uint8_t end, uint16_t duration, uint8_t cycles, uint8_t red, uint8_t green, uint8_t blue) {
        if (start > end || end >= strip.numPixels()) return;
    
        for (uint8_t i = 0; i < cycles; i++) {
            for (uint8_t j = start; j <= end; j++) {
                strip.setPixelColor(j, strip.Color(red, green, blue));
            }
            strip.show();
            delay(duration);
    
            for (uint8_t j = start; j <= end; j++) {
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
            String rangeStr = config["range"];
            int startIdx, endIdx;
            sscanf(rangeStr.c_str(), "%d-%d", &startIdx, &endIdx);
    
            if (startIdx < 0 || endIdx >= strip.numPixels() || startIdx > endIdx) return;
    
            uint8_t r = config["color"]["r"];
            uint8_t g = config["color"]["g"];
            uint8_t b = config["color"]["b"];
    
            uint8_t brightness = config["brightness"].is<uint8_t>() ? config["brightness"].as<uint8_t>() : 255;
            if (brightness > 255) brightness = 255;
    
            String status = config["status"].as<String>();
       
            if (status == "on") {
                uint8_t adj_r = (r * brightness) / 255;
                uint8_t adj_g = (g * brightness) / 255;
                uint8_t adj_b = (b * brightness) / 255;
                setRangeColor(startIdx, endIdx, adj_r, adj_g, adj_b);
            } 
            else if (status == "off") {
                setRangeColor(startIdx, endIdx, 0, 0, 0);
            } 
            else if (status == "flash") {
                if (!config["duration"].is<uint32_t>()) {
                    Serial.println("❌ Invalid duration for flash");
                    return;
                }
    
                if (!config["cycles"].is<uint8_t>()) {
                    Serial.println("❌ Invalid cycles for flash");
                    return;
                }
    
                uint16_t duration = config["duration"].as<uint32_t>();
                uint8_t cycles = config["cycles"].as<uint8_t>();
                flashRange(startIdx, endIdx, duration, cycles, r, g, b);
            }   
        }         
    }
    
  
private:
    Adafruit_NeoPixel strip;
};



#endif