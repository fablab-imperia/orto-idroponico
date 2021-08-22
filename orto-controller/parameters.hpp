#ifndef DEVICE_PARAMETERS
#define DEVICE_PARAMETERS

#define LCD_I2C_ADDRESS 0x27
#define LCD_ROWS 4
#define LCD_COLS 20


//Time the acid pump remains ON acter activation in ms
#define TIME_PERISTALTIC_PUMP_ACID_ON 1*1000L
//Time the fertilizer pump remains ON after activation in ms
#define TIME_PERISTALTIC_PUMP_FERTILIZER_ON 1*1000L
//Time between two consecutives sensor reads. If the value is below/up the the threshold the pump is acivated (acid or fertlizer)
#define TIME_BETWEEN_SENSOR_READS 60*1000L

// Update time of sensors shown in display
#define TIME_BETWEEN_DISPLAY_VALUES 1000 

//PH Threshold vale (if sensor value is higher, acid pump turns ON)
#define THRESHOLD_VALUE_PH 6
//PH Threshold vale (if  value is higher, acid pump turns ON)
#define THRESHOLD_VALUE_CONDUCTIVITY 900




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
