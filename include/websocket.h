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


  <!-- Inclure nipplejs -->
  <script src="https://cdnjs.cloudflare.com/ajax/libs/nipplejs/0.8.6/nipplejs.min.js"></script>

  <!-- Script JavaScript pour le contrôle de la voiture -->
  <script>
    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;
    window.addEventListener('load', onLoad);

    function initWebSocket() {
      console.log('Trying to open a WebSocket connection...');
      websocket = new WebSocket(gateway);
      websocket.onopen = onOpen;
      websocket.onclose = onClose;
    }

    function onOpen(event) {
      console.log('Connection opened');
    }

    function onClose(event) {
      console.log('Connection closed');
      setTimeout(initWebSocket, 2000);
    }

    let currentSpeed = 95;
    const maxSpeed = 4095;
    const minSpeed = 95;
    const step = 1000;
    const maxSpeedMS = 0.56; // Vitesse max en m/s
    let currentCameraVertical = 90;
    let currentCameraHorizontal = 90;
    const cameraMaxAngle = 140;
    const cameraMinAngle = 40;

    function calculateSpeedMS(joystickValue) {
      return (joystickValue / maxSpeed) * maxSpeedMS;
    }

    function updateSpeed(speedArray) {
      const command = {
        cmd: 1,
        data: speedArray
      };
      websocket.send(JSON.stringify(command));
    }

    function updateSpeedDisplay(joystickValue) {
      let speedMS = calculateSpeedMS(joystickValue);
      document.getElementById("vitesseVoiture").innerText = `${speedMS.toFixed(2)} m/s`;
    }

    function updateJoystickOrientation(forward, right) {
      let orientation = 'Neutre';
      if (forward > 0.5) {
        orientation = 'Avant';
      } else if (forward < -0.5) {
        orientation = 'Arrière';
      }
      if (right > 0.5) {
        orientation += ' Droite';
      } else if (right < -0.5) {
        orientation += ' Gauche';
      }
      document.getElementById("joystickOrientation").innerText = orientation;
    }

    function updateCamera(verticalAngle, horizontalAngle) {
      const command = {
        cmd: 3,
        data: [verticalAngle, horizontalAngle]
      };
      websocket.send(JSON.stringify(command));
    }

    function onLoad(event) {
      initWebSocket();
      initJoystick();
      initJoystickCam();
      updateSpeedDisplay(0); // Initial display
      updateJoystickOrientation(0, 0); // Initial orientation display
    }

    function initJoystick() {
      var options = {
        zone: document.getElementById('joystick'),
        mode: 'static',
        position: { left: '100%', top: '50%' },
        color: 'blue',
      };
      var manager = nipplejs.create(options);

      manager.on('move', function (evt, data) {
        var forward = data.vector.y;
        var right = data.vector.x;

        var speedLeft = Math.round(currentSpeed * forward);
        var speedRight = Math.round(currentSpeed * forward);

        if (right > 0) {
          speedLeft = Math.round(currentSpeed * forward * (1 - right));
        } else if (right < 0) {
          speedRight = Math.round(currentSpeed * forward * (1 + right));
        }

        let joystickValue = Math.max(speedLeft, speedRight); // Assuming max speed
        updateSpeedDisplay(joystickValue); // Update display with proportional speed
        updateJoystickOrientation(forward, right); // Update orientation display
        updateSpeed([speedLeft, speedLeft, speedRight, speedRight]);
      });

      manager.on('end', function (evt) {
        updateSpeedDisplay(0); // Affiche 0 m/s lorsque le joystick est relâché
        updateJoystickOrientation(0, 0); // Revenir à "Neutre"
        updateSpeed([0, 0, 0, 0]); // Arrête la voiture
      });
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

    function initJoystickCam() {
      var options = {
        zone: document.getElementById('joystickCam'),
        mode: 'static',
        position: { left: '80px' },
        color: 'red',
      };
      var manager = nipplejs.create(options);

      manager.on('move', function (evt, data) {
        var verticalCamera = data.vector.y;
        var horizontalCamera = data.vector.x;

        var newCameraVertical = currentCameraVertical + Math.round(verticalCamera * 25);
        var newCameraHorizontal = currentCameraHorizontal + Math.round(horizontalCamera * 25);

        // Constrain the camera values between 40 and 140 degrees
        newCameraVertical = Math.max(cameraMinAngle, Math.min(cameraMaxAngle, newCameraVertical));
        newCameraHorizontal = Math.max(cameraMinAngle, Math.min(cameraMaxAngle, newCameraHorizontal));

        updateCamera(newCameraVertical, newCameraHorizontal);
      });

      manager.on('end', function (evt) {
        currentCameraVertical = 90;
        currentCameraHorizontal = 90;
        updateCamera(currentCameraVertical, currentCameraHorizontal); // Reposition to the center (90 degrees) when joystick is released
      });
    }

  </script>
</body>
</html>
)rawliteral";
