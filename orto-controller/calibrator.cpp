#include "calibrator.hpp"

// best fit formula taken from https://www.varsitytutors.com/hotmath/hotmath_help/topics/line-of-best-fit
LineFit Calibrator::findBestFit(XYPair calibrationPoints[], unsigned int numberOfPoints) {
    //Find mean values for X and Y
    float meanX = 0.0f;
    float meanY = 0.0f;

    for (unsigned int i = 0; i < numberOfPoints; i++)
    {
        meanX += calibrationPoints[i].X;
        meanY += calibrationPoints[i].Y;
    }
    
    meanX = meanX / numberOfPoints;
    meanY = meanY / numberOfPoints;

    float sumProdOfDiffsXy = 0.0f;
    float sumOfDiffsXSquared = 0.0f;

    for (unsigned int i = 0; i < numberOfPoints; i++)
    {
        float diffFromX = calibrationPoints[i].X - meanX;
        float diffFromY = calibrationPoints[i].Y - meanY;
        float prodDiffXY = diffFromX * diffFromY;
        float diffFromXSquared = diffFromX * diffFromX;
        sumProdOfDiffsXy += prodDiffXY;
        sumOfDiffsXSquared += diffFromXSquared;
    }

    LineFit params;

    params.slope = sumProdOfDiffsXy / sumOfDiffsXSquared;
    params.intercept = meanY - params.slope * meanX;

    return params;

}