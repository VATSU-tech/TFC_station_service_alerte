#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

// Configuration WiFi
const char* ssid = "Airtel_3031";
const char* password = "123456789000";

// Configuration WebSocket
WebSocketsClient webSocket;
const char* serverHost = "192.168.0.103";
const int serverPort = 3000;

// Identifiant unique de la station (changez pour chaque carte)
String stationId = "Station_B";

// Pin pour la LED d'alerte
#define LED_PIN 2
#define BUZZER_PIN 4  // Ajoutez un buzzer si disponible

// État de connexion
bool isRegistered = false;

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED: {
      Serial.println("✅ Connecté au serveur WebSocket");
      // Enregistrer la station
      String registerMsg = "{\"type\":\"register\",\"stationId\":\"" + stationId + "\"}";
      webSocket.sendTXT(registerMsg);
      Serial.println("📤 Enregistrement envoyé: " + registerMsg);
      break;
    }

    case WStype_TEXT: {
      // Créer une String sécurisée à partir du payload
      char temp[length + 1];
      memcpy(temp, payload, length);
      temp[length] = '\0';
      String message = temp;

      Serial.println("📩 Message reçu: " + message);

      // Parser le message JSON
      if (message.indexOf("\"type\":\"registered\"") != -1) {
        isRegistered = true;
        Serial.println("✅ Station enregistrée avec succès");
      } else if (message.indexOf("\"type\":\"alert\"") != -1) {
        // Alerte reçue d'une autre station
        Serial.println("🚨 Alerte reçue d'une autre station !");
        // Déclencher LED et buzzer
        digitalWrite(LED_PIN, false);
        digitalWrite(BUZZER_PIN, HIGH);
        delay(2000);  // Durée de l'alerte
        digitalWrite(LED_PIN, true);
        digitalWrite(BUZZER_PIN, LOW);
      }
      break;
    }

    case WStype_DISCONNECTED: {
      Serial.println("❌ Déconnecté du serveur WebSocket");
      isRegistered = false;
      break;
    }

    case WStype_ERROR: {
      Serial.println("❌ Erreur WebSocket");
      break;
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN, true);
  digitalWrite(BUZZER_PIN, LOW);

  // Connexion WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connexion WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" ✅ WiFi connecté ! IP: " + WiFi.localIP().toString());

  // Connexion WebSocket
  webSocket.begin(serverHost, serverPort, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);  // Reconnexion automatique
}

void loop() {
  webSocket.loop();

  // Vérifier et reconnecter WiFi si nécessaire
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi déconnecté, reconnexion...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
    Serial.println("WiFi reconnecté !");
  }

  // Vérifier et reconnecter WebSocket si nécessaire
  if (!webSocket.isConnected() && isRegistered) {
    Serial.println("WebSocket déconnecté, reconnexion...");
    webSocket.begin(serverHost, serverPort, "/");
  }

  // Détection de gaz et envoi d'alerte (toutes les 15 secondes si détecté)
  static unsigned long lastSend = 0;
  if (analogRead(A0) > 50 && isRegistered) {  // Seulement si enregistré
    if (millis() - lastSend > 15000) {
      String alert = "{\"type\":\"alert\",\"stationId\":\"" + stationId + "\",\"alert\":\"Gaz détecté\"}";
      webSocket.sendTXT(alert);
      Serial.println("🚨 Alerte envoyée: " + alert);
      lastSend = millis();
    }
  }

  // Petit délai pour éviter la surcharge
  delay(100);
}
