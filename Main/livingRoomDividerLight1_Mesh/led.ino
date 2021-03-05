/*----------------------------setup led-----------------------*/
void setupLEDs() {
  
  strip.Begin();                                  // NeoPixelBus
  strip.Show();                                   // NeoPixelBus
  
  strip.SetBrightness(_ledGlobalBrightnessCur);
  _gHue2saved = _gHue2;
  checkAndSetGHue2CycleMillis();
}


/*----------------------------loop led------------------------*/
//void loopLEDs() { }

/*
 Fade all leds except for the sacrificial one in the box
 LED '0' is used for debug porpoises
 */
void FadeAll(uint8_t darkenBy) {
    RgbColor color;
    for (uint16_t i = 0; i < strip.PixelCount(); i++)
    {
        color = strip.GetPixelColor(i);
        if (color.R > 0 && color.G > 0 && color.B > 0) {
          color.Darken(darkenBy);
        } else {
          color = _rgbBlack;
        }
        strip.SetPixelColor(i, color);
    }
}

/*
 A gradient of colour from one end of a strip to the other
 */
void FillGradientRGB(byte first, byte total, RgbColor colA, RgbColor colB) {
  for (uint16_t indexPixel = 0; indexPixel < total; indexPixel++)
    {
      //map number of used pixels to 0-100
      //mapf();
      int ti = map(indexPixel, 0, (total+1), 0, 100);
      
      //divide by 100 for 0.0-1.0
      float tf = ti/100.0;
      
      RgbColor updatedColor = RgbColor::LinearBlend(colA, colB, tf);
      
      strip.SetPixelColor( (indexPixel + first), updatedColor);
      
      if (DEBUG_GEN) { 
        Serial.print("SetGradient - i "); 
        Serial.print(indexPixel); 
        Serial.print(", ti - "); 
        Serial.print(ti); 
        Serial.print(", tf - "); 
        Serial.print(tf); 
        Serial.print(", col - "); 
        Serial.print(updatedColor.R); 
        Serial.print(", "); 
        Serial.print(updatedColor.G); 
        Serial.print(", "); 
        Serial.print(updatedColor.B); 
        Serial.println(); 
      }
    }
}

/*
 * Increment gHue by 0.1 every N milliseconds
 * Saved as a uint8_t (using 0-100) and converted to a float (0.0-1.0)
 */
void gHueRotate() {
  if (_modeCur == 8) {
    unsigned long _gHue2CurMillis = millis();
    if ( (unsigned long)(_gHue2CurMillis - _gHue2PrevMillis) >= _gHue2CycleMillis) {
      _gHue2PrevMillis = millis(); //re-initilize Timer
      _gHue2++;                                   // slowly cycle the "base color" through the rainbow
      checkAndSetHslEffect0Hue();
    }
  }
}

/*
 * Checks and resets _gHue2 bounds 0-100
 * Converts and sets effect colour Hue
 */
void checkAndSetHslEffect0Hue() {
  if (_gHue2 > 100) { _gHue2 = 0; }           // rollover catch
  float tg = (float) _gHue2;
  tg = tg/100;
  _hslEffect0.H = tg;
}

void setHslEffect0(RgbColor rgb) {
  _hslEffect0 = rgb;
  
  if (DEBUG_GEN) { 
    Serial.print("setHSLEffect0 - R ");
    Serial.print(rgb.R);
    Serial.print(", G ");
    Serial.print(rgb.G);
    Serial.print(", B ");
    Serial.println(rgb.B);
  }
}

void setHslEffect1(RgbColor rgb) {
  _hslEffect1 = rgb;
  
  if (DEBUG_GEN) { 
    Serial.print("setHSLEffect1 - R ");
    Serial.print(rgb.R);
    Serial.print(", G ");
    Serial.print(rgb.G);
    Serial.print(", B ");
    Serial.println(rgb.B);
  }
}

/*
 * Checks and resets LED rise speed bounds 0-255 (for the moment, change to something like 5-50)
 * Coverts and sets LED rise speed
 */
 void checkAndSetLedRiseSpeed() {
  if (_ledRiseSpeedSaved > 255) { _ledRiseSpeedSaved = 0; }
  if (_ledRiseSpeedSaved < 0) { _ledRiseSpeedSaved = 255; }
  _ledRiseSpeed = (unsigned long) _ledRiseSpeedSaved;
 }

/*
 * Checks and resets gHue2 saved cycle time 0-255
 * Converts and sets gHue2 cycle millis
 */
void checkAndSetGHue2CycleMillis() {
  if (_gHue2CycleSaved > 255) { _gHue2CycleSaved = 0; }
  if (_gHue2CycleSaved < 0) { _gHue2CycleSaved = 255; }
  _gHue2CycleMillis = (unsigned long) (_gHue2CycleSaved * _gHue2CycleMultiplier);
}

/*
 Check Segment Endpoints
 usage | showSegmentEndpoints();
 overlay/place near end of chain
 This places different coloured lights at the ends of the LED strip(s) segments for quick visual feedback of calculations
 */
void showSegmentEndpoints() {
  strip.ClearTo(_rgbBlack);

  strip.SetPixelColor(ledSegment[0].first, _rgbRed);
  strip.SetPixelColor(ledSegment[0].last, _rgbRed);
  
  strip.SetPixelColor(ledSegment[1].first, _rgbRed);
  strip.SetPixelColor(ledSegment[1].last, _rgbRed);
  
  strip.SetPixelColor(ledSegment[2].first, _rgbRed);
  strip.SetPixelColor(ledSegment[2].last, _rgbRed);
  
  strip.SetPixelColor(ledSegment[3].first, _rgbRed);
  strip.SetPixelColor(ledSegment[3].last, _rgbRed);
}

/*
 * Global brightness utils
 */
void setGlobalBrightness(uint8_t gb) {
  //use this to achieve an override from the mesh, eg. to match levels
  _ledGlobalBrightnessCur = gb;
  brightnessRolloverCatch();
}
void increaseBrightness() {
  _ledGlobalBrightnessCur += _ledBrightnessIncDecAmount;
  brightnessRolloverCatch();
}
void decreaseBrightness() {
  _ledGlobalBrightnessCur -= _ledBrightnessIncDecAmount;
  brightnessRolloverCatch();
}
void brightnessRolloverCatch() {
  if(_ledGlobalBrightnessCur > 255) {
    _ledGlobalBrightnessCur = 255;
  } else if(_ledGlobalBrightnessCur < 0) {
    _ledGlobalBrightnessCur = 0;
  }
  strip.SetBrightness(_ledGlobalBrightnessCur);
}
