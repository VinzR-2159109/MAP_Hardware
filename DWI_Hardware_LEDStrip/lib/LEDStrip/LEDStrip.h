#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <Adafruit_NeoPixel.h>
#include <vector>

class LEDStrip {
public:
    LEDStrip(uint16_t numLEDs, uint8_t pin) 
        : strip(numLEDs, pin, NEO_GRB + NEO_KHZ800) {}

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

private:
    Adafruit_NeoPixel strip;
};



#endif
