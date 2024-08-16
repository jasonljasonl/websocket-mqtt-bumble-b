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

function increaseSpeedLevel() {
  if (currentSpeed + step <= maxSpeed) {
    currentSpeed += step;
    updateSpeedDisplay(currentSpeed);
  }
  console.log('Vitesse augmentée à :', currentSpeed);
}

function decreaseSpeedLevel() {
  if (currentSpeed - step >= minSpeed) {
    currentSpeed -= step;
    updateSpeedDisplay(currentSpeed);
  }
  console.log('Vitesse réduite à :', currentSpeed);
}
