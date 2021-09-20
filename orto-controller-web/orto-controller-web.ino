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
StaticJsonDocument<200> doc;


/*************************************************************
   Hardware components
 ************************************************************/

//LCD dislay
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLS, LCD_ROWS);
OneWire oneWire(PIN_TEMPERATURE);

//Temperature sensor
DallasTemperature temperatureSensor(&oneWire);

//PH sensor
AnalogProbe phProbe(PIN_PH_PROBE, 10);

//Conductivity sensor
AnalogProbe conductivityProbe(PIN_CONDUCTIVITY_PROBE, 10);

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
unsigned long lastdisplay = 0;

// Manual or automatic control state
bool manual_control = false;
bool manual_peristaltic_active = false;

void setup() {
  // Serial port for debugging purposes
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("Starting...");
  Banner::printSerial();



  //XYPair phCalibrationPoints[] = { {1.80f, 6.88f}, {1.46f, 4.00f,}, {2.13f, 9.23f}};//stefano
  XYPair phCalibrationPoints[] = {{1.51f, 4.01f,}, {1.86f, 7.00f}, {2.08f, 8.80f}, {2.23f, 10.01f,}};//Vale
  //LineFit phBestFit = Calibrator::findBestFit(phCalibrationPoints, 3);//Stefano
  LineFit phBestFit = Calibrator::findBestFit(phCalibrationPoints, 4);//vale

  XYPair condCalibrationPoints[] = { { 0.2f, 220.0f}, { 0.6f, 564.0f}, { 0.0f, 68.0f}};
  LineFit condBestFit = Calibrator::findBestFit(condCalibrationPoints, 3);

  phProbe.initProbe(phBestFit);
  conductivityProbe.initProbe(condBestFit);

  lcd.init();
  lcd.backlight();
  temperatureSensor.begin();
  waterPump.init();
  fertilizerPump.init();
  acidPump.init();

  // Stops all pumps at beginning
  acidPump.turnOff();
  fertilizerPump.turnOff();
  waterPump.turnOff();

  // Water pump always ON to simplify mixing
  waterPump.turnOn();

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
    if (millis() - lastswitch > MANUAL_CONTROL_PERISTALTIC_PUMP_TIMEOUT) {
      acidPump.turnOff();
      fertilizerPump.turnOff();
    }
  } else {
    if (millis() - lastdisplay >= TIME_BETWEEN_SENSOR_READS) {
      readSensorsAndStartPumps();
    }
    if (TIME_WATER_PUMP_CYCLE) {
      if ((millis()/1000 % TIME_WATER_PUMP_CYCLE) < TIME_WATER_PUMP_ACTIVE) {
        waterPump.turnOn();
      } else {
        waterPump.turnOff();
      }
    } else {
      waterPump.turnOn();
    }
  }
}
