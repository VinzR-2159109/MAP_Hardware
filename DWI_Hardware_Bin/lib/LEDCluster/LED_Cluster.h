#ifndef LED_Cluster_H
#define LED_Cluster_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

class LEDCluster {
public:
    LEDCluster(uint8_t numLEDs=7, uint8_t pin=9) : strip(numLEDs, pin, NEO_GRB + NEO_KHZ800), numLEDs(numLEDs) {}

    void begin() {
        strip.begin();
        strip.show();
    }
    
    void processMQTTMessage(const JsonDocument& doc) {
        uint8_t red = doc["color"]["r"] | 0;
        uint8_t green = doc["color"]["g"] | 0;
        uint8_t blue = doc["color"]["b"] | 0;
    
        uint8_t brightness = doc["brightness"] | 255;
        strip.setBrightness(brightness);
        
        String effect = doc["effect"] | "";
    
        Serial.printf("Received: R=%d, G=%d, B=%d, Brightness=%d, Effect=%s\n", red, green, blue, brightness, effect.c_str());
    
        if (effect == "flash") {
            uint16_t duration = doc["duration"] | 500;
            uint8_t cycles = doc["cycles"] | 3;
            Serial.println("🚥 Flashing LEDs");
            flashAll(red, green, blue, duration, cycles);
        } else {
            setColorAll(red, green, blue);
        }
    }
    
    

    void flash(uint8_t index, uint8_t red, uint8_t green, uint8_t blue, uint16_t duration, uint8_t cycles) {
        for (uint8_t i = 0; i < cycles; i++) {
            setColor(index, red, green, blue);
            delay(duration);
            clear();
            delay(duration);
        }
    }

    void flashAll(uint8_t red, uint8_t green, uint8_t blue, uint16_t duration, uint8_t cycles) {
        for (uint8_t i = 0; i < cycles; i++) {
            setColorAll(red, green, blue);
            delay(duration);
            clear();
            delay(duration);
        }
    }
    
    void setColorAll(uint8_t red, uint8_t green, uint8_t blue) {
        for (uint16_t i = 0; i < numLEDs; i++) {
            setColor(i, red, green, blue);
        }
    }

    void setColor(uint8_t index, uint8_t red, uint8_t green, uint8_t blue) {
        if (index < numLEDs) {
            strip.setPixelColor(index, strip.Color(red, green, blue));
            strip.show();
        }
    }

    void clear() {
        strip.clear();
        strip.show();
    }

private:
    uint8_t id;
    Adafruit_NeoPixel strip;
    uint16_t numLEDs;
};

#endif // LED_CONTROLLER_H
