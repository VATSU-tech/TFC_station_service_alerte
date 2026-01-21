const stations = new Map();

function registerStation(ws, data) {
  ws.stationId = data.stationId;
  stations.set(data.stationId, ws);

  console.log(`Station enregistrée : ${data.stationId}`);
}

function removeStation(ws) {
  if (ws.stationId) {
    stations.delete(ws.stationId);
    console.log(`Station déconnectée : ${ws.stationId}`);
  }
}

function getStations() {
  return stations;
}

module.exports = {
  registerStation,
  removeStation,
  getStations
};
