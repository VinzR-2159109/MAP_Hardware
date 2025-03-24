#ifndef SevenSegment_Display
#define SevenSegment_Display

#include <Arduino.h>
#include <ArduinoJson.h>
#include <TM1637Display.h>

class SevenSegmentDisplay {
private:
    TM1637Display display;

public:
    SevenSegmentDisplay(uint8_t clkPin, uint8_t dioPin) : display(clkPin, dioPin) {}

    void init(uint8_t brightness = 7) {
        display.setBrightness(brightness);
        display.clear();
    }

    void showNumber(int num) {
        Serial.printf("Displaying: %d\n", num);
        display.showNumberDec(num, false);
    }

    void turnOff() {
        Serial.println("Turning off display");
        display.setBrightness(0);
        display.clear();
    }

    void processMQTTMessage(const JsonDocument& doc) {   
        uint8_t brightness = doc["brightness"] | 7;
        init(brightness);
            
        Serial.printf("Received: Number=%d\n", doc["count"].as<int>());
        uint8_t quantity = doc["quantity"] | 9999;
    
        const char* status = doc["status"] | "on";

        if ( strcmp(status, "off") == 0 ) {
            turnOff();
        } else if (strcmp(status, "on") == 0) {
            showNumber(quantity);
        }

    }
};

#endif
