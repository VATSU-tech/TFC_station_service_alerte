const express = require('express');
const WebSocket = require('ws');

const app = express();
const PORT = 3000;

// Serveur HTTP
const server = app.listen(PORT, () => {
  console.log(`Serveur HTTP lancé sur http://localhost:${PORT}`);
});

// Serveur WebSocket
const wss = new WebSocket.Server({ server });

// Liste des stations connectées : Map ws → {stationId, ip}
let connectedStations = new Map();

wss.on('connection', (ws, req) => {
  const clientIP = req.socket.remoteAddress;
  console.log(`Nouvelle connexion WebSocket depuis ${clientIP}`);

  // Quand un message arrive
  ws.on('message', (message) => {
    try {
      const data = JSON.parse(message.toString());

      // Enregistrement de la station
      if (data.type === "register") {
        const stationId = data.stationId;
        connectedStations.set(ws, { stationId, ip: clientIP });
        console.log(`Station enregistrée: ${stationId} (IP: ${clientIP})`);

        // Confirmer l'enregistrement à la station
        ws.send(JSON.stringify({ type: "registered", stationId }));
      }

      // Alerte reçue d'une station
      else if (data.type === "alert") {
        const stationInfo = connectedStations.get(ws);
        if (!stationInfo) {
          console.log(`Alerte d'une station inconnue (IP: ${clientIP}): ${data.alert}`);
          return;
        }
        const { stationId } = stationInfo;
        console.log(`Alerte de ${stationId}: ${data.alert}`);

        // Diffuser l'alerte à toutes les autres stations connectées
        wss.clients.forEach(client => {
          if (client !== ws && client.readyState === WebSocket.OPEN) {
            client.send(JSON.stringify({
              type: "alert",
              from: stationId,
              alert: data.alert
            }));
          }
        });
      }
    } catch (err) {
      console.error(`Erreur de parsing du message depuis ${clientIP}:`, err);
    }
  });

  // Quand une station se déconnecte
  ws.on('close', () => {
    const stationInfo = connectedStations.get(ws);
    if (stationInfo) {
      const { stationId, ip } = stationInfo;
      console.log(`Station déconnectée: ${stationId} (IP: ${ip})`);
      connectedStations.delete(ws);
    } else {
      console.log(`Connexion inconnue fermée (IP: ${clientIP})`);
    }
  });

  // Gestion des erreurs
  ws.on('error', (err) => {
    console.error(`Erreur WebSocket depuis ${clientIP}:`, err);
  });
});

// Route simple pour vérifier le serveur
app.get('/', (req, res) => {
  res.send('Serveur WebSocket pour stations d\'alerte actif');
});
 