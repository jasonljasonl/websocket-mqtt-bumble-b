let currentCameraVertical = 90;
let currentCameraHorizontal = 90;
const cameraMaxAngle = 140;
const cameraMinAngle = 40;

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

function updateCamera(verticalAngle, horizontalAngle) {
  sendCommand(3, [verticalAngle, horizontalAngle]);
}
