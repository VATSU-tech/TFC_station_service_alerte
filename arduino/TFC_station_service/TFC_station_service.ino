#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

// ⚙️ Paramètres réseau
const char* ssid = "Airtel_3031";       // Nom du WiFi
const char* password = "123456789000";    // Mot de passe WiFi

// ⚙️ Paramètres serveur
WebSocketsClient webSocket;
const char* serverHost = "192.168.0.103"; // IP du serveur Node.js
const int serverPort = 3000;

// ⚙️ Identifiant unique de la station
String stationId = "Station_A"; // 🔄 Change ce nom pour chaque carte

// Fonction appelée quand un message est reçu du serveur
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_CONNECTED:
      Serial.println("✅ Connecté au serveur WebSocket");
      break;

    case WStype_TEXT:
      Serial.printf("📩 Message reçu: %s\n", payload);
      // Ici tu déclenches buzzer / LED / OLED
      break;

    case WStype_DISCONNECTED:
      Serial.println("❌ Déconnecté du serveur");
      break;
  }
}

void setup() {
  Serial.begin(115200);

  // Connexion au WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connecté !");

  // Connexion au serveur WebSocket
  webSocket.begin(serverHost, serverPort, "/");
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
  
  if (analogRead(A0) > 50) {
    String alert = "{\"stationId\":\"" + stationId + "\",\"alert\":\"Gaz détecté\"}";
    webSocket.sendTXT(alert);
    Serial.println("🚨 Alerte envoyée: " + alert);
    delay(1000);
  }
}
