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

void acidPumpOff() {
  acidPump.turnOff();
  char timeString[20];
  lastswitch = millis();
  convertMillisToTimeString(timeString, 20, lastswitch);
  Serial.print("---------ACID PUMP STOPS at ");
  Serial.println(timeString);
}



void fertilizerPumpOff() {
  fertilizerPump.turnOff();
  char timeString[20];
  lastswitch = millis();
  convertMillisToTimeString(timeString, 20, lastswitch);
  Serial.print("---------FERTILIZER PUMP STOPS at ");
  Serial.println(timeString);
}



void readSensorsAndStartPumps() {
  AnalogValues phValues;
  AnalogValues condValues;

  phValues = phProbe.getAverageValue(adc);
  condValues = conductivityProbe.getAverageValue(adc);

  char timeString[20];
  lastcontrol = millis();
  convertMillisToTimeString(timeString, 20, lastcontrol);

  // If PH value is found above threshold
  if (phValues.value > threshold_value_ph)
  {
    Serial.println("---------PH LEVEL OVER THRESHOLD: NEED TO START ACID PUMP");
    //start acid pump
    acidPump.turnOn();
    Serial.print("---------ACID PUMP STARTS at ");
    Serial.println(timeString);
    //schedule a task for stopping the pump after specific number of seconds
    delay(time_peristaltic_pump_on);  // TODO: manage delay in loop with non-blocking code
    acidPumpOff();
  }
  else
  {
    Serial.print("PH IS ");
    Serial.println(phValues.value);
    Serial.print("THRESHOLD IS ");
    Serial.println(threshold_value_ph);
    Serial.print("NO NEED TO ACTIVATE PUMP AT ");
    Serial.println(timeString);
  }


  // If conductivity value is found below threshold
  if (condValues.value < threshold_value_conductivity)
  {
    Serial.println("---------CONDUCTIVITY LEVEL UNDER THRESHOLD: NEED TO START FERTILIZER PUMP");
    //start fertilizer pump
    fertilizerPump.turnOn();
    Serial.print("---------FERTILIZER PUMP STARTS at ");
    Serial.println(timeString);
    delay(time_peristaltic_pump_on); // TODO: manage delay in loop with non-blocking code
    fertilizerPumpOff();
  }
  else
  {
    Serial.print("CONDUCTIVITY IS ");
    Serial.println(condValues.value);
    Serial.print("THRESHOLD IS ");
    Serial.println(threshold_value_conductivity);
    Serial.print("NO NEED TO ACTIVATE PUMP AT ");
    Serial.println(timeString);
  }

}

void readAndShowSensorValues() {
  lastdisplay = millis();

  AnalogValues phValues;
  AnalogValues condValues;
  double temperatureC = 0.0;

  phValues = phProbe.getAverageValue(adc);
  condValues = conductivityProbe.getAverageValue(adc);
  temperatureSensor.requestTemperatures();
  temperatureC = temperatureSensor.getTempCByIndex(0);


  //buffer for output
  char messageBuffer[20];
  //buffer for doubles that must be converted to strings
  char valueBuffer[10];


  lcd.setCursor(0, 3);        // Overwrite last line
  char timeString[20];
  char lastimeString[20];
  convertMillisToTimeString(timeString, 20, millis());
  convertMillisToShortTimeString(lastimeString, 20, millis() - lastcontrol);
  snprintf(messageBuffer , 20, "%s - %sm fa", timeString, lastimeString);
  lcd.print (messageBuffer);
  lcd.print("    ");          // Clear next cells

  //clear display
  //lcd.clear();
  lcd.setCursor(0, 0); // Set cursor to starting position instead of clear() to reduce flickering

  //dtostrf(phValues.voltage, 3, 1, voltageBuffer);
  dtostrf(phValues.value, 4, 1, valueBuffer);
  snprintf(messageBuffer, 20, "pH :%s   ", valueBuffer); // VPH:%s  , voltageBuffer
  //Serial.println(messageBuffer);

  lcd.print(messageBuffer);
  //lcd.setCursor(0,1);

  //dtostrf(condValues.voltage, 4, 1, voltageBuffer);
  dtostrf(condValues.value, 5, 1, valueBuffer);
  snprintf(messageBuffer, 20, "CND:%s ", valueBuffer); //VCD:%s  , voltageBuffer
  //Serial.println(messageBuffer);
  lcd.print(messageBuffer);
  lcd.print("    ");          // Clear next cells
  lcd.setCursor(0, 1);

  dtostrf(temperatureC, 5, 1, valueBuffer);
  snprintf(messageBuffer , 20, "TMP:%s", valueBuffer);
  //Serial.println(messageBuffer);
  lcd.print(messageBuffer);

  lcd.print("    ");          // Clear next cells


  doc["temp"] = temperatureC;
  doc["ph"] = phValues.value;
  doc["cond"] = condValues.value;


  send_current_status();
}

void convertMillisToTimeString(char* timeString, int timeStringSize, long millis) {
  long total = millis / 1000;
  int seconds = total % 60;
  total = total / 60;
  int minutes = total % 60;
  total = total / 60;
  int hours = total % 24;
  int days = total / 24;
  if (days == 0) {
    snprintf(timeString, timeStringSize, "%d:%d:%d", hours, minutes, seconds);
  }
  else
  {
    snprintf(timeString, timeStringSize, "%dd %d:%d:%d", days, hours, minutes, seconds);
  }
}
void convertMillisToShortTimeString(char* timeString, int timeStringSize, long millis) {
  long total = millis / 1000;
  total = total / 60;
  int minutes = total % 60;
  total = total / 60;
  int hours = total % 24;
  snprintf(timeString, timeStringSize, "%d:%d", hours, minutes);
}


// EEPROM memory map
/*
  /-------------------------------------------------------------------------\
  | address |                 parameter                 |  unit of measure  |
  |-------------------------------------------------------------------------|
  | 0       | checksum*                                 |      -            |
  | 1       | threshold_value_ph*100                    |      pH           |
  | 2       | threshold_value_conductivity   LOW BYTE   |      uS/cm        |
  | 3       | threshold_value_conductivity   HIGH BYTE  |      uS/cm        |
  | 4       | time_peristaltic_pump_on  LOW BYTE        |      ms           |
  | 5       | time_peristaltic_pump_on  MIDDLE BYTE     |      ms           |
  | 6       | time_peristaltic_pump_on  HIGH BYTE       |      ms           |
  | 7       | time_between_sensor_reads  LOW BYTE       |      s            |
  | 8       | time_between_sensor_reads  HIGH BYTE      |      s            |
  | 9       | time_water_pump_cycle  LOW BYTE           |      s            |
  | 10      | time_water_pump_cycle  MIDDLE BYTE        |      s            |
  | 11      | time_water_pump_cycle  HIGH BYTE          |      s            |
  | 12      | time_water_pump_active  LOW BYTE          |      s            |
  | 13      | time_water_pump_active  MIDDLE BYTE       |      s            |
  | 14      | time_water_pump_active  HIGH BYTE         |      s            |
  \-------------------------------------------------------------------------/

  the checksum byte is calculated as the XOR of bytes from 1 to 14 (included)
  so the XOR of bytes from 1 to 14 (included) should be 0 for valid data

  NOTE: in case of invalid checksum or if all 14 bytes are 0 or 255 will be used the default parameters

*/


void save_configuration (char parameter) {
  switch (parameter) {
    case '0':
      EEPROM.write(1, (uint8_t)(threshold_value_ph * 10));
      break;
    case '1':
      EEPROM.write(2, (threshold_value_conductivity & 0b0000000011111111));
      EEPROM.write(3, (threshold_value_conductivity & 0b1111111100000000) >> 8);
      break;
    case '2':
      EEPROM.write(4, (time_peristaltic_pump_on & 0b000000000000000011111111));
      EEPROM.write(5, (time_peristaltic_pump_on & 0b000000001111111100000000) >> 8);
      EEPROM.write(6, (time_peristaltic_pump_on & 0b111111110000000000000000) >> 16);
      break;
    case '3':
      EEPROM.write(7, (time_between_sensor_reads & 0b0000000011111111));
      EEPROM.write(8, (time_between_sensor_reads & 0b1111111100000000) >> 8);
      break;
    case '4':
      EEPROM.write(9, (time_water_pump_cycle & 0b000000000000000011111111));
      EEPROM.write(10, (time_water_pump_cycle & 0b000000001111111100000000) >> 8);
      EEPROM.write(11, (time_water_pump_cycle & 0b111111110000000000000000) >> 16);
      break;
    case '5':
      EEPROM.write(12, (time_water_pump_active & 0b000000000000000011111111));
      EEPROM.write(13, (time_water_pump_active & 0b000000001111111100000000) >> 8);
      EEPROM.write(14, (time_water_pump_active & 0b111111110000000000000000) >> 16);
      break;
  }

  EEPROM.commit();
  delay(10);                            // Delay just to be safe

  if (parameter == '5') {
    EEPROM.write(0, calculate_checksum());
    EEPROM.commit();
  }
}

bool retrive_configuration () {
  bool is_0 = true;
  bool is_1 = true;

  uint8_t checksum = 0;
  for (uint8_t i = 0; i < EEPROM_USED_SIZE; i++) {
    uint8_t val = EEPROM.read(i);
    //    Serial.println(val, BIN);
    checksum ^= val;

    if (i != 0) {
      if (val != 0b00000000) {
        is_0 = false;
      }
      if (val != 0b11111111) {
        is_1 = false;
      }
    }
  }

  if (is_0 || is_1 || checksum != 0) {
    Serial.println("Invalid parameters or checksum, using default values");
    return false;
  }

  threshold_value_ph = EEPROM.read(1) / 10.0;
  threshold_value_conductivity = EEPROM.read(2) + (EEPROM.read(3) << 8);
  time_peristaltic_pump_on = EEPROM.read(4) + (EEPROM.read(5) << 8) + (EEPROM.read(6) << 16);
  time_between_sensor_reads = EEPROM.read(7) + (EEPROM.read(8) << 8);
  time_water_pump_cycle = EEPROM.read(9) + (EEPROM.read(10) << 8) + (EEPROM.read(11) << 16);
  time_water_pump_active = EEPROM.read(12) + (EEPROM.read(13) << 8) + (EEPROM.read(14) << 16);

  return true;
}

uint8_t calculate_checksum () {
  uint8_t checksum = 0;
  for (uint8_t i = 1; i < EEPROM_USED_SIZE; i++) {
    checksum ^= EEPROM.read(i);
  }

  return checksum;
}

// END EEPROM memory

// WEB

void send_current_status () {
  doc["pump_on"] = waterPump.isOn();
  doc["manual_control"] = manual_control;
  doc["ph_th"] = threshold_value_ph;
  doc["cond_th"] = threshold_value_conductivity;
  doc["t_peristaltic"] = time_peristaltic_pump_on;
  doc["t_reads"] = time_between_sensor_reads;
  doc["t_pump_cycle"] = time_water_pump_cycle;
  doc["t_pump_active"] = time_water_pump_active;


  char payload[800];
  serializeJson(doc, payload);

  notifyClients(payload);
}

void fs_web_init() {

  SPIFFS.begin();
  {
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file) {
      String fileName = file.name();
      size_t fileSize = file.size();
      Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
      file = root.openNextFile();
    }
    Serial.printf("\n");
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });
  server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/main.js", "application/javascript");
  });
  server.on("/manual_control.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/manual_control.js", "application/javascript");
  });
  server.on("/automatic_config.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/automatic_config.js", "application/javascript");
  });

  initWebSocket();

  // Start server
  server.begin();

}

void notifyClients(String payload) {
  ws.textAll(payload);
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      char payload = *data;
      Serial.println(payload);

      if (payload == 'M') {           // Manual Control
        manual_control = true;
        acidPump.turnOff();
        fertilizerPump.turnOff();
        waterPump.turnOff();
        manual_peristaltic_active = false;
      } else if (payload == 'U') {    // aUtomatic Control
        manual_control = false;
        acidPump.turnOff();
        fertilizerPump.turnOff();
        waterPump.turnOff();
        manual_peristaltic_active = false;
      }

      if (manual_control) {
        if (payload == 'F') {
          fertilizerPump.turnOn();
          manual_peristaltic_active = true;
          lastping = millis();
        } else if (payload == 'A') {
          acidPump.turnOn();
          manual_peristaltic_active = true;
          lastping = millis();
        } else if (payload == 'P') {
          waterPump.turnOn();
        } else if (payload == 'f') {
          fertilizerPump.turnOff();
          manual_peristaltic_active = false;
          lastping = millis();
        } else if (payload == 'a') {
          acidPump.turnOff();
          manual_peristaltic_active = false;
          lastping = millis();
        } else if (payload == 'p') {
          waterPump.turnOff();
        }
      }

      if (payload >= '0' && payload <= '5') {
        String value_string = "";
        int i = 1;
        while (*(data + i) != ';' && i < 15) {
          value_string += (char) * (data + i);
          i++;
        }

        if (payload == '0') {
          threshold_value_ph = value_string.toFloat();
        } else if (payload == '1') {
          threshold_value_conductivity = value_string.toInt();
        } else if (payload == '2') {
          time_peristaltic_pump_on = value_string.toInt();
        } else if (payload == '3') {
          time_between_sensor_reads = value_string.toInt();
        } else if (payload == '4') {
          time_water_pump_cycle = value_string.toInt();
        } else if (payload == '5') {
          time_water_pump_active = value_string.toInt();
        }

        save_configuration(payload);
      }

      send_current_status();

      break;
      /*    case WS_EVT_PONG:
          case WS_EVT_ERROR:
            break;
      */
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}
