//Includes for Temperature sensor
#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(3);
//Temperature sensor
DallasTemperature temperatureSensor(&oneWire);

void setup() {
  Serial.begin(9600);
   temperatureSensor.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  double temperatureC = 0.0;
 temperatureSensor.requestTemperatures(); 
  temperatureC = temperatureSensor.getTempCByIndex(0);
    Serial.print("TEMPERATURE IS: ");
  Serial.println(temperatureC);
  delay(1000);
}
