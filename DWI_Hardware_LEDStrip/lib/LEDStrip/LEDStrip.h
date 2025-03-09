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

    void wave(uint16_t delayTime, uint8_t waveLength, uint8_t red, uint8_t green, uint8_t blue) {
        if (waveLength >= strip.numPixels()) return;
    
        // Forward motion
        for (uint16_t i = 0; i < strip.numPixels() + waveLength; i++) {
            clear();  // Clear the strip before each wave step
            for (uint16_t j = 0; j < waveLength; j++) {
                uint16_t pixelIndex = i + j;
                if (pixelIndex < strip.numPixels()) {
                    strip.setPixelColor(pixelIndex, strip.Color(red, green, blue));
                }
            }
            strip.show();
            delay(delayTime);
        }
    
        for (int i = strip.numPixels() + waveLength - 1; i >= 0; i--) {
            clear();
            for (uint16_t j = 0; j < waveLength; j++) {
                uint16_t pixelIndex = i + j;
                if (pixelIndex < strip.numPixels()) {
                    strip.setPixelColor(pixelIndex, strip.Color(red, green, blue));
                }
            }
            strip.show();
            delay(delayTime);
        }
    }
    
    void flash(uint16_t duration, uint8_t cycles, uint8_t red, uint8_t green, uint8_t blue) {
        for (uint8_t i = 0; i < cycles; i++) {
            for (uint16_t j = 0; j < strip.numPixels(); j++) {
                strip.setPixelColor(j, strip.Color(red, green, blue));
            }
            strip.show();
            delay(duration);
    
            clear();
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

    void show() {
        strip.show();
    }

    void processMQTTMessage(const JsonDocument& doc) {       
        if (!doc["leds"].is<JsonArrayConst>()) return;
    
        JsonArrayConst leds = doc["leds"].as<JsonArrayConst>();
        for (JsonObjectConst led : leds) {
            String rangeStr = led["range"];
            int startIdx, endIdx;
    
            sscanf(rangeStr.c_str(), "%d-%d", &startIdx, &endIdx);
    
            if (startIdx < 0 || endIdx >= strip.numPixels() || startIdx > endIdx) continue;
    
            uint8_t r = led["color"]["r"];
            uint8_t g = led["color"]["g"];
            uint8_t b = led["color"]["b"];
    
            uint8_t brightness = led["brightness"].is<uint8_t>() ? led["brightness"].as<uint8_t>() : 255;
            String status = led["status"].as<String>();
            
            Serial.printf("Range: %d-%d, Color: (%d, %d, %d), Brightness: %d, Status: %s\n", startIdx, endIdx, r, g, b, brightness, status.c_str());

            for (int i = startIdx; i <= endIdx; i++) {
                if (status == "on") {
                    uint8_t adj_r = (r * brightness) / 255;
                    uint8_t adj_g = (g * brightness) / 255;
                    uint8_t adj_b = (b * brightness) / 255;
                    strip.setPixelColor(i, strip.Color(adj_r, adj_g, adj_b));
                } else if (status == "off") {
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