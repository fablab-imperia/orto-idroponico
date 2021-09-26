#include  <Arduino.h>
#include "analog_probe.hpp"
#include "parameters.hpp"

AnalogProbe::AnalogProbe(unsigned int channelNumber, unsigned int numberOfSamplesToAverage)  {
  this->channelNumber = channelNumber;
  this->numberOfSamplesToAverage = numberOfSamplesToAverage;
}

void AnalogProbe::initProbe(LineFit fit) {
  this->bestFit = fit;
}

AnalogValues AnalogProbe::getAverageValue(ADAFRUIT_ADC adc) {
  long average = 0;
  for (int i = 0; i < this->numberOfSamplesToAverage; i++)
  {
    int digitalValueFromADC = adc.readADC_SingleEnded(this->channelNumber);
    average +=  digitalValueFromADC;
  }

  float averageValue = float(average) / this->numberOfSamplesToAverage;

  float voltageValue = averageValue * ADC_MAX_VOLTAGE / ADC_MAX_VALUES;

  float realValue = this->bestFit.slope * voltageValue + this->bestFit.intercept;

  AnalogValues analogValue;
  analogValue.voltage = voltageValue;
  analogValue.value = realValue;

  return analogValue;
}
