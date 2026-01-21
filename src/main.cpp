#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MQTTClient.h>

// --- Configuration Wi-Fi ---
const char WIFI_SSID[] = "Airtel_3031";
const char WIFI_PASSWORD[] = "123456789000";

// --- Configuration MQTT ---
const char MQTT_BROKER_ADDRESS[] = "broker.freemqtt.com";
const int MQTT_PORT = 8084;
const char MQTT_CLIENT_ID[] = "esp8266-station-001";  

// --- Topics organisés ---
const char TOPIC_PUBLISH[]   = "station/temp";
const char TOPIC_SUBSCRIBE[] = "station/cmd";

const int PUBLISH_INTERVAL = 5000;  // 5 secondes

WiFiClient network;
MQTTClient mqtt(256);

unsigned long lastPublishTime = 0;

// --- Prototypes ---
void connectWiFi();
void connectMQTT();
void sendToMQTT();
void messageHandler(String &topic, String &payload);

void setup() { 
  Serial.begin(115200);
  ArduinoOTA.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // LED OFF
  connectWiFi();
  connectMQTT();
}

void loop() {
  ArduinoOTA.handle();
  // Maintenir la connexion MQTT
  mqtt.loop();

  // Vérifier la connexion Wi-Fi et MQTT
  if (WiFi.status() != WL_CONNECTED) connectWiFi();
  if (!mqtt.connected()) connectMQTT();

  // Publier périodiquement
  if (millis() - lastPublishTime > PUBLISH_INTERVAL) {
    sendToMQTT();
    lastPublishTime = millis();
  }
}

// --- Connexion Wi-Fi ---
void connectWiFi() {
  Serial.print("Connexion au Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connecté, IP: " + WiFi.localIP().toString());
}

// --- Connexion MQTT ---
void connectMQTT() {
  mqtt.begin(MQTT_BROKER_ADDRESS, MQTT_PORT, network);
  mqtt.onMessage(messageHandler);

  Serial.print("Connexion au broker MQTT...");
  while (!mqtt.connect(MQTT_CLIENT_ID)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nMQTT connecté !");

  // S'abonner au topic de commandes
  mqtt.subscribe(TOPIC_SUBSCRIBE);
  Serial.println("Abonné au topic: " + String(TOPIC_SUBSCRIBE));
}

// --- Publication ---
void sendToMQTT() {
  JsonDocument message;
  message["timestamp"] = millis();
  message["temperature"] = analogRead(A0); // Exemple: lecture capteur

  char buffer[256];
  serializeJson(message, buffer);

  mqtt.publish(TOPIC_PUBLISH, buffer);
  Serial.println("Message publié sur " + String(TOPIC_PUBLISH) + ": " + buffer);
}

// --- Réception ---
void messageHandler(String &topic, String &payload) {
  Serial.println("Message reçu:");
  Serial.println("Topic: " + topic);
  Serial.println("Payload: " + payload);

  // Exemple: si payload = "LED_ON", allumer une LED
  if (payload == "LED_ON") {
    digitalWrite(LED_BUILTIN, LOW); // LED ON
  } else if (payload == "LED_OFF") {
    digitalWrite(LED_BUILTIN, HIGH); // LED OFF
  }
}
