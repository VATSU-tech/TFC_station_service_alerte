const { getStations } = require('./stations');
const { v4: uuidv4 } = require('uuid');

function handleAlert(ws, data) {
  const eventId = uuidv4();

  console.log(`ALERTE de ${data.stationId} : ${data.danger}`);

  const stations = getStations();

  for (const [stationId, client] of stations) {
    if (stationId !== data.stationId) {
      client.send(JSON.stringify({
        type: 'REMOTE_ALERT',
        eventId,
        source: data.stationId,
        danger: data.danger,
        severity: data.severity,
        timestamp: Date.now()
      }));
    }
  }
}

module.exports = { handleAlert };
