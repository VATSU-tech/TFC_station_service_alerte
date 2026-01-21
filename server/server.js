const WebSocket = require('ws');
const { registerStation, removeStation, getStations } = require('./stations');
const { handleAlert } = require('./alerts');

const wss = new WebSocket.Server({ port: 8080 });

console.log('Serveur WebSocket démarré sur le port 8080');

wss.on('connection', (ws) => {

  console.log('Nouvelle connexion');

  ws.on('message', (message) => {
    try {
      const data = JSON.parse(message.toString());

      if (data.type === 'REGISTER') {
        registerStation(ws, data);
      }

      if (data.type === 'ALERT') {
        handleAlert(ws, data);
      }

      if (data.type === 'HEARTBEAT') {
        ws.lastSeen = Date.now();
      }

    } catch (err) {
      console.error('Message invalide', err);
    }
  });

  ws.on('close', () => {
    removeStation(ws);
  });
});
