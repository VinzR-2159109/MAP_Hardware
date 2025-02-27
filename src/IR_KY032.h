#ifndef IR_KY032_H
#define IR_KY032_H

#include <Arduino.h>

class IR_KY032 {
private:
    int outPin;
    int enPin;

public:
    IR_KY032(int outPin=5, int enPin=6) {
        this->outPin = outPin;
        this->enPin = enPin;
        pinMode(outPin, INPUT);
        pinMode(enPin, OUTPUT);
    }
    
    //Remove jumper to use this:
    void enable() {
        digitalWrite(enPin, HIGH);
    }

    void disable() {
        digitalWrite(enPin, LOW);
    }

    bool isObstacleDetected() {
        return digitalRead(outPin) == LOW;
    }
};

#endif
