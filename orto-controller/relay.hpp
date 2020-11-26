#ifndef RELAY_H
#define RELAY_H

class Relay {
    public:
        Relay(unsigned int pinNumber, bool isNormallyClosed = false);
        void turnOn();
        void turnOff();
        bool isOn();
    private:
        unsigned int pinNumber;
        bool relayIsOpen;
        bool relayIsNormallyClosed;
};

#endif