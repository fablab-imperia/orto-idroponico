/*
This file is part of Orto idroponico Fablab Imperia

Orto idroponico Fablab Imperia is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Nome-Programma is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DEVICE_PARAMETERS
#define DEVICE_PARAMETERS


#define ORTO_WEB_SSID_BASENAME "OrtoWeb"
#define ORTO_WEB_SSID_PASSWD "OrtoWeb12345"

#define LCD_I2C_ADDRESS 0x27
#define LCD_ROWS 4
#define LCD_COLS 20

// Duration of one water pump cycle in seconds, set to 0 to keep the pump always on
#define TIME_WATER_PUMP_CYCLE (0)                                               // Put brackets around expressions like (60*60) [1 hour] because of operator precedence
// Time in seconds of water pump activity in a complete cycle (it's ignored if TIME_WATER_PUMP_CYCLE is zero)
#define TIME_WATER_PUMP_ACTIVE 15                                               // Put brackets around expressions like (60*60) [1 hour] because of operator precedence

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

// After MANUAL_CONTROL_PUMP_TIMEOUT ms all pumps will be turned off (without any other command)
#define MANUAL_CONTROL_PERISTALTIC_PUMP_TIMEOUT 800

#define SERIAL_BAUDRATE 115200
#define ADC_MAX_VALUES 4096
#define ADC_MAX_VOLTAGE 3.3

/*
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
#define SERIAL_BAUDRATE 9600
#define ADC_MAX_VALUES 1024
#define ADC_MAX_VOLTAGE 5
#else 
#define SERIAL_BAUDRATE 115200
#define ADC_MAX_VALUES 4096
#define ADC_MAX_VOLTAGE 3.3
#endif
*/
#endif
