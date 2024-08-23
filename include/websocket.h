#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// CSS de la page
const char styles_css[] PROGMEM = R"rawliteral(
html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
    background-color: #020617;
    color: white;
  }

  h2 {
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top: 10px;
    padding-bottom: 20px;
  }
  .state {
    font-size: 1.5rem;
    color: #8c8c8c;
    font-weight: bold;
  }
  #camera-stream {
    transform: rotate(180deg);
    width:100%;

  }

  #camera-stream img {
    width:100%;
    height:100%;
  }

  #joystick {
    width: 200px;
    height: 200px;
    margin: 20px auto;
    position: relative;
  }
  .back {
    opacity: 0.8;
    background-color: #1E293B
  }
  #joystickCommandes {
    display:flex;
    width:100%;
  }
  #joystick {
  padding: 50px;
  }
  #joystickCam {
  padding : 150px;
  }
  #vitesseVoiture {
  font-size:33px;
  margin:0 16px 0 16px;
  }
  #titre-vitesse {
  font-size:17px;
  }
  #div-vitesse {
  display:flex;
  align-items:center;
  justify-content:center;
  }


)rawliteral";

// Intégration de la page en HTML
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Bumble-B Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <link rel="stylesheet" href="/styles.css">
</head>
<body>

  <div id="camera-stream">
    <img src="http://172.20.10.3:81/stream">
  </div>


    <div>
      <p id="titre-vitesse">Vitesse</p>
      <!-- Mise à jour ici pour afficher en m/s -->
    
    <div id ="div-vitesse">
      <button onclick="increaseSpeedLevel()" class="regulateur-vitesse">+</button>
      <p id="vitesseVoiture">0 m/s</p>
      <button onclick="decreaseSpeedLevel()" class="regulateur-vitesse">-</button>
    </div>


      <p>Orientation du joystick :</p>
      <p id="joystickOrientation">Neutre</p> <!-- Affichage de l'orientation -->
    </div>

  </div>

  <div class="content">
    <div id="joystickCommandes">
      <div id="joystick"></div>
      <div id="joystickCam"></div>
    </div>


  <div id="timer">00:00:00</div>
  <button id="startRace" onclick="startRace()">Démarrer la course</button>
  <button id="stopRace" onclick="stopRace()" disabled>Arrêter la course</button>


  <!-- Inclure nipplejs -->
  <script src="https://cdnjs.cloudflare.com/ajax/libs/nipplejs/0.8.6/nipplejs.min.js"></script>



<script src="/script_joystick_roues.js"></script>
<script src="/script_joystick_camera.js"></script>
<script src="/script_main.js"></script>
<script src="/script_moteur.js"></script>
<script src="/script_websocket.js"></script>
<script src="/script_chronometre.js"></script>


</body>
</html>
)rawliteral";
