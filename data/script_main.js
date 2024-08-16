window.addEventListener('load', onLoad);

function onLoad() {
  initWebSocket();
  initJoystick();
  initJoystickCam();
  updateSpeedDisplay(0); // Initial display
  updateJoystickOrientation(0, 0); // Initial orientation display
}
