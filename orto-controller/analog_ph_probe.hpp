#ifndef ANALOG_PH_PROBE
#define ANALOG_PH_PROBE

    class AnalogPhProbe {
        public:
            AnalogPhProbe(int analogPin, int numberOfSamplesToAverage=10);
            float getAveragePhValue();
        private:
            int analogPin = -1;
            int numberOfSamplesToAverage = -1;
    }; 
#endif