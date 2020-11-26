#include "Arduino.h"
#include "relay.hpp"

Relay::Relay(unsigned int pinNumber, bool isNormallyClosed) {
    pinMode(this->pinNumber, OUTPUT);
    this->pinNumber = pinNumber;
    this->relayIsOpen = !isNormallyClosed;
    this->relayIsNormallyClosed = isNormallyClosed;
}

void Relay::turnOn() {
    this->relayIsOpen = true;
    if (this->relayIsNormallyClosed) {
        digitalWrite(this->pinNumber, LOW);
    } else {
        digitalWrite(this->pinNumber, HIGH);
    }
}

void Relay::turnOff() {
    this->relayIsOpen = false;
    if (this->relayIsNormallyClosed) {
        digitalWrite(this->pinNumber, HIGH);
    } else {
        digitalWrite(this->pinNumber, LOW);
    }
}

bool Relay::isOn() {
    return this->relayIsOpen;
}