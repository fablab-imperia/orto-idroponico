#include  <Arduino.h>
#include "analog_conductivity_probe.hpp"
#include "boards_pinouts.hpp"

AnalogConductivityProbe::AnalogConductivityProbe(int analogPin, int numberOfSamplesToAverage)  {
    this->analogPin = analogPin;
    this->numberOfSamplesToAverage = numberOfSamplesToAverage;
}

float AnalogConductivityProbe::getAverageConductivityValue() {
    unsigned int average = 0;
    for (int i=0; i < this->numberOfSamplesToAverage; i++)
    {
        int digitalValueFromADC = analogRead(this->analogPin);
        average +=  digitalValueFromADC;
    } 

    float averageValue = float(average)/ this->numberOfSamplesToAverage;

    float phVoltageValue = averageValue * ADC_MAX_VOLTAGE / ADC_MAX_VALUES;
    return phVoltageValue;
}