/*----------------------------utils---------------------------*/

void turnOffWifi() {
  if (DEBUG_GEN) { Serial.println("Disconnecting..."); }
  WiFi.disconnect();
}

void turnOffMesh() {
  if (DEBUG_GEN) { Serial.println("Disconnecting mesh..."); }
  mesh.stop();
}

void turnOffSerial() {
  if (DEBUG_GEN) { Serial.println("Disconnecting serial..."); }
  //
}

void factoryReset() { /* TODO */ }

void deviceRestart() { ESP.restart(); }

void deviceShutdown() { /* ??? */ }


/*----------------------------main calls-----------------------*/
/* Reset everything to default. */
void doReset() {
  resetDefaults();
  deviceRestart();
}

/* Restart the device (with a delay) */
void doRestart(uint8_t restartTime) {
  uint16_t dly = (restartTime * 60 * 1000); // static ???
  delay(dly);
  deviceRestart(); // ..and restart
}

/*
 * Lockdown.
 * 
 * Emergency global disconnect (requires hard reset). 
 *  0 = do thing (no severity)
 *  1 = disconnect from LAN (bridges shutdown and mesh reboots) 
 *  2 = shutdown mesh (everything reboots in standalone mode) 
 *  3 = shutdown everything. 
 *      - Devices to power off if possible, if not then reboot in standalone emergency mode. 
 *      - These devices will require a hardware reset button implemented to clear the emergency mode.)
 */
void doLockdown(uint8_t severity) {
  
}

/*
 * Debug utils
 */
void loopDebug() {
  if (DEBUG_OVERLAY) {
    showSegmentEndpoints();
  } else {
    //strip.SetPixelColor(0, _rgbBlack);            // modes are responsible for all other leds
  }
  
  if (DEBUG_MESHSYNC) { }
}

/*
 * Color temperature utils
 */
void setColorTemp(int i) {
  _colorTempCur = i;
  colorTempCatch();
  setColorTempRgbCur(i);
}
void setColorTempRgbCur(int i) {
  if (i == 0) {
    _rgbColorTempCur = _rgbWarmFluorescent;
  } else if (i == 1) {
    _rgbColorTempCur = _rgbStandardFluorescent;
  } else if (i == 2) {
    _rgbColorTempCur = _rgbCoolWhiteFluorescent;
  }
}
void cycleColorTemp() {
  _colorTempCur += 1;
  colorTempCatch();
  setColorTempRgbCur(_colorTempCur);
}
void colorTempCatch() {
  if ( (_colorTempCur >= _colorTempNum) || (_colorTempCur < 0) ) { 
    // rollover
    _colorTempCur = 0; 
  }  
}


/*
 * Alternative map function that uses float (instead of int)
 */
float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
