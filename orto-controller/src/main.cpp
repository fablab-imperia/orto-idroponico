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
#include "analog_ph_probe.hpp"

//LCD dislay
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS,LCD_COLS,LCD_ROWS);
OneWire oneWire(PIN_TEMPERATURE);

//Temperature sensor
DallasTemperature temperatureSensor(&oneWire);

//PH sensor
AnalogPhProbe phProbe(PIN_PH_PROBE);

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("Starting...");

  lcd.init();             
  lcd.backlight();

  temperatureSensor.begin();
}

void loop() {
  temperatureSensor.requestTemperatures(); 
  lcd.clear();
  float temperatureC = temperatureSensor.getTempCByIndex(0);
  Serial.println(phProbe.getAveragePhValue());
  lcd.print(temperatureC);
  delay(2000);
} 

