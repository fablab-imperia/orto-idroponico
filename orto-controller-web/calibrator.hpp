#ifndef CALIBRATOR_HPP
#define CALIBRATOR_HPP

#include "line_fit.hpp"

struct XYPair {
  float X;
  float Y;
};

class Calibrator {
  public:
    static LineFit findBestFit(XYPair calibrationPoints[], unsigned int numberOfPoints);
};

#endif
