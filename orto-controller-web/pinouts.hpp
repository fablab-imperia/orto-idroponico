#ifndef BOARDS_PARAMETERS_H
#define BOARDS_PARAMETERS_H

// PINOUTS FOR ESP32
// #define PIN_I2C_SDA 21                 // Default SDA pin, different SDA pin is not implemented in this code
// #define PIN_I2C_SCL 22                 // Default SCL pin, different SCL pin is not implemented in this code
#define PIN_LED  1
#define PIN_TEMPERATURE 32
#define CHANNEL_PH_PROBE 0                // Channel of the ADS_1x15 ADC connected to pH probe
#define CHANNEL_CONDUCTIVITY_PROBE 1      // Channel of the ADS_1x15 ADC connected to conductivity probe
#define PIN_MAIN_WATER_PUMP 25
#define PIN_PERISTALTIC_PUMP_ACID 26
#define PIN_PERISTALTIC_PUMP_FERTILIZER 27

//#endif

#endif
