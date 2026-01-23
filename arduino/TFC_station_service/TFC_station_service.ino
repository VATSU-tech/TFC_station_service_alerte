#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

const char* ssid = "Airtel_3031";
const char* password = "123456789000";

WebSocketsClient webSocket;
const char* serverHost = "192.168.0.103";
const int serverPort = 3000;

// Identifiant unique de la station
String stationId = "Station_A";  // 🔄 Change pour chaque carte

String Register = "{\"type\":\"register\",\"stationId\":\"" + stationId + "\"}";

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.println("✅ Connecté au serveur WebSocket");
      // Envoie un message de registre avec l'ID
      webSocket.sendTXT(Register);
      break;

    case WStype_TEXT:
      Serial.printf("📩 Message reçu: %s\n", payload);
      // Ici déclenche buzzer / LED / OLED
      break;

    case WStype_DISCONNECTED:
      Serial.println("❌ Déconnecté du serveur");
      break;
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connecté !");
  Serial.println(WiFi.localIP());

  webSocket.begin(serverHost, serverPort, "/");
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();

  // Exemple : envoyer une alerte toutes les 15s
  static unsigned long lastSend = 0;
  if (analogRead(A0) > 50) {
    if (millis() - lastSend > 15000) {
      String alert = "{\"type\":\"alert\",\"stationId\":\"" + stationId + "\",\"alert\":\"Gaz détecté\"}";
      webSocket.sendTXT(alert);
      Serial.println("🚨 Alerte envoyée: " + alert);
      lastSend = millis();
    }
  }
}
