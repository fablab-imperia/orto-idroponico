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
