window.addEventListener('load', onLoad);

// Initialise les fonctions au chargement du script
function onLoad() {
  // Initialise la fonction du websocket
  initWebSocket();
  // Initialise la fonction du joystick des roues
  initJoystick();
  // Initialise la fonction du joystick de la caméra
  initJoystickCam();
  // Initialise la vitesse affichée à 0 
  updateSpeedDisplay(0); 
  // Initialise la position du joystick à 0
  updateJoystickOrientation(0, 0); 
}
