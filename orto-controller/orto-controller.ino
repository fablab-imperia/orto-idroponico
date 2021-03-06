#include <Arduino.h>

//Includes for LCD
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//Includes for Temperature sensor
#include <OneWire.h>
#include <DallasTemperature.h>

#include <TaskScheduler.h>

//Includes for our own pin definitions and setups
#include "pinouts.hpp"
#include "parameters.hpp"
#include "analog_probe.hpp"
#include "calibrator.hpp"
#include "relay.hpp"
#include "banner.hpp"



/************************************************************
 * Global variables
 ***********************************************************/
AnalogValues phValues;
AnalogValues condValues;
double temperatureC = 0.0;

/*************************************************************
 * Hardware components
 ************************************************************/

//LCD dislay
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS,LCD_COLS,LCD_ROWS);
OneWire oneWire(PIN_TEMPERATURE);

//Temperature sensor
DallasTemperature temperatureSensor(&oneWire);

//PH sensor
AnalogProbe phProbe(PIN_PH_PROBE,10);

//Conductivity sensor
AnalogProbe conductivityProbe(PIN_CONDUCTIVITY_PROBE,10);

// Main water pump
Relay mainWaterPump(PIN_MAIN_WATER_PUMP);  

// Peristaltic pumps
Relay fertilizerPump = Relay(PIN_PERISTALTIC_PUMP_FERTILIZER);  
Relay acidPump = Relay(PIN_PERISTALTIC_PUMP_ACID);  


/*************************************************************
 * Scheduling and tasks declarations
 ************************************************************/
Scheduler taskManager;

//TIMER
unsigned long lastswitch=0;

void displaySensorValue();
Task displayDataTask(TIME_BETWEEN_DISPLAY_VALUES_MS, TASK_FOREVER, displaySensorValue);

void readSensors();
Task sensorReadTask(TIME_BETWEEN_SENSORS_READ_MS, TASK_FOREVER, readSensors);

void waterPumpOn();
void waterPumpOff();
Task mainPumpOnTask(TIME_BETWEEN_PUMP_ACTIVATION_MS, TASK_ONCE, waterPumpOn);
Task mainPumpOffTask(TIME_BETWEEN_PUMP_ACTIVATION_MS, TASK_ONCE, waterPumpOff);

void acidPumpOn();
void acidPumpOff();
Task acidPumpOnTask(TASK_IMMEDIATE, TASK_ONCE, acidPumpOn);
Task acidPumpOffTask(TASK_IMMEDIATE, TASK_ONCE, acidPumpOff);

void fertilizerPumpOn();
void fertilizerPumpOff();
Task fertilizerPumpOnTask(TASK_IMMEDIATE, TASK_ONCE, fertilizerPumpOn);
Task fertilizerPumpOffTask(TASK_IMMEDIATE, TASK_ONCE, fertilizerPumpOff);

void convertMillisToTimeString(char* timeString, int timeStringSize, long millis) {
  long total = millis / 1000;
  int seconds = total % 60;
  total = total / 60;
  int minutes = total % 60;
  total = total / 60;
  int hours = total % 24;
  int days = total / 24;
  if (days==0) {
    snprintf(timeString, timeStringSize, "%d:%d:%d",hours, minutes, seconds);
    }
  else
  {
    snprintf(timeString, timeStringSize, "%dd %d:%d:%d", days,hours, minutes, seconds);
  } 
}
void convertMillisToShortTimeString(char* timeString, int timeStringSize, long millis) {
  long total = millis / 1000;
  total = total / 60;
  int minutes = total % 60;
  total = total / 60;
  int hours = total % 24;
  snprintf(timeString, timeStringSize, "%d:%d",hours, minutes);
  }

void setup() {

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
  mainWaterPump.init();
  fertilizerPump.init();
  acidPump.init();

  // Stops all pumps at beginning
  acidPump.turnOff();
  fertilizerPump.turnOff();
  mainWaterPump.turnOff();

  //Initialize tasks
  taskManager.init();
  taskManager.addTask(displayDataTask);
  taskManager.addTask(sensorReadTask);
  taskManager.addTask(mainPumpOnTask);
  taskManager.addTask(mainPumpOffTask);
  taskManager.addTask(acidPumpOnTask);
  taskManager.addTask(acidPumpOffTask);
  taskManager.addTask(fertilizerPumpOnTask);
  taskManager.addTask(fertilizerPumpOffTask);

  //Enable tasks 
  displayDataTask.enable();
  sensorReadTask.enable();
  mainPumpOnTask.enable();

}

void loop() {
  taskManager.execute();
} 

void waterPumpOn() {
  char timeString[20];
  lastswitch=millis();
  convertMillisToTimeString(timeString,20,lastswitch);
  Serial.print("---------WATER PUMP STARTS at ");
  Serial.println(timeString);
  mainWaterPump.turnOn();
  mainPumpOffTask.set(TIME_PUMP_ON_MS, TASK_ONCE, waterPumpOff);
  mainPumpOffTask.enableDelayed();
}

void waterPumpOff() {
  mainWaterPump.turnOff();
  char timeString[20];
  lastswitch=millis();
  convertMillisToTimeString(timeString,20,lastswitch);
  Serial.print("---------WATER PUMP STOPS at ");
  Serial.println(timeString);
  mainWaterPump.turnOff();
  mainPumpOnTask.set(TIME_BETWEEN_PUMP_ACTIVATION_MS, TASK_ONCE, waterPumpOn);
  mainPumpOnTask.enableDelayed();
}

void acidPumpOn() {
  char timeString[20];
  lastswitch=millis();
  convertMillisToTimeString(timeString,20,lastswitch);
  Serial.print("---------ACID PUMP STARTS at ");
  Serial.println(timeString);
  acidPump.turnOn();
  acidPumpOffTask.set(TIME_PERISTALTIC_PUMP_ACID_ON_MS, TASK_ONCE, acidPumpOff);
  acidPumpOffTask.enableDelayed();
}

void acidPumpOff() {
  acidPump.turnOff();
  char timeString[20];
  lastswitch=millis();
  convertMillisToTimeString(timeString,20,lastswitch);
  Serial.print("---------ACID PUMP STOPS at ");
  Serial.println(timeString);
  acidPump.turnOff();
}

void fertilizerPumpOn() {
  fertilizerPump.turnOn();
  char timeString[20];
  lastswitch=millis();
  convertMillisToTimeString(timeString,20,lastswitch);
  Serial.print("---------FERTILIZER PUMP STARTS at ");
  Serial.println(timeString);
  fertilizerPump.turnOn();
  fertilizerPumpOffTask.set(TIME_PERISTALTIC_PUMP_FERTILIZER_ON_MS, TASK_ONCE, fertilizerPumpOff);
  fertilizerPumpOffTask.enableDelayed();
}

void fertilizerPumpOff() {
  fertilizerPump.turnOff();
  char timeString[20];
  lastswitch=millis();
  convertMillisToTimeString(timeString,20,lastswitch);
  Serial.print("---------FERTILIZER PUMP STOPS at ");
  Serial.println(timeString);
  fertilizerPump.turnOff();
}

void readSensors() {
  phValues = phProbe.getAverageValue();
  condValues = conductivityProbe.getAverageValue();
  temperatureSensor.requestTemperatures(); 
  temperatureC = temperatureSensor.getTempCByIndex(0);

  
  // acid pump tasks are both disabled and ph values are out of range
  if (!acidPumpOnTask.isEnabled() && 
      !acidPumpOffTask.isEnabled() && 
      (phValues.value - THRESHOLD_VALUE_PH) > THRESHOLD_TOLERANCE_PH)
      {
          Serial.println("---------PH LEVEL OVER THRESHOLD: NEED TO START ACID PUMP");
          acidPumpOnTask.set(TASK_IMMEDIATE, TASK_ONCE, acidPumpOn);
          acidPumpOnTask.enableDelayed();
      } 

    // fertilizer pump tasks are both disabled and ph values are out of range
  if (!fertilizerPumpOnTask.isEnabled() && 
      !fertilizerPumpOffTask.isEnabled() && 
      (THRESHOLD_VALUE_CONDUCTIVITY - condValues.value) > THRESHOLD_TOLERANCE_CONDUCTIVITY)
      {
          Serial.println("---------CONDUCTIVITY BELOW THRESHOLD: NEED TO START FERTIZILER PUMP");
          fertilizerPumpOnTask.set(TASK_IMMEDIATE, TASK_ONCE, fertilizerPumpOn);
          fertilizerPumpOnTask.enableDelayed();
      } 
}

void displaySensorValue() {

/*  
  Serial.print("PH VALUE: ");
  Serial.println(phValues.value);
  
  Serial.print("COND VALUE: ");
  Serial.println(condValues.value);
  */
  
  //clear display
  lcd.clear();
  //buffer for output
  char messageBuffer[20];
  //buffer for doubles that must be converted to strings
  char valueBuffer[10];

  //dtostrf(phValues.voltage, 3, 1, voltageBuffer);
  dtostrf(phValues.value, 4, 1, valueBuffer);
  snprintf(messageBuffer,20,"PH :%s   ", valueBuffer);  // VPH:%s  , voltageBuffer
  //Serial.println(messageBuffer);

  lcd.print(messageBuffer);
  //lcd.setCursor(0,1);

  //dtostrf(condValues.voltage, 4, 1, voltageBuffer);
  dtostrf(condValues.value, 5, 1, valueBuffer);
  snprintf(messageBuffer,20,"CND:%s ", valueBuffer);  //VCD:%s  , voltageBuffer
  //Serial.println(messageBuffer);
  lcd.print(messageBuffer);
  lcd.setCursor(0,1);

  dtostrf(temperatureC, 5, 1, valueBuffer);
  snprintf(messageBuffer ,20,"TMP:%s", valueBuffer);
  //Serial.println(messageBuffer);
  lcd.print(messageBuffer); 

  lcd.setCursor(0,2);
  char timeString[20];
  char lastimeString[20];
  convertMillisToTimeString(timeString,20,millis());
  convertMillisToShortTimeString(lastimeString,20,millis()-lastswitch);
  snprintf(messageBuffer ,20,"%s - %sm fa", timeString, lastimeString);
  lcd.print (messageBuffer);

}

