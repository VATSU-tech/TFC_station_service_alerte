#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

// === À MODIFIER POUR CHAQUE STATION ===
#define STATION_ID "STATION_B"
#define led LED_BUILTIN

// WiFi02
const char* ssid = "Airtel_3031";
const char* password = "123456789000";

// Serveur
const char* ws_host = "192.168.0.103";
const uint16_t ws_port = 8080;

WebSocketsClient webSocket;

void blink(int temp = 1000){
  digitalWrite(led, false);
  delay(temp);
  digitalWrite(led,true);
  delay(temp);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  if (type == WStype_CONNECTED) {
    Serial.println("Connecté au serveur");
    blink();

    JsonDocument doc;
    doc["type"] = "REGISTER";
    doc["stationId"] = STATION_ID;

    String msg;
    serializeJson(doc, msg);
    webSocket.sendTXT(msg);
  }

  if (type == WStype_TEXT) {
    JsonDocument doc;
    deserializeJson(doc, payload);
    blink(5000);

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
  pinMode(led,OUTPUT);
  digitalWrite(led,true);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.print("Adresse Ip : ");
  Serial.println(WiFi.localIP());
  Serial.print("Adresse Ip server : ");
  Serial.print(ws_host);
  Serial.print(":");
  Serial.println(ws_port);

  webSocket.begin(ws_host, ws_port, "/");
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();

  // SIMULATION D’UN DANGER
  if(analogRead(A0) > 100){
  // if (digitalRead(D5) == HIGH) {
    envoyerAlerte("GAS_LEAK", "CRITICAL");
  Serial.println(analogRead(A0));
    delay(5000);
  }
}

void envoyerAlerte(const char* danger, const char* severity) {
  JsonDocument doc;
  doc["type"] = "ALERT";
  doc["stationId"] = STATION_ID;
  doc["danger"] = danger;
  doc["severity"] = severity;

  String msg;
  serializeJson(doc, msg);
  Serial.println(msg);
  webSocket.sendTXT(msg);
}
