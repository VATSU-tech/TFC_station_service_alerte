# Système d'Alerte pour Stations de Service TFC - Documentation Développeur

## Vue d'ensemble

Ce projet implémente un système distribué d'alerte pour stations de service TFC utilisant des microcontrôleurs ESP8266 pour la détection de gaz et un serveur WebSocket Node.js pour la communication en temps réel. Le système permet la détection locale d'anomalies gazeuses et la propagation d'alertes entre stations connectées via Internet.

## Architecture

### Composants principaux

1. **Stations clientes (ESP8266)** :
   - Détection analogique de gaz via capteur MQ-135 ou similaire.
   - Communication WebSocket sécurisée (WSS) avec le serveur central.
   - Interface utilisateur locale : LED et buzzer pour alertes visuelles/sonores.

2. **Serveur central (Node.js + Express + WebSocket)** :
   - Gestion des connexions WebSocket.
   - Routage des messages d'alerte entre stations.
   - Hébergement sur Render (PaaS) avec SSL automatique.

3. **Protocole de communication** :
   - JSON over WebSocket pour tous les messages.
   - Types de messages : `register`, `registered`, `alert`.

### Flux de données

```
ESP8266 → WiFi → Internet → Render (WSS) → Diffusion aux autres ESP8266
```

## Configuration et Déploiement

### ESP8266 (Arduino)

#### Dépendances

- **Bibliothèques Arduino** :
  - `ESP8266WiFi` (version 1.0) : Gestion WiFi.
  - `WebSockets` (version 2.7.2) : Client WebSocket avec support SSL.

#### Configuration matérielle

- **Broches** :
  - A0 : Entrée analogique capteur gaz.
  - GPIO 2 : LED d'alerte (active LOW).
  - GPIO 4 : Buzzer (active HIGH).
- **Alimentation** : 3.3V/5V via USB ou batterie.

#### Configuration logicielle

```cpp
// WiFi
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Serveur
const char* serverHost = "tfc-station-service-alerte.onrender.com";
const int serverPort = 443;
const uint8_t sslFingerprint[20] = {0xA8, 0xEE, 0x46, 0x11, 0x10, 0x0C, 0x0E, 0x7D, 0x4E, 0x9D, 0x25, 0xEB, 0x63, 0x50, 0x68, 0x30, 0x45, 0x91, 0x6B, 0x28};

// Station
String stationId = "Station_A";  // Unique par carte
```

#### Logique principale

- **Setup** : Connexion WiFi, initialisation WebSocket SSL avec fingerprint.
- **Loop** :
  - Maintenance WebSocket.
  - Reconnexion automatique WiFi/WebSocket.
  - Lecture capteur (seuil > 50).
  - Envoi alerte si enregistré et délai écoulé (15s).

#### Gestion des événements WebSocket

```cpp
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      // Envoi inscription
      webSocket.sendTXT("{\"type\":\"register\",\"stationId\":\"" + stationId + "\"}");
      break;
    case WStype_TEXT:
      // Parsing JSON et gestion alertes
      if (message.indexOf("\"type\":\"registered\"") != -1) isRegistered = true;
      else if (message.indexOf("\"type\":\"alert\"") != -1) {
        // Activation LED/buzzer
      }
      break;
    // ...
  }
}
```

### Serveur Node.js

#### Dépendances

- **express** : Framework web.
- **ws** : Implémentation WebSocket.
- **https/fs** : Support SSL (optionnel, géré par Render).

#### Structure du code

```javascript
const express = require('express');
const WebSocket = require('ws');

const app = express();
const server = app.listen(process.env.PORT || 3000);
const wss = new WebSocket.Server({ server });

let connectedStations = new Map();  // ws → {stationId, ip}

wss.on('connection', (ws, req) => {
  ws.on('message', (message) => {
    const data = JSON.parse(message);
    if (data.type === 'register') {
      connectedStations.set(ws, { stationId: data.stationId, ip: req.socket.remoteAddress });
      ws.send(JSON.stringify({ type: 'registered', stationId: data.stationId }));
    } else if (data.type === 'alert') {
      // Diffusion à tous sauf émetteur
      wss.clients.forEach(client => {
        if (client !== ws && client.readyState === WebSocket.OPEN) {
          client.send(JSON.stringify({ type: 'alert', from: data.stationId, alert: data.alert }));
        }
      });
    }
  });

  ws.on('close', () => {
    connectedStations.delete(ws);
  });
});

app.get('/', (req, res) => res.send('Serveur WebSocket en ligne...'));
```

#### Déploiement sur Render

- **Service** : Web Service.
- **Runtime** : Node.js.
- **Build Command** : `npm install`.
- **Start Command** : `node serverOnline.js`.
- **Environment** : PORT (auto-assigné par Render).
- **SSL** : Automatique via Render (proxy HTTPS vers HTTP interne).

## Protocole de messages

### Format JSON

Tous les messages sont des objets JSON stringifiés.

#### Enregistrement de station

**Client → Serveur**

```json
{
  "type": "register",
  "stationId": "Station_A"
}
```

**Serveur → Client**

```json
{
  "type": "registered",
  "stationId": "Station_A"
}
```

#### Alerte

**Client → Serveur**

```json
{
  "type": "alert",
  "stationId": "Station_A",
  "alert": "Gaz détecté"
}
```

**Serveur → Clients**

```json
{
  "type": "alert",
  "from": "Station_A",
  "alert": "Gaz détecté"
}
```

## Sécurité

- **SSL/TLS** : Connexions WSS avec vérification fingerprint (ESP8266) et certificat (Render).
- **Authentification** : Basée sur stationId (non crypté, améliorable avec tokens).
- **Pare-feu** : Dépend de Render et du réseau local.

## Limitations et améliorations

### Limitations actuelles

- Pas de persistance des données (alertes non stockées).
- Pas d'authentification forte.
- Détection gaz basique (seuil fixe).
- Pas de gestion des conflits d'ID station.

### Améliorations possibles

- **Base de données** : MongoDB/PostgreSQL pour logs d'alertes.
- **Authentification** : JWT ou API keys.
- **Monitoring** : Dashboard web avec Socket.IO.
- **Capteurs avancés** : Calibration automatique, multi-gaz.
- **Fiabilité** : Retry logic, heartbeat WebSocket.
- **Scalabilité** : Load balancing si >100 stations.

## Tests et débogage

### ESP8266

- **Moniteur série** : Logs de connexion, envoi/réception.
- **Simulation gaz** : Court-circuit A0 à 3.3V pour test.
- **WiFi** : Vérifier stabilité réseau.

### Serveur

- **Logs Render** : Console pour connexions/messages.
- **Test local** : `node serverOnline.js` + client WebSocket (ex. wscat).
- **Endpoint health** : GET / retourne status.

## Contribution

- Fork le repo, branche feature, PR.
- Respecter style Arduino/Node.js standard.
- Tests unitaires pour logique serveur.

## Licence

MIT - Libre utilisation/modification.
