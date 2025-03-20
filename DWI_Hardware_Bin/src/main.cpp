#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

// LCD-configuratie (adres 0x27 of 0x3F afhankelijk van het model, 16x2 formaat)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    lcd.init();        // Initialiseer het LCD-scherm
    lcd.backlight();   // Zet de achtergrondverlichting aan
    lcd.setCursor(0, 0);
    lcd.print("Hallo!");
    lcd.setCursor(0, 1);
    lcd.print("Arduino Uno :)");
    delay(2000);  // Wacht 2 sec om de boodschap te tonen
}

void loop() {
    lcd.clear();  // Wis het scherm

    // Toon een teller op het LCD
    for (int i = 0; i <= 10; i++) {
        lcd.setCursor(0, 0);
        lcd.print("Teller: ");
        lcd.print(i);

        lcd.setCursor(0, 1);
        lcd.print("Tik Tok...");
        
        delay(1000);  // Wacht 1 seconde
    }
}
