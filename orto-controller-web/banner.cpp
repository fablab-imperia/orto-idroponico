#include "Arduino.h"
#include "banner.hpp"

void Banner::printSerial() {
  Serial.println("***********************   Orto IDROPONICO 2.0   **************************");

  // Write:     Orto Idroponico
  //            Fablab Imperia
  Serial.println(" _____      _          _____    _                             _           ");
  delay(100);
  Serial.println("|  _  |    | |        |_   _|  | |                           (_)          ");
  delay(100);
  Serial.println("| | | |_ __| |_ ___     | |  __| |_ __ ___  _ __   ___  _ __  _  ___ ___  ");
  delay(100);
  Serial.println("| | | | '__| __/ _ \\    | | / _` | '__/ _ \\| '_ \\ / _ \\| '_ \\| |/ __/ _ \\ ");
  delay(100);
  Serial.println("\\ \\_/ / |  | || (_) |  _| || (_| | | | (_) | |_) | (_) | | | | | (_| (_) |");
  delay(100);
  Serial.println(" \\___/|_|   \\__\\___/   \\___/\\__,_|_|  \\___/| .__/ \\___/|_| |_|_|\\___\\___/ ");
  delay(100);
  Serial.println("                                           | |                            ");
  delay(100);
  Serial.println("                                           |_|                            ");
  delay(100);
  Serial.println("______    _     _       _       _____                          _          ");
  delay(100);
  Serial.println("|  ___|  | |   | |     | |     |_   _|                        (_)         ");
  delay(100);
  Serial.println("| |_ __ _| |__ | | __ _| |__     | | _ __ ___  _ __   ___ _ __ _  __ _    ");
  delay(100);
  Serial.println("|  _/ _` | '_ \\| |/ _` | '_ \\    | || '_ ` _ \\| '_ \\ / _ \\ '__| |/ _` |   ");
  delay(100);
  Serial.println("| || (_| | |_) | | (_| | |_) |  _| || | | | | | |_) |  __/ |  | | (_| |   ");
  delay(100);
  Serial.println("\\_| \\__,_|_.__/|_|\\__,_|_.__/   \\___/_| |_| |_| .__/ \\___|_|  |_|\\__,_|   ");
  delay(100);
  Serial.println("                                              | |                         ");
  delay(100);
  Serial.println("                                              |_|                         ");
}
