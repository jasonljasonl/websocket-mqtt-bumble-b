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
  
  function updateSpeed(speedArray) {
    sendCommand(1, speedArray);
  }
  