#ifndef ANALOG_PROBE_HPP
#define ANALOG_PROBE_HPP

#include "line_fit.hpp"
#include "parameters.hpp"

#define NUMBER_OF_CALIBRATION_POINTS 3

struct AnalogValues {
  float voltage;
  float value;
};

class AnalogProbe {
  public:
    AnalogProbe(unsigned int analogPin, unsigned int numberOfSamplesToAverage);
    void initProbe(LineFit fit);
    AnalogValues getAverageValue(ADAFRUIT_ADC);
  private:
    int channelNumber = -1;
    int numberOfSamplesToAverage = -1;
    LineFit bestFit;
};
#endif
