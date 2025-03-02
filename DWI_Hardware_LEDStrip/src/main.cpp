#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "WiFiManager.h"
#include "MQTTClient.h"

// Include the LEDStrip class
#include "LEDStrip.h"

#define LED_PIN 13     // The pin connected to the LED strip
#define LED_COUNT 30    // Total number of LEDs in the strip

LEDStrip strip(LED_COUNT, LED_PIN);

void setup() {
    strip.begin();
    strip.clear();
}

void loop() {
    std::vector<uint16_t> redPixels = {2, 5, 8, 12}; // Specific red pixels
    std::vector<uint16_t> greenPixels = {3, 6, 9, 15}; // Specific green pixels
    std::vector<uint16_t> bluePixels = {1, 4, 7, 10}; // Specific blue pixels

    strip.setRangeRed(redPixels);
    delay(1000);
    
    strip.setRangeGreen(greenPixels);
    delay(1000);

    strip.setRangeColor(bluePixels, 0, 0, 255); // Blue color for selected pixels
    delay(1000);
    
    strip.clear(); // Turn off all LEDs
    delay(1000);
}
