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
  sendToMysql(raceData);
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
  }
  


  const raceData = {
    id: 2,
    averageSpeed: totalSpeed,
    distanceCovered: 46,
    timeSpent: raceDuration,
    wheelRotationSpeed: 71
  };


  function sendToMysql(raceData) {
    // URL du backend
    const url = 'http://localhost:8080/races';
  
    // Token d'authentification (à remplacer par votre token)
    const token = 'eyJhbGciOiJIUzM4NCJ9.eyJzdWIiOiJ0ZXN0MSIsImlhdCI6MTcyNTM1NTE0MCwiZXhwIjoxNzI1NDQxNTQwfQ.tAhLVI2z1_cH45vey4LVzBxQDmt-UevkyxVgC2MdA3DMefI57LsYKA30MvV1iwp8';
  
    // Configuration de la requête fetch
    fetch(url, {
      method: 'POST', // Méthode d'envoi
      headers: {
        'Authorization': `Bearer ${token}`, // Ajout du token JWT dans l'en-tête Authorization
        'Content-Type': 'application/json' // Le contenu envoyé est du JSON
      },
      body: JSON.stringify(raceData) // Corps de la requête contenant les données de la course
    })
    .then(response => {
      if (!response.ok) {
        throw new Error('Network response was not ok');
      }
      return response.json(); // Transformation de la réponse en JSON
    })
    .then(json => {
      console.log('Données envoyées avec succès :', json); // Affichage de la réponse du serveur
    })
    .catch(error => {
      console.error('Erreur lors de l\'envoi des données :', error); // Gestion des erreurs
    });
  }
  