#ifndef ANALOG_PROBE_HPP
#define ANALOG_PROBE_HPP

#include "line_fit.hpp"

#define NUMBER_OF_CALIBRATION_POINTS 3

struct AnalogValues {
    float voltage;
    float value;
};

class AnalogProbe {
    public:
        AnalogProbe(unsigned int analogPin, unsigned int numberOfSamplesToAverage=10);
        void initProbe(LineFit fit);
        AnalogValues getAverageValue();
    private:
        int analogPin = -1;
        int numberOfSamplesToAverage = -1;
        LineFit bestFit;
}; 
#endif