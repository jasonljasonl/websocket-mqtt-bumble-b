let currentSpeed = 95;
const maxSpeed = 4095;
const minSpeed = 95;
const step = 1000;
const maxSpeedMS = 0.56; // Vitesse max en m/s

function calculateSpeedMS(joystickValue) {
  return (joystickValue / maxSpeed) * maxSpeedMS;
}

function updateSpeedDisplay(joystickValue) {
  let speedMS = calculateSpeedMS(joystickValue);
  document.getElementById("vitesseVoiture").innerText = `${speedMS.toFixed(2)} m/s`;
}

// Envoi de la vitesse à Node-RED
function sendSpeedToNodeRed(speed) {
  fetch('http://localhost:1880/speed', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify({ speed: speed })
  })
  .then(response => response.json())
  .then(data => console.log('Success:', data))
  .catch((error) => console.error('Error:', error));
}

function increaseSpeedLevel() {
  if (currentSpeed + step <= maxSpeed) {
    currentSpeed += step;
    updateSpeedDisplay(currentSpeed);

    // Envoyer la vitesse mise à jour à Node-RED
    sendSpeedToNodeRed(calculateSpeedMS(currentSpeed));
  }
  console.log('Vitesse augmentée à :', currentSpeed);
}

function decreaseSpeedLevel() {
  if (currentSpeed - step >= minSpeed) {
    currentSpeed -= step;
    updateSpeedDisplay(currentSpeed);

    // Envoyer la vitesse mise à jour à Node-RED
    sendSpeedToNodeRed(calculateSpeedMS(currentSpeed));
  }
  console.log('Vitesse réduite à :', currentSpeed);
}
