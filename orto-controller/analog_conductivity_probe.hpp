#ifndef ANALOG_CONDUCTIVITY_PROBE
#define ANALOG_CONDUCTIVITY_PROBE

    class AnalogConductivityProbe {
        public:
            AnalogConductivityProbe(int analogPin, int numberOfSamplesToAverage=10);
            float getAverageConductivityValue();
        private:
            int analogPin = -1;
            int numberOfSamplesToAverage = -1;
    }; 
#endif