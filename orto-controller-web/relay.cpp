#include "Arduino.h"
#include "relay.hpp"

Relay::Relay(unsigned int pinNumber, bool swithcesWhenHigh, bool isNormallyClosed) {

  this->pinNumber = pinNumber;
  this->relayIsOpen = !isNormallyClosed;
  this->relayIsNormallyClosed = isNormallyClosed;
  this->switchesWhenHigh = switchesWhenHigh;
}

void Relay::init() {
  pinMode(this->pinNumber, OUTPUT);
}

void Relay::turnOn() {
  this->relayIsOpen = true;
  if (this->relayIsNormallyClosed) {
    digitalWrite(this->pinNumber, (this->switchesWhenHigh) ? LOW : HIGH);
  } else {
    digitalWrite(this->pinNumber, (this->switchesWhenHigh) ? HIGH : LOW);
  }
}

void Relay::turnOff() {
  this->relayIsOpen = false;
  if (this->relayIsNormallyClosed) {
    digitalWrite(this->pinNumber, (this->switchesWhenHigh) ? HIGH : LOW);
  } else {
    digitalWrite(this->pinNumber, (this->switchesWhenHigh) ? LOW : HIGH);
  }
}

bool Relay::isOn() {
  return this->relayIsOpen;
}

void Relay::toggle() {
  if (this->isOn()) {
    this->turnOff();
  } else {
    this->turnOn();
  }
}
