#ifndef BOARDS_PARAMETERS_H
#define BOARDS_PARAMETERS_H

#ifdef ESP32
// PINOUTS FOR ESP32
#define PIN_I2C_SDA 21
#define PIN_I2C_SCL 22
#define PIN_LED  1
//Baudrate 
#define SERIAL_BAUDRATE 115200
#endif


#ifdef ARDUINO_UNO
// PINOUTS FOR ARDUINO UNO
#define PIN_I2C_SDA 18
#define PIN_I2C_SCL 19
#define PIN_LED  13
//Baudrate 
#define SERIAL_BAUDRATE 9600
#endif

#endif
