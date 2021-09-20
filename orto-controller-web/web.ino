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

  initWebSocket();

  // Start server
  server.begin();

}




void notifyClients(String payload) {
  ws.textAll(payload);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

  }
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
      int i = 0;
      char payload = *(data + i);
      Serial.println(payload);

      if (payload == 'T') {
        manual_control = true;
        // TODO
      } else if (payload == 'F') {
        manual_control = false;
        // TODO
      } else if (payload == 'F') {
        fertilizerPump.turnOn();
        manual_peristaltic_active = true;
        lastswitch = millis();
      } else if (payload == 'A') {
        acidPump.turnOn();
        manual_peristaltic_active = true;
        lastswitch = millis();
      } else if (payload == 'P') {
        // TODO
      } else if (payload == 'f') {
        fertilizerPump.turnOff();
        manual_peristaltic_active = false;
        lastswitch = millis();
      } else if (payload == 'a') {
        acidPump.turnOff();
        manual_peristaltic_active = false;
        lastswitch = millis();
      } else if (payload == 'p') {
        // TODO
      }
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
