// Import des modules nécessaires
const express = require('express');
const WebSocket = require('ws');

const app = express();
const PORT = 3000;

// Création du serveur HTTP
const server = app.listen(PORT, () => {
  console.log(`✅ Serveur HTTP lancé sur http://localhost:${PORT}`);
});

// Création du serveur WebSocket attaché au serveur HTTP
const wss = new WebSocket.Server({ server });

// Quand une station se connecte
wss.on('connection', (ws) => {
  console.log("📡 Nouvelle station connectée");

  // Quand une station envoie un message (alerte)
  ws.on('message', (message) => {
    console.log("🚨 Alerte reçue:", message);

    // Diffuser l’alerte à toutes les autres stations
    wss.clients.forEach(client => {
      if (client !== ws && client.readyState === WebSocket.OPEN) {
        client.send(message);
      }
    });
  });

  // Quand une station se déconnecte
  ws.on('close', () => {
    console.log("❌ Station déconnectée");
  });
});
