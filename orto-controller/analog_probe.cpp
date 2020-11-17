#include  <Arduino.h>
#include "analog_probe.hpp"
#include "boards_pinouts.hpp"

AnalogProbe::AnalogProbe(int analogPin, int numberOfSamplesToAverage)  {
    this->analogPin = analogPin;
    this->numberOfSamplesToAverage = numberOfSamplesToAverage;
}

void AnalogProbe::initProbe(LineFit fit) {
    this->bestFit = fit;
}

float AnalogProbe::getAverageValue() {
    unsigned int average = 0;
    for (int i=0; i < this->numberOfSamplesToAverage; i++)
    {
        int digitalValueFromADC = analogRead(this->analogPin);
        average +=  digitalValueFromADC;
    } 

    float averageValue = float(average)/ this->numberOfSamplesToAverage;

    float phVoltageValue = averageValue * ADC_MAX_VOLTAGE / ADC_MAX_VALUES;

    return (this->bestFit.slope * phVoltageValue + this->bestFit.intercept);
}

