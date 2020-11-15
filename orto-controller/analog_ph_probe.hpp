#ifndef ANALOG_PH_PROBE
#define ANALOG_PH_PROBE

#define NUMBER_OF_CALIBRATION_POINTS 3


    struct PhVoltagePair {
        float phValue;
        float voltage;
    };

    struct BestLineFitParameters {
        float slope;
        float intercept;
    };


    class AnalogPhProbe {
        public:
            AnalogPhProbe(int analogPin, int numberOfSamplesToAverage=10);
            float getAveragePhValue();
        private:
            const PhVoltagePair calibrationPoints[NUMBER_OF_CALIBRATION_POINTS] = { {6.88f, 1.80f}, {4.00f, 1.46f}, {9.23f, 2.13f}};
            BestLineFitParameters findBestFit();
            int analogPin = -1;
            int numberOfSamplesToAverage = -1;
            BestLineFitParameters bestFit;
    }; 
#endif