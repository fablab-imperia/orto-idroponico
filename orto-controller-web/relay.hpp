#ifndef RELAY_H
#define RELAY_H

class Relay {
  public:
    Relay(unsigned int pinNumber, bool switchesWhenHigh = false, bool isNormallyClosed = true);
    void init();
    void turnOn();
    void turnOff();
    void toggle();
    bool isOn();
  private:
    unsigned int pinNumber;
    bool relayIsOpen;
    bool relayIsNormallyClosed;
    bool switchesWhenHigh;
};

#endif
