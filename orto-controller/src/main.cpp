#include <Arduino.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include "boards_parameters.hpp"
#include "devices_parameters.hpp"

LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS,LCD_COLS,LCD_ROWS);

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("Starting...");

  lcd.init();             
  lcd.backlight();

}

void loop() {
  delay(1000);
  lcd.clear();
  lcd.print("Hello World!");
} 

