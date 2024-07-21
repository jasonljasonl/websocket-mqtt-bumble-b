#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>


// Intégration de la page en HTML
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Bumble-B Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">



<!--  *********  CSS (à mettre dans un fichier a part) *************  -->

  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
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
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.button:hover {background-color: #0f8b8d}*/
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
  </style>

<!-- ************ FIN DU CSS ****************** -->

</head>


<body>
  <div class="topnav">
    <h1>Bumble-B WebSocket Server</h1>
  </div>






<!-- ***************** AFFICHAGE DE LA CAMÉRA DE LA VOITURE ******************** -->


<div id="camera-stream">
    <!-- IP du streaming -->
    <img src="http://172.20.10.3:81/stream" width="1080px" height="600px">
</div>


<!-- *************** FIN DE L'AFFICHAGE DE LA CAMÉRA *********************** -->





<!-- **************** COMMANDES DE LA VOITURE ******************** -->


  <div class="content">

      <p><button id="button_haut" class="button">Haut</button></p>
      <p><button id="button_bas" class="button">Bas</button></p>
      <p><button id="button_droite" class="button">Droite</button></p>
      <p><button id="button_gauche" class="button">Gauche</button></p>
      <p><button id="button_stop" class="button">STOP</button></p>

    </div>
  </div>

<!-- **************** FIN DES COMMANDES DE LA VOITURE ******************** -->


  


<!-- **************** SCRIPT JAVASCRIPT DE LA VOITURE (COMMUNICATION ENTRE LA VOITURE ET L'ESP32) *************** -->

<script>

  <!-- Lien du websocket ws://AdresseDeLaVoiture/ws -->
  var gateway = `ws://${window.location.hostname}/ws`;


  <!-- Initialisation de la page Websocket -->
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
  }


  <!-- Vérification de l'ouverture de la connexion dans la console -->
  function onOpen(event) {
    console.log('Connection opened');
  }

  <!-- Vérification de la fermeture de la connexion dans la console -->
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }



  <!-- Appel des fonctions -->
  function onLoad(event) {
    initWebSocket();
    initButton();
    initButtonHaut();
    initButtonBas();
    initButtonDroite();
    initButtonGauche();
    initButtonStop();
  }



  <!-- Récupération des boutons par l'ID et appel des fonctions aux clics  -->
  function initButton() {
    document.getElementById('button_haut').addEventListener('click', btnHaut);
    document.getElementById('button_bas').addEventListener('click', btnBas);
    document.getElementById('button_droite').addEventListener('click', btnDroite);
    document.getElementById('button_gauche').addEventListener('click', btnGauche);
    document.getElementById('button_stop').addEventListener('click', btnStop);


  }



  <!-- Déclarations des fonctions. Chaque fonction envoi du texte au format JSON à l'ESP32 qui sert à intéragir avec la voiture --> 
  <!-- "cmd : 1" correspond aux moteurs, "data", à la vitesse ([moteur1, moteur2, moteur3, moteur4]) -->
  function btnHaut(){
    websocket.send('{"cmd": 1,"data": [1000, 1000, 1000, 1000]}');
  }

  function btnBas(){
    websocket.send('{"cmd": 1,"data": [-1000, -1000, -1000, -1000]}');
  }

  function btnDroite(){
    websocket.send('{"cmd": 1,"data": [2000, 2000, -1000, -1000]}');
  }

  function btnGauche(){
    websocket.send('{"cmd": 1,"data": [-1000, -1000, 2000, 2000]}');
  }

  function btnStop(){
    websocket.send('{"cmd": 1,"data": [000, 000, 000, 000]}');
  }

</script>

<!-- ******************** FIN DU SCRIPT DE LA VOITURE ***************** ->


</body>
</html>
)rawliteral";


