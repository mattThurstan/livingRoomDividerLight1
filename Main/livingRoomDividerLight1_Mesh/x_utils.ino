/*----------------------------utils---------------------------*/
void factoryReset() { }


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
