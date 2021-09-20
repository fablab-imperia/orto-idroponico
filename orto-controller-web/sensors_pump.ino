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

  phValues = phProbe.getAverageValue();
  condValues = conductivityProbe.getAverageValue();

  char timeString[20];
  lastswitch = millis();
  convertMillisToTimeString(timeString, 20, lastswitch);

  // If PH value is found above threshold
  if (phValues.value > THRESHOLD_VALUE_PH)
  {
    Serial.println("---------PH LEVEL OVER THRESHOLD: NEED TO START ACID PUMP");
    //start acid pump
    acidPump.turnOn();
    Serial.print("---------ACID PUMP STARTS at ");
    Serial.println(timeString);
    //schedule a task for stopping the pump after specific number of seconds
    delay(TIME_PERISTALTIC_PUMP_ACID_ON);  // TODO: manage delay in loop with non-blocking code
    acidPumpOff();
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


  // If conductivity value is found below threshold
  if (condValues.value < THRESHOLD_VALUE_CONDUCTIVITY)
  {
    Serial.println("---------CONDUCTIVITY LEVEL UNDER THRESHOLD: NEED TO START FERTILIZER PUMP");
    //start fertilizer pump
    fertilizerPump.turnOn();
    Serial.print("---------FERTILIZER PUMP STARTS at ");
    Serial.println(timeString);
    delay(TIME_PERISTALTIC_PUMP_FERTILIZER_ON); // TODO: manage delay in loop with non-blocking code
    acidPumpOff();
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
  lastdisplay = millis();

  AnalogValues phValues;
  AnalogValues condValues;
  double temperatureC = 0.0;

  phValues = phProbe.getAverageValue();
  condValues = conductivityProbe.getAverageValue();
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
  convertMillisToShortTimeString(lastimeString, 20, millis() - lastswitch);
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


  char payload[50];
  //  JsonObject json = jsonBuffer.to<JsonObject>();
  doc["temp"] = temperatureC + random(-10, 10);
  doc["ph"] = phValues.value + random(-5, 5);
  doc["cond"] = condValues.value + random(0, 2000);

  serializeJson(doc, payload);

  notifyClients(payload);
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
