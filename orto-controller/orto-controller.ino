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
 // lcd.backlight();

 // temperatureSensor.begin();
}

void loop() {
  AnalogValues phValues = phProbe.getAverageValue();
  AnalogValues condValues = conductivityProbe.getAverageValue();

  //float conductivityValue = conductivityProbe.getAverageConductivityValue();
  Serial.print("voltage: ");
  Serial.print(phValues.voltage);
  Serial.print(" - ");
  Serial.print(" ph: ");
  Serial.println(phValues.value);
  
  Serial.print("voltage: ");
  Serial.print(condValues.voltage);
  Serial.print(" - ");
  Serial.print(" conductivity: ");
  Serial.println(condValues.value);
  //Serial.println(a2);
  //temperatureSensor.requestTemperatures(); 
  //lcd.clear();
  //float temperatureC = temperatureSensor.getTempCByIndex(0);
  //lcd.print(temperatureC);
  delay(2000);
} 

