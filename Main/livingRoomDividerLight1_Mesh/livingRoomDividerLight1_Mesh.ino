/*
    'livingRoomDividerLight1_Mesh' by Thurstan. LEDs controlled by motion sensors.
    Copyright (C) 2021 MTS Standish (Thurstan|mattKsp)
    
    https://github.com/mattThurstan/
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be usefull,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    https://github.com/mattThurstan/
    
    WeMos D1 (R2 &) mini ESP8266, 80 MHz, 115200 baud, 4M, (1M SPIFFS)
*/


#include <MT_LightControlDefines.h>
#include <FS.h>                                   // file system
#include <FastLED.h>                              // still using some bits
#include <NeoPixelBrightnessBus.h>                // NeoPixelBrightnessBus (just for ESP8266)- for brightness functions (instead of NeoPixelBus.h)
#include "painlessMesh.h"


/*----------------------------system--------------------------*/
const String _progName = "livingRoomDividerLight1_Mesh";
const String _progVers = "0.209";                 // mode and global brightness

boolean DEBUG_GEN = false;                        // realtime serial debugging output - general
boolean DEBUG_OVERLAY = false;                    // show debug overlay on leds (eg. show segment endpoints, center, etc.)
boolean DEBUG_MESHSYNC = false;                   // show painless mesh sync by flashing some leds (no = count of active mesh nodes) 
boolean DEBUG_COMMS = false;                      // realtime serial debugging output - comms

boolean _firstTimeSetupDone = false;              // starts false //this is mainly to catch an interrupt trigger that happens during setup, but is usefull for other things
bool _onOff = false;
bool _shouldSaveSettings = false;                 // flag for saving data
bool _runonce = true;                             // flag for sending states when first mesh conection
//const int _mainLoopDelay = 0;                     // just in case  - using FastLED.delay instead..
bool _isBreathing = false;                        // toggle for breath
bool _isBreathOverlaid = false;                   // toggle for whether breath is overlaid on top of modes
bool _isBreathingSynced = false;                  // breath sync local or global

/*----------------------------pins----------------------------*/
// NeoPixelBus - For Esp8266, the Pin is omitted and it uses GPIO3 (RX) due to DMA hardware use. 

/*----------------------------modes---------------------------*/
const int _modeNum = 9;
volatile int _modeCur = 0;                        // current mode in use - this is not the var you are looking for.. try _modePresetSlotCur
String modeName[_modeNum] = { "Glow", "Sunrise", "Morning", "Day", "Working", "Evening", "Sunset", "Night", "Effect" };

const int _colorTempNum = 3;                      // 3 color temperature sub-modes for now
int _colorTempCur = 1;                            // current colour temperature
String colorTempName[_colorTempNum] = { "Warm", "Standard", "CoolWhite" }; // color temperature sub-mode names for the main "Working" mode.

const int _effectNum = 1;
volatile int _effectCur = 0;
String effectName[_effectNum] = { "RotateHue" };

/*----------------------------PIR-----------------------------*/

/*----------------------------LED-----------------------------*/
//56,65,64,57 cm
//16,19,19,16 px = 70
//const uint16_t _ledNum = 71;                      // NeoPixelBus - 70 + 1 LEDs
const uint16_t _ledNum = 70;                      // NeoPixelBus - 70 LEDs
NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> strip(_ledNum);

typedef struct {
  byte first;
  byte last;
  byte total;                                     // using a byte here is ok as we haven't got more than 256 LEDs in a segment
} LED_SEGMENT;
/*
const byte _segmentTotal = 5;                     // 1 + 4
LED_SEGMENT ledSegment[_segmentTotal] = {
  { 0, 0, 1 },  // sacrificial level changer
  { 1, 16, 16 },
  { 17, 35, 19 },
  { 36, 54, 19 },
   55, 70, 16 }
};
*/
const byte _segmentTotal = 4;                     // 
LED_SEGMENT ledSegment[_segmentTotal] = {
  { 0, 15, 16 },
  { 16, 34, 19 },
  { 35, 53, 19 },
  { 54, 69, 16 }
};
uint8_t _ledGlobalBrightnessCur = 127;            // current global brightness - adjust this
uint8_t _ledBrightnessIncDecAmount = 10;          // the brightness amount to increase or decrease
unsigned long _ledRiseSpeed = 25; //35;           // speed at which the LEDs turn on
uint8_t _ledRiseSpeedSaved = 25;                  // cos of saving / casting unsigned long issues - use 0-255 via mqtt
uint8_t _gHue2 = 0;                               // incremental cycling "base color", 0-100, converted to 0-1
uint8_t _gHue2saved = 0;                          // used to revert color when going back to 'Normal' mode
unsigned long _gHue2CycleMillis = 200UL;          // gHue loop update time (millis)
uint8_t _gHue2CycleSaved = 50;                    // 0-255 mapped to millis range - speed
uint8_t _gHue2CycleMultiplier = 4;                // (__gHue2CycleSaved * _gHue2CycleMultiplier) = (unsigned long) _gHue2CycleMillis
unsigned long _gHue2PrevMillis;                   // gHue loop previous time (millis)

RgbColor _rgbClearBlueSky(64, 156, 255);      // 20000 Kelvin - ClearBlueSky = 0x409CFF - 64, 156, 255

RgbColor _rgbRed(255, 0, 0);
RgbColor _rgbGreen(0, 255, 0);
RgbColor _rgbBlue(0, 0, 255);
RgbColor _rgbYellow(255, 255, 0);
RgbColor _rgbFuchsia(255, 0, 128);
RgbColor _rgbOrange(255, 165, 0);
RgbColor _rgbViolet(148, 0, 211);
RgbColor _rgbTeal(0, 128, 128);
RgbColor _rgbPink(255, 105, 180);
RgbColor _rgbWhite(255, 250, 255);
RgbColor _rgbGlow(4, 4, 4);
RgbColor _rgbBlack(0, 0, 0);
RgbColor _rgbEve(128, 64, 64);

// HSL colours for modes - "Glow", "Sunrise", "Morning", "Day", "Working", "Evening", "Sunset", "Night", "Effect"
HslColor _hslGlowStart(_rgbGlow);
HslColor _hslGlowEnd(_rgbBlack);
HslColor _hslSunriseStart(_rgbYellow);
HslColor _hslSunriseEnd(_rgbGreen);
HslColor _hslMorningStart(_rgbYellow);
HslColor _hslMorningEnd(_rgbBlack);
HslColor _hslDayStart(_rgbGreen);
HslColor _hslDayEnd(_rgbYellow);
HslColor _hslEveningStart(_rgbEve);
HslColor _hslEveningEnd(_rgbEve);
HslColor _hslSunsetStart(_rgbClearBlueSky);
HslColor _hslSunsetEnd(_rgbOrange);
HslColor _hslNightStart(_rgbBlack);
HslColor _hslNightEnd(_rgbGlow);
HslColor _hslEffect0(0.25f, 0.5f, 0.5f);
HslColor _hslEffect1(_rgbGlow);
//HslColor _hslEffect2(_rgbBlack);

// RGB colours for "Working" colour temperature sub-mode
RgbColor _rgbWarmFluorescent(255, 244, 229);  // WarmFluorescent = 0xFFF4E5 - 0 K, 255, 244, 229
RgbColor _rgbStandardFluorescent(244, 255, 250); // StandardFluorescent = 0xF4FFFA - 0 K, 244, 255, 250
RgbColor _rgbCoolWhiteFluorescent(212, 235, 255); // CoolWhiteFluorescent = 0xD4EBFF - 0 K, 212, 235, 255
RgbColor _rgbColorTempCur(_rgbStandardFluorescent); // use this one in day-to-day operations

//HslColor _colorHSL(0.25f, 0.5f, 0.5f);

/*----------------------------Mesh----------------------------*/
painlessMesh  mesh;                               // initialise
uint32_t id_bridge1 = DEVICE_ID_BRIDGE1;
uint8_t _stationChannel = STATION_CHANNEL;

void receivedCallback(uint32_t from, String &msg ) {
  if (DEBUG_COMMS) { Serial.printf("livingRoomDividerLight1_Mesh: Received from %u msg=%s\n", from, msg.c_str()); }
  receiveMessage(from, msg);
}

void newConnectionCallback(uint32_t nodeId) {
  if (_runonce == true) {
    publishStatusAll(false);
    _runonce = false;
  }
  if (DEBUG_COMMS) { Serial.printf("--> livingRoomDividerLight1_Mesh: New Connection, nodeId = %u\n", nodeId); }
}

void changedConnectionCallback() {
  //publish..
  if (DEBUG_COMMS) { Serial.printf("Changed connections %s\n",mesh.subConnectionJson().c_str()); }
}

void nodeTimeAdjustedCallback(int32_t offset) {
  if (DEBUG_COMMS) { Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset); }
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  if (DEBUG_COMMS) { Serial.printf("Delay to node %u is %d us\n", from, delay); }
}


/*----------------------------MAIN----------------------------*/
void setup() {
  
  // LED strip - Wemos D1 - GPIO 3 (RX) - swap the pin from serial to a GPIO.
  pinMode(3, FUNCTION_3); // FUNCTION_0 = swap back

  // start serial regardless but control debug output from mqtt
  Serial.begin(115200);
  
  Serial.println();
  Serial.print(_progName);
  Serial.print(" v");
  Serial.print(_progVers);
  Serial.println();
  Serial.print("..");
  Serial.println();
  
  delay(3000);                                    // give the power, LED strip, etc. a couple of secs to stabilise
  
  //loadSettings();
  
  setupLEDs();
  strip.ClearTo(_rgbBlack);
  strip.SetPixelColor(0, _rgbYellow);
  strip.Show();
  delay(400);
  
  setupMesh();
  strip.ClearTo(_rgbBlack);
  strip.SetPixelColor(0, _rgbBlue);
  strip.Show();
  delay(400);

  //everything done? ok then..
  Serial.print(F("Setup done"));
  Serial.println("-----");
  Serial.print(F("Device Node ID is "));
  String s = String(mesh.getNodeId());
  Serial.println(s);
  Serial.println("-----");
  Serial.println("");
  
  strip.ClearTo(_rgbBlack);
  strip.SetPixelColor(0, _rgbGreen);
  strip.Show();
  
  delay(1500);
  strip.ClearTo(_rgbBlack);
}

void loop()  {
  
  if(_firstTimeSetupDone == false) {
    _onOff = true;
    publishStatusAll(false);
    _firstTimeSetupDone = true;                   // need this for stuff like setting sunrise, cos it needs the time to have been set
    if (DEBUG_GEN) { Serial.print(F("firstTimeSetupDone  = true")); }
  }

  mesh.update();
  
  gHueRotate();
  loopModes();
  loopDebug();
  loopSaveSettings();
  //factoryReset();              //TODO           // Press and hold the button to reset to factory defaults

  strip.Show();
  //delay(_mainLoopDelay); 
}
