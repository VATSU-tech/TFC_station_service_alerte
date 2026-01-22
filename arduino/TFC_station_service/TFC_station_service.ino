#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

// === À MODIFIER POUR CHAQUE STATION ===
#define STATION_ID "STATION_A"

// WiFi02
const char* ssid = "Airtel_3031";
const char* password = "12345678900";

// Serveur
const char* ws_host = "192.168.0.200";
const uint16_t ws_port = 8080;

WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  if (type == WStype_CONNECTED) {
    Serial.println("Connecté au serveur");

    StaticJsonDocument<200> doc;
    doc["type"] = "REGISTER";
    doc["stationId"] = STATION_ID;

    String msg;
    serializeJson(doc, msg);
    webSocket.sendTXT(msg);
  }

  if (type == WStype_TEXT) {
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload);

    if (doc["type"] == "REMOTE_ALERT") {
      Serial.println("ALERTE DISTANTE RECUE");
      Serial.println(doc["source"].as<String>());

      // ICI → mesures préventives
      // stopPompes();
      // allumerGyro();
    }
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  webSocket.begin(ws_host, ws_port, "/");
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();

  // SIMULATION D’UN DANGER
  if (digitalRead(D5) == HIGH) {
    envoyerAlerte("GAS_LEAK", "CRITICAL");
    delay(5000);
  }
}

void envoyerAlerte(const char* danger, const char* severity) {
  StaticJsonDocument<256> doc;
  doc["type"] = "ALERT";
  doc["stationId"] = STATION_ID;
  doc["danger"] = danger;
  doc["severity"] = severity;

  String msg;
  serializeJson(doc, msg);
  webSocket.sendTXT(msg);
}
