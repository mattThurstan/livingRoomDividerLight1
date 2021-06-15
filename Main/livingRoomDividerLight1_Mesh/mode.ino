/*----------------------------mode----------------------------*/

/*
 * 0 - glow     (static/moving)
 * 1 - sunrise  (static/moving)
 * 2 - morning  (static/moving)
 * 3 - day      (static/moving) (full bright/overcast/clearbluesky)
 * 4 - working  (static) (full bright) (with colour temperature sub-mode)
 * 5 - evening  (static/moving)
 * 6 - sunset   (static/moving)
 * 7 - night    (static/moving)
 * 8 - effect   (moving) (could use static to pause it?)
 * 
 * gradient fade (top/mid/bot) (all can invert) (overlay/fx?)
 * moveable/resizable bright bar (overlay/fx?) (might use brightness +/- intputs to move up/down)
 */

/*
  select mode to run based upon mode var, set by input
  called from main loop
  (select from preset array) ..later
  numbers denote slots
  slots are filled (by hand for now) with named functions
  ..needs an override aswell
  modes are represented as numbers, but have names if needed
  modes are selected individualy or in a present sequence
 */
void loopModes() {
  if (_onOff) {
    if(_modeCur == 0) {       mode0(); } 
    else if(_modeCur == 1) {  mode1(); }
    else if(_modeCur == 2) {  mode2(); }
    else if(_modeCur == 3) {  mode3(); }
    else if(_modeCur == 4) {  mode4(); }
    else if(_modeCur == 5) {  mode5(); }
    else if(_modeCur == 6) {  mode6(); }
    else if(_modeCur == 7) {  mode7(); }
    else if(_modeCur == 8) {  mode8(); }
  } 
  else {
    loopBreathing();
  }

}

/*----------------------------modes----------------------------*/
void mode0() {
  // name should be glow!
  // default is static (mabye with a few sparkles)
  for(int i = ledSegment[0].first; i <= ledSegment[3].last; i++) {
    strip.SetPixelColor(i, _hslGlowStart);
  }
}

//

void mode2() {
  // name should be morning!
  FillGradientRGB(ledSegment[0].first, ledSegment[3].last, _hslMorningStart, _hslMorningEnd);
}

void mode3() {
  // name should be day!
  FillGradientRGB(ledSegment[0].first, ledSegment[3].last, _hslDayStart, _hslDayEnd);
}

void mode4() {
  // name should be working!
  for (int i = ledSegment[0].first; i <= ledSegment[3].last; i++) {
    strip.SetPixelColor(i, _rgbColorTempCur);
  }
}

void mode5() {
  // name should be evening!
  for(int i = ledSegment[0].first; i <= ledSegment[3].last; i++) {
    strip.SetPixelColor(i, _hslEveningStart);
  }
}

//

void mode7() {
  // name should be night!
   for(int i = ledSegment[0].first; i <= ledSegment[3].last; i++) {
      strip.SetPixelColor(i, _hslNightEnd);
    }
}

void mode8() {
  // name should be effect!
  // gHueRotate will be running in this mode

  if (_effectCur == 0) {
    // TEMP
    // rotating gHue
    for (int i = ledSegment[0].first; i <= ledSegment[3].last; i++) {
      strip.SetPixelColor(i, _hslEffect0);
    }
  }
  else if (_effectCur == 1) {
    // TEMP
    for (int i = ledSegment[0].first; i <= ledSegment[3].last; i++) {
      strip.SetPixelColor(i, _hslEffect1);
    }
  }
  
}
