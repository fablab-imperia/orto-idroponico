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
#include "controller.hpp"






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
Relay waterPump(PIN_MAIN_WATER_PUMP);  

// Peristaltic pumps
Relay fertilizerPump = Relay(PIN_PERISTALTIC_PUMP_FERTILIZER);  
Relay acidPump = Relay(PIN_PERISTALTIC_PUMP_ACID);  


/*************************************************************
 * Scheduling and tasks declarations
 ************************************************************/
Controller ctrl;

Scheduler taskManager;

//TIMER
unsigned long lastswitch=0;

//Timer for updating sensor values shown to users (display, webn, etc)
void readAndShowSensorValues();
Task readAndShowSensorValuesTask(TIME_BETWEEN_DISPLAY_VALUES * TASK_MILLISECOND, TASK_FOREVER, readAndShowSensorValues);

//Timer for reading sensors and starting both pumps if it is the case
void readSensorsAndStartPumps();
Task readSensorsAndStartPumpsTask(TIME_BETWEEN_SENSOR_READS * TASK_MILLISECOND, TASK_FOREVER, readSensorsAndStartPumps);

//Task for stopping acid pump
void acidPumpOff();
Task acidPumpOffTask(TIME_PERISTALTIC_PUMP_ACID_ON * TASK_MILLISECOND, TASK_ONCE, acidPumpOff);

//Task for stopping fertilizer pump
void fertilizerPumpOff();
Task fertilizerPumpOffTask(TIME_PERISTALTIC_PUMP_FERTILIZER_ON * TASK_MILLISECOND, TASK_ONCE, fertilizerPumpOff);


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

void convertCurrentStateToString(char* stateString, int stateStringSize) {
  switch (ctrl.getState())
  {
  case ControllerState::WATER_CIRCULATING:
    snprintf(stateString, stateStringSize, "WATER_CIRCULATING");
    break;

  case ControllerState::WATER_MIXING:
    snprintf(stateString, stateStringSize, "WATER_MIXING");
    break;

  default:
   snprintf(stateString, stateStringSize, "IDLE");
    break;
  }
  
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
  waterPump.init();
  fertilizerPump.init();
  acidPump.init();

  // Stops all pumps at beginning
  acidPump.turnOff();
  fertilizerPump.turnOff();
  waterPump.turnOff();

  // Water pump always ON to simplify mixing
  waterPump.turnOn();

  //Initialize tasks
  taskManager.init();
  taskManager.addTask(readAndShowSensorValuesTask);
  taskManager.addTask(readSensorsAndStartPumpsTask);
  taskManager.addTask(acidPumpOffTask);
  taskManager.addTask(fertilizerPumpOffTask);

  //Enable all tasks (show values and sensor reads) 
  readAndShowSensorValuesTask.enable();
  readSensorsAndStartPumpsTask.enable();

}

void loop() {
  taskManager.execute();
} 


void acidPumpOff() {
  acidPump.turnOff();
  char timeString[20];
  lastswitch=millis();
  convertMillisToTimeString(timeString,20,lastswitch);
  Serial.print("---------ACID PUMP STOPS at ");
  Serial.println(timeString);
}



void fertilizerPumpOff() {
  fertilizerPump.turnOff();
  char timeString[20];
  lastswitch=millis();
  convertMillisToTimeString(timeString,20,lastswitch);
  Serial.print("---------FERTILIZER PUMP STOPS at ");
  Serial.println(timeString);
}

void readSensorsAndStartPumps() {

  AnalogValues phValues;
  AnalogValues condValues;
  double temperatureC;

  phValues = phProbe.getAverageValue();
  condValues = conductivityProbe.getAverageValue();
  temperatureSensor.requestTemperatures(); 
  temperatureC = temperatureSensor.getTempCByIndex(0);

  char timeString[20];
  lastswitch=millis();
  convertMillisToTimeString(timeString,20,lastswitch);

  // If PH value is found above threshold
  if (phValues.value > THRESHOLD_VALUE_PH)
  {
      Serial.println("---------PH LEVEL OVER THRESHOLD: NEED TO START ACID PUMP");
      //start acid pump
      acidPump.turnOn();
      Serial.print("---------ACID PUMP STARTS at ");
      Serial.println(timeString);
      //schedule a task for stopping the pump after specific number of seconds
      acidPumpOffTask.set(TIME_PERISTALTIC_PUMP_ACID_ON * TASK_MILLISECOND, TASK_ONCE, acidPumpOff);
      acidPumpOffTask.enableDelayed();
  } 
  else 
  {
    Serial.print("PH IS ");
    Serial.println(phValues.value);
    Serial.print("THRESHOLD IS ");
    Serial.println(THRESHOLD_VALUE_PH);
    Serial.print("NO NEED TO ACTIVATE PUMP AT ");
    Serial.println(timeString);
  }


  // If PH value is found above threshold
  if (condValues.value < THRESHOLD_VALUE_CONDUCTIVITY)
  {
      Serial.println("---------CONDUCTIVITY LEVEL UNDER THRESHOLD: NEED TO START FERTILIZER PUMP");
      //start fertilizer pump
      fertilizerPump.turnOn();
      Serial.print("---------FERTILIZER PUMP STARTS at ");
      Serial.println(timeString);
      //schedule a task for stopping the pump after specific number of seconds
      fertilizerPumpOffTask.set(TIME_PERISTALTIC_PUMP_FERTILIZER_ON * TASK_MILLISECOND, TASK_ONCE, fertilizerPumpOff);
      fertilizerPumpOffTask.enableDelayed();
  }  
  else 
  {
    Serial.print("CONDUCTIVITY IS ");
    Serial.println(condValues.value);
    Serial.print("THRESHOLD IS ");
    Serial.println(THRESHOLD_VALUE_CONDUCTIVITY);
    Serial.print("NO NEED TO ACTIVATE PUMP AT ");
    Serial.println(timeString);
  }

}

void readAndShowSensorValues() {

  AnalogValues phValues;
  AnalogValues condValues;
  double temperatureC = 0.0;

  phValues = phProbe.getAverageValue();
  condValues = conductivityProbe.getAverageValue();
  temperatureSensor.requestTemperatures(); 
  temperatureC = temperatureSensor.getTempCByIndex(0);
  
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

