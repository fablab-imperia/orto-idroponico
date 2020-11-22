#include  <Arduino.h>
#include "analog_ph_probe.hpp"
#include "boards_pinouts.hpp"

AnalogPhProbe::AnalogPhProbe(int analogPin, int numberOfSamplesToAverage)  {
    this->analogPin = analogPin;
    this->numberOfSamplesToAverage = numberOfSamplesToAverage;
    this->bestFit = findBestFit();
}

float AnalogPhProbe::getAveragePhValue() {
    unsigned int average = 0;
    for (int i=0; i < this->numberOfSamplesToAverage; i++)
    {
        int digitalValueFromADC = analogRead(this->analogPin);
        average +=  digitalValueFromADC;
        delay (20);
    } 

    float averageValue = float(average)/ this->numberOfSamplesToAverage;

    float phVoltageValue = averageValue * ADC_MAX_VOLTAGE / ADC_MAX_VALUES;
    return phVoltageValue;
    //return (this->bestFit.slope * phVoltageValue + this->bestFit.intercept);
}

// best fit formula taken from https://www.varsitytutors.com/hotmath/hotmath_help/topics/line-of-best-fit
BestLineFitParameters AnalogPhProbe::findBestFit() {
    //Find mean values for X and Y
    float meanX = 0.0f;
    float meanY = 0.0f;

    for (unsigned int i = 0; i < NUMBER_OF_CALIBRATION_POINTS; i++)
    {
        meanX += this->calibrationPoints[i].voltage;
        meanY += this->calibrationPoints[i].phValue;
    }
    
    meanX = meanX / NUMBER_OF_CALIBRATION_POINTS;
    meanY = meanY / NUMBER_OF_CALIBRATION_POINTS;

    float sumProdOfDiffsXy = 0.0f;
    float sumOfDiffsXSquared = 0.0f;

    for (unsigned int i = 0; i < NUMBER_OF_CALIBRATION_POINTS; i++)
    {
        float diffFromX = this->calibrationPoints[i].voltage - meanX;
        float diffFromY = this->calibrationPoints[i].phValue - meanY;
        float prodDiffXY = diffFromX * diffFromY;
        float diffFromXSquared = diffFromX * diffFromX;
        sumProdOfDiffsXy += prodDiffXY;
        sumOfDiffsXSquared += diffFromXSquared;
    }

    BestLineFitParameters params;

    params.slope = sumProdOfDiffsXy / sumOfDiffsXSquared;
    params.intercept = meanY - params.slope * meanX;

    return params;

}