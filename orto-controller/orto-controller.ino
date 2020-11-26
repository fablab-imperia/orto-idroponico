#include <Arduino.h>

//Includes for LCD
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//Includes for Temperature sensor
#include <OneWire.h>
#include <DallasTemperature.h>

//Includes for our own pin definitions and setups
#include "boards_pinouts.hpp"
#include "devices_parameters.hpp"
#include "analog_probe.hpp"

#include "calibrator.hpp"

//LCD dislay
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS,LCD_COLS,LCD_ROWS);
OneWire oneWire(PIN_TEMPERATURE);

//Temperature sensor
DallasTemperature temperatureSensor(&oneWire);

//PH sensor
AnalogProbe phProbe(PIN_PH_PROBE,10);

//Conductivity sensor
AnalogProbe conductivityProbe(PIN_CONDUCTIVITY_PROBE,10);



void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("Starting...");
  
  XYPair phCalibrationPoints[] = { {6.88f, 1.80f}, {4.00f, 1.46f}, {9.23f, 2.13f}};
  LineFit phBestFit = Calibrator::findBestFit(phCalibrationPoints, 3);
  phProbe.initProbe(phBestFit);

  XYPair condCalibrationPoints[] = { {6.88f, 1.80f}, {4.00f, 1.46f}, {9.23f, 2.13f}};
  LineFit condBestFit = Calibrator::findBestFit(condCalibrationPoints, 3);
  conductivityProbe.initProbe(condBestFit);
  lcd.init();
  lcd.backlight();

  temperatureSensor.begin();
}

void loop() {
  AnalogValues phValues = phProbe.getAverageValue();
  AnalogValues condValues = conductivityProbe.getAverageValue();

  temperatureSensor.requestTemperatures(); 
  float temperatureC = temperatureSensor.getTempCByIndex(0);
  //clear display
  lcd.clear();
  //buffer for output
  char messageBuffer[20];
  //buffer for floats that must be converted to strings
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

  delay(2000);
} 

