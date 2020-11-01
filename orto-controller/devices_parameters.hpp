#ifndef DEVICE_PARAMETERS
#define DEVICE_PARAMETERS

#define LCD_I2C_ADDRESS 0x27
#define LCD_ROWS 4
#define LCD_COLS 20

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
#define PH_7_VOLTAGE 2.19
#else
#define PH_7_VOLTAGE 1.0
#endif


#endif
