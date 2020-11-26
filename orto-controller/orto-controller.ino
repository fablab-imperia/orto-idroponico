#include <Arduino.h>

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

//LCD dislay
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS,LCD_COLS,LCD_ROWS);
OneWire oneWire(PIN_TEMPERATURE);

//Temperature sensor
DallasTemperature temperatureSensor(&oneWire);

//PH sensor
AnalogProbe phProbe(PIN_PH_PROBE,10);

//Conductivity sensor
AnalogProbe conductivityProbe(PIN_CONDUCTIVITY_PROBE,10);

Relay peristalticPumpAcid(PIN_PERISTALTIC_PUMP_ACID);  

void setup() {

  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("Starting...");
  Banner::printSerial();

  XYPair phCalibrationPoints[] = { {1.80f, 6.88f}, {1.46f, 4.00f,}, {2.13f, 9.23f}};
  LineFit phBestFit = Calibrator::findBestFit(phCalibrationPoints, 3);
  phProbe.initProbe(phBestFit);

  XYPair condCalibrationPoints[] = { { 0.2f, 220.0f}, { 0.6f, 564.0f}, { 0.0f, 68.0f}};
  LineFit condBestFit = Calibrator::findBestFit(condCalibrationPoints, 3);

  conductivityProbe.initProbe(condBestFit);
  lcd.init();
  lcd.backlight();

  temperatureSensor.begin();
  peristalticPumpAcid.init();
}

void loop() {
  AnalogValues phValues = phProbe.getAverageValue();
  AnalogValues condValues = conductivityProbe.getAverageValue();

  temperatureSensor.requestTemperatures(); 
  double temperatureC = temperatureSensor.getTempCByIndex(0);
  //clear display
  lcd.clear();
  //buffer for output
  char messageBuffer[20];
  //buffer for doubles that must be converted to strings
  char voltageBuffer[6];
  char valueBuffer[10];

  dtostrf(phValues.voltage, 4, 1, voltageBuffer);
  dtostrf(phValues.value, 6, 1, valueBuffer);
  snprintf(messageBuffer,20,"PH :%s VPH:%s", valueBuffer, voltageBuffer);
  Serial.println(messageBuffer);
  lcd.print(messageBuffer);
  lcd.setCursor(0,1);

  dtostrf(condValues.voltage, 4, 1, voltageBuffer);
  dtostrf(condValues.value, 6, 1, valueBuffer);
  snprintf(messageBuffer,20,"CND:%s VCD:%s", valueBuffer, voltageBuffer);
  Serial.println(messageBuffer);
  lcd.print(messageBuffer);
  lcd.setCursor(0,2);

  dtostrf(temperatureC, 6, 1, valueBuffer);
  snprintf(messageBuffer ,20,"TMP:%s", valueBuffer);
  Serial.println(messageBuffer);
  lcd.print(messageBuffer); 

  peristalticPumpAcid.toggle();
  delay(2000);
} 

