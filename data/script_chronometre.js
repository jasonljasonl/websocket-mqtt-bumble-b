let raceStartTime;
let raceEndTime;
let raceInterval;
let isRacing = false;
let totalSpeed = 0;
let speedCount = 0;
let raceDuration = 0; // in seconds

function formatTime(time) {
  const hours = String(Math.floor(time / 3600)).padStart(2, '0');
  const minutes = String(Math.floor((time % 3600) / 60)).padStart(2, '0');
  const seconds = String(time % 60).padStart(2, '0');
  return `${hours}:${minutes}:${seconds}`;
}

function startRace() {
  raceStartTime = Date.now();
  raceInterval = setInterval(updateRaceTimer, 1000);
  isRacing = true;

  document.getElementById("startRace").disabled = true;
  document.getElementById("stopRace").disabled = false;

  totalSpeed = 0;
  speedCount = 0;
  raceDuration = 0;
}

function stopRace() {
  clearInterval(raceInterval);
  raceEndTime = Date.now();
  isRacing = false;

  document.getElementById("startRace").disabled = false;
  document.getElementById("stopRace").disabled = true;

  calculateAverageSpeed();
}

function updateRaceTimer() {
  if (isRacing) {
    raceDuration = Math.floor((Date.now() - raceStartTime) / 1000);
    document.getElementById('timer').innerText = formatTime(raceDuration);
  }
}

  
  function calculateAverageSpeed() {
    const averageSpeed = totalSpeed / speedCount;
    console.log(`Vitesse moyenne : ${averageSpeed.toFixed(2)} m/s`);
  
    // Envoyer la vitesse moyenne à Node-RED ou à une base de données
    sendAverageSpeedToNodeRed(averageSpeed);
  }
  