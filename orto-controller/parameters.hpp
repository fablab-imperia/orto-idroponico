#ifndef DEVICE_PARAMETERS
#define DEVICE_PARAMETERS

#define LCD_I2C_ADDRESS 0x27
#define LCD_ROWS 4
#define LCD_COLS 20

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
#define SERIAL_BAUDRATE 9600
#define ADC_MAX_VALUES 1024
#define ADC_MAX_VOLTAGE 5
#else 
#define SERIAL_BAUDRATE 115200
#define ADC_MAX_VALUES 4096
#define ADC_MAX_VOLTAGE 3.3
#endif
#endif
