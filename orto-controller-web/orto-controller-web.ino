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

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>

// Include library to save parameters in EEPROM
#include <EEPROM.h>

//Includes for LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Includes for Temperature sensor
#include <OneWire.h>
#include <DallasTemperature.h>


//Includes for our own pin definitions and setups
#include "pinouts.hpp"
#include "parameters.hpp"
#include "analog_probe.hpp"
#include "calibrator.hpp"
#include "relay.hpp"
#include "banner.hpp"


// Filesystem for web page files
#define FORMAT_FILESYSTEM false
#include <SPIFFS.h>

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// JSON buffer
StaticJsonDocument<800> doc;


/*************************************************************
   Hardware components
 ************************************************************/

//LCD dislay
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLS, LCD_ROWS);

//Temperature sensor
OneWire oneWire(PIN_TEMPERATURE);
DallasTemperature temperatureSensor(&oneWire);


ADAFRUIT_ADC adc;

//PH sensor
AnalogProbe phProbe(CHANNEL_PH_PROBE, 10);

//Conductivity sensor
AnalogProbe conductivityProbe(CHANNEL_CONDUCTIVITY_PROBE, 10);

// Main water pump
Relay waterPump(PIN_MAIN_WATER_PUMP);

// Peristaltic pumps
Relay fertilizerPump = Relay(PIN_PERISTALTIC_PUMP_FERTILIZER);
Relay acidPump = Relay(PIN_PERISTALTIC_PUMP_ACID);


/*************************************************************
   Scheduling and tasks declarations
 ************************************************************/

//TIMER
unsigned long lastswitch = 0;
unsigned long lastcontrol = 0;
unsigned long lastping = 0;
unsigned long lastdisplay = 0;

// Manual or automatic control state
bool manual_control = false;
bool manual_peristaltic_active = false;

// Parameters
float threshold_value_ph = DEFAULT_THRESHOLD_VALUE_PH;                          // pH
int threshold_value_conductivity = DEFAULT_THRESHOLD_VALUE_CONDUCTIVITY;        // uS/cm (?)
unsigned int time_peristaltic_pump_on = DEFAULT_TIME_PERISTALTIC_PUMP_ON;       // ms
unsigned long time_between_sensor_reads = DEFAULT_TIME_BETWEEN_SENSOR_READS;    // s
unsigned long time_water_pump_cycle = DEFAULT_TIME_WATER_PUMP_CYCLE;            // s
unsigned long time_water_pump_active = DEFAULT_TIME_WATER_PUMP_ACTIVE;          // s


void setup() {
  // Serial port for debugging purposes
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("Starting...");
  Banner::printSerial();

  EEPROM.begin(EEPROM_USED_SIZE);

  //XYPair phCalibrationPoints[] = {{1.80f, 6.88f}, {1.46f, 4.00f,}, {2.13f, 9.23f}};//stefano
  XYPair phCalibrationPoints[] = {{1.51f, 4.01f,}, {1.86f, 7.00f}, {2.08f, 8.80f}, {2.23f, 10.01f,}};//Vale
  //LineFit phBestFit = Calibrator::findBestFit(phCalibrationPoints, 3);//Stefano
  LineFit phBestFit = Calibrator::findBestFit(phCalibrationPoints, 4);//vale

  XYPair condCalibrationPoints[] = {{ 0.2f, 220.0f}, { 0.6f, 564.0f}, { 0.0f, 68.0f}};
  LineFit condBestFit = Calibrator::findBestFit(condCalibrationPoints, 3);

  phProbe.initProbe(phBestFit);
  conductivityProbe.initProbe(condBestFit);

  lcd.init();
  lcd.backlight();
  temperatureSensor.begin();
  waterPump.init();
  fertilizerPump.init();
  acidPump.init();

  adc.begin(0x48);              // Initialize ADC at the default address 0x48 (address can be setted with ADR pin: https://learn.adafruit.com/adafruit-4-channel-adc-breakouts/assembly-and-wiring#i2c-addressing-2974117-12)
  adc.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV (default)

  retrive_configuration();

  // Stops all pumps at beginning
  acidPump.turnOff();
  fertilizerPump.turnOff();
  waterPump.turnOff();


  WiFi.softAP(ORTO_WEB_SSID_BASENAME, ORTO_WEB_SSID_PASSWD);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  fs_web_init();

}

void loop() {
  ws.cleanupClients();

  if (millis() - lastdisplay >= TIME_BETWEEN_DISPLAY_VALUES && !manual_peristaltic_active) {
    readAndShowSensorValues();
  }



  if (manual_control) {
    unsigned long t = millis();
    if ((t - lastping) > MANUAL_CONTROL_PERISTALTIC_PUMP_TIMEOUT && t >= lastping && manual_peristaltic_active) {
      acidPump.turnOff();
      fertilizerPump.turnOff();
      manual_peristaltic_active = false;
      Serial.print("Turning off peristaltic pumps: no ping received in ");
      Serial.print(MANUAL_CONTROL_PERISTALTIC_PUMP_TIMEOUT);
      Serial.print(" ms -  ");
      Serial.print(t);
      Serial.print("  -  ");
      Serial.println(lastping);
    }
  } else {
    if ((millis() - lastcontrol) / 1000 >= time_between_sensor_reads) {
      readSensorsAndStartPumps();
    }
    if (time_water_pump_cycle) {
      if ((millis() / 1000 % time_water_pump_cycle) < time_water_pump_active) {
        waterPump.turnOn();
      } else {
        waterPump.turnOff();
      }
    } else {
      waterPump.turnOn();
    }
  }
}
