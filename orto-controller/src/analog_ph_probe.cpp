#include "analog_ph_probe.hpp"
#include "boards_pinouts.hpp"

AnalogPhProbe::AnalogPhProbe(int analogPin, int numberOfSamplesToAverage)  {
    this->analogPin = analogPin;
    this->numberOfSamplesToaverage = numberOfSamplesToaverage;
  

}

float AnalogPhProbe::getAveragePhValue() {
    unsigned int average = 0;
    for (int i=0; i < numberOfSamplesToaverage; i++)
    {
        int digitalValueFromADC = analogRead(analogPin);
        Serial.println(digitalValueFromADC);
        average +=  digitalValueFromADC;
    }
//erial.println(average);
    float averageValue = float(average) / numberOfSamplesToaverage;

    float phVoltageValue = averageValue * ADC_MAX_VOLTAGE / ADC_MAX_VALUES;
    return phVoltageValue;
}