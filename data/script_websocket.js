// Variable du lien du serveur websocket
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

// Initialisation du serveur websocket
function initWebSocket() {
  console.log('Trying to open a WebSocket connection...');
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
}

function onOpen(event) {
  // Indique à l'utilisateur quand le websocket est ouvert
  console.log('Connection opened');
}


function onClose(event) {
  // Indique à l'utilisateur quand le websocket est fermé
  console.log('Connection closed');
  // et retente la connexion après 2000 millisecondes
  setTimeout(initWebSocket, 2000);
}


// Fonction qui enverra les commandes vers le websocket (communication avec la voiture)
function sendCommand(cmd, data) {
  const command = {
    // cmd = clé du json
    cmd: cmd,
    // data = valeur de la clé
    data: data
    // Exemple : cmd : "1" / data : [2000,2000,2000,2000] = "clé pointant les moteurs" , [valeur de la vitesse des moteurs]
  };
  // Envoi les commandes sous forme de chaîne de caractère
  websocket.send(JSON.stringify(command));
}