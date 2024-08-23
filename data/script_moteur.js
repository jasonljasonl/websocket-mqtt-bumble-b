// Variable de la vitesse (définit à 95 par défaut)
let currentSpeed = 95;
// Variable de la vitesse MAX (vitesse MAX des moteurs, équivalent à 165tr/min)
const maxSpeed = 4095;
// Variable de la vitesse MIN (moteurs pas assez puissant pour démarrer la voiture, équivaut à 0)
const minSpeed = 95;
// Valeur qui incrémentera la vitesse max à chaque appuis de boutons
// 95 / 1095 / 2095 / 3095 / 4095 
const step = 1000;
// Vitesse max en m/s
const maxSpeedMS = 0.56;

// Calcule la vitesse actuelle de la voiture grâce à la position du joystick, de la valeurs max des moteurs et la vitesse max en M/S
function calculateSpeedMS(joystickValue) {
  return (joystickValue / maxSpeed) * maxSpeedMS;
}

// Récupère l'élément qui affiche la vitesse gràace a son ID et la met a jour à chaque fois que la vitesse change
function updateSpeedDisplay(joystickValue) {
  let speedMS = calculateSpeedMS(joystickValue);
  document.getElementById("vitesseVoiture").innerText = `${speedMS.toFixed(2)} m/s`;
}

// Augmente la vitesse maximale du régulateur de vitesse
function increaseSpeedLevel() {
  // Si la vitesse actuelle + la valeur d'incrémentation est inférieur à la vitesse max
  if (currentSpeed + step <= maxSpeed) {
    // augmente la vitesse max du régulateur de vitesse
    currentSpeed += step;
    // Met a jour l'élément qui affiche la vitesse en affichant la nouvelle vitesse max du régulateur
    updateSpeedDisplay(currentSpeed);
  }
  // Affiche dans la console l'action effectuée
  console.log('Vitesse augmentée à :', currentSpeed);
}

// Réduit la vitesse maximale de la voiture
function decreaseSpeedLevel() {
  // Si la vitesse actuelle - la valeur d'incrémentation est supérieur à la vitesse min
  if (currentSpeed - step >= minSpeed) {
    // réduit la vitesse max du régulateur de vitesse
    currentSpeed -= step;
    // Met a jour l'élément qui affiche la vitesse en affichant la nouvelle vitesse max du régulateur
    updateSpeedDisplay(currentSpeed);
  }
  // Affiche dans la console l'action effectuée
  console.log('Vitesse réduite à :', currentSpeed);
}
