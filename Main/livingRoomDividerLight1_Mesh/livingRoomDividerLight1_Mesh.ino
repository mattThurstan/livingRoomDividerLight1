/*
    'livingRoomDividerLight1_Mesh' by Thurstan. LEDs controlled by motion sensors.
    Copyright (C) 2020 MTS Standish (Thurstan|mattKsp)
    
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


#include <FS.h>                                   // file system
#include <FastLED.h>                              // still using some bits
#include <NeoPixelBrightnessBus.h>                // NeoPixelBrightnessBus (just for ESP8266)- for brightness functions (instead of NeoPixelBus.h)
#include "painlessMesh.h"
#include <MT_LightControlDefines.h>


/*----------------------------system--------------------------*/
const String _progName = "livingRoomDividerLight1_Mesh";
const String _progVers = "0.1";                   // 

boolean DEBUG_GEN = false;                        // realtime serial debugging output - general
boolean DEBUG_OVERLAY = false;                    // show debug overlay on leds (eg. show segment endpoints, center, etc.)
boolean DEBUG_MESHSYNC = false;                   // show painless mesh sync by flashing some leds (no = count of active mesh nodes) 
boolean DEBUG_COMMS = false;                      // realtime serial debugging output - comms
boolean DEBUG_INTERRUPT = false;                  // realtime serial debugging output - interrupt pins
boolean DEBUG_USERINPUT = false;                  // realtime serial debugging output - user input

boolean _firstTimeSetupDone = false;              // starts false //this is mainly to catch an interrupt trigger that happens during setup, but is usefull for other things
bool _shouldSaveSettings = false;                 // flag for saving data
bool _runonce = true;                             // flag for sending states when first mesh conection
bool _isBreathing = false;                        // toggle for breath
bool _isBreathOverlaid = false;                   // toggle for whether breath is overlaid on top of modes
bool _isBreathingSynced = false;                  // breath sync local or global

/*----------------------------pins----------------------------*/
/*----------------------------modes---------------------------*/
/*----------------------------PIR-----------------------------*/
/*----------------------------LED-----------------------------*/
/*----------------------------Mesh----------------------------*/
/*----------------------------MAIN----------------------------*/
/*----------------------------interrupt callbacks-------------*/
