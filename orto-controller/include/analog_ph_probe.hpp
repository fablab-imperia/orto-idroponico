#ifndef ANALOG_PH_PROBE
#define ANALOG_PH_PROBE
    #include <Arduino.h>

    class AnalogPhProbe {
        public:
            AnalogPhProbe(int analogPin, int numberOfSamplesToaverage=10);
            float getAveragePhValue();
        private:
            int analogPin = -1;
            int numberOfSamplesToaverage = -1;
    }; 
#endif