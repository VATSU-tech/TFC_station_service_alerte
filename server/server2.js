const express = require('express');
const WebSocket = require('ws');

const app = express();
const PORT = 3000;

// Serveur HTTP
const server = app.listen(PORT, () => {
  console.log(`✅ Serveur HTTP lancé sur http://localhost:${PORT}`);
});

// Serveur WebSocket
const wss = new WebSocket.Server({ server });

// Liste des stations connectées
let connectedStations = new Map(); 
// Map: ws → stationId

wss.on('connection', (ws, req) => {
  const clientIP = req.socket.remoteAddress;
  console.log(`📡 Nouvelle connexion détectée depuis ${clientIP}`);

  // Quand un message arrive
  ws.on('message', (message) => {
    try {
      const data = JSON.parse(message);

      // Si c'est un message d'enregistrement (stationId)
      if (data.type === "register") {
        connectedStations.set(ws, data.stationId);
        console.log(`✅ Station connectée: ${data.stationId} (IP: ${clientIP})`);
      }

      // Si c'est une alerte
      else if (data.type === "alert") {
        const stationId = connectedStations.get(ws) || "Inconnue";
        console.log(`🚨 Alerte de ${stationId}: ${data.alert}`);

        // Diffuser aux autres stations
        wss.clients.forEach(client => {
          if (client !== ws && client.readyState === WebSocket.OPEN) {
            client.send(JSON.stringify({
              from: stationId,
              alert: data.alert
            }));
          }
        });
      }
    } catch (err) {
      console.error("❌ Erreur de parsing:", err);
    }
  });

  // Quand une station se déconnecte
  ws.on('close', () => {
    const stationId = connectedStations.get(ws) || "Inconnue";
    console.log(`❌ Station déconnectée: ${stationId} (IP: ${clientIP})`);
    connectedStations.delete(ws);
  });
});
