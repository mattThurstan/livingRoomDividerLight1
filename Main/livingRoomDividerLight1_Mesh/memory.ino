/*----------------------------memory---------------------------*/
void loadSettings()
{
    //read configuration from FS json
    if (DEBUG_GEN) { Serial.println("mounting FS..."); }

    if (SPIFFS.begin())
    {
        if (DEBUG_GEN) { Serial.println("mounted file system"); }

        if (SPIFFS.exists("/settings.json")) {
            //file exists, reading and loading
            if (DEBUG_GEN) { Serial.println("reading user settings file"); }
            File configFile = SPIFFS.open("/settings.json", "r");
            if (configFile)
            {
                if (DEBUG_GEN) { Serial.println("opened user settings file"); }
                const size_t size = configFile.size();
                // Allocate a buffer to store contents of the file.
                std::unique_ptr<char[]> buf(new char[size]);

                configFile.readBytes(buf.get(), size);
                DynamicJsonDocument jsonDoc(512);
                DeserializationError error = deserializeJson(jsonDoc, buf.get());
                if (DEBUG_GEN) { 
                  serializeJson(jsonDoc, Serial);
                }
                if (error)
                {
                    if (DEBUG_GEN) { Serial.println("failed to load json user settings"); }
                }
                else
                {
                    if (DEBUG_GEN) { Serial.println("\nparsed json"); }

                    _ledGlobalBrightnessCur = jsonDoc["gBrightnessCur"];
                    _ledRiseSpeedSaved = jsonDoc["ledRiseSpeedSaved"];
                    checkAndSetLedRiseSpeed();
                    _gHue2CycleSaved = jsonDoc["gHue2CycleSaved"];
                    checkAndSetGHue2CycleMillis();
                    _hslEffect0.H = jsonDoc["hslEffect0_H"];
                    _hslEffect0.S = jsonDoc["hslEffect0"];
                    _hslEffect0.L = jsonDoc["hslEffect0_L"];
                    _stationChannel = jsonDoc["stationChannel"];
                }
            }
        }
    }
    else
    {
        if (DEBUG_GEN) { Serial.println("failed to mount FS"); }
    }
   
}

void saveSettings()
{
  if (DEBUG_GEN) { Serial.println("saving user settings"); }
  DynamicJsonDocument jsonDoc(512);
  JsonObject json = jsonDoc.to<JsonObject>();
  
  json["gBrightnessCur"] = _ledGlobalBrightnessCur;
  json["ledRiseSpeedSaved"] = _ledRiseSpeedSaved;
  json["gHue2CycleSaved"] = _gHue2CycleSaved;
  json["hslEffect0_H"] = _hslEffect0.H;
  json["hslEffect0_S"] = _hslEffect0.S;
  json["hslEffect0_L"] = _hslEffect0.L;
  json["stationChannel"] = _stationChannel;

  File settingsFile = SPIFFS.open("/settings.json", "w");
  if (!settingsFile && DEBUG_GEN) { Serial.println("failed to open user settings file for writing"); }
  if (DEBUG_GEN) { 
    serializeJson(jsonDoc, Serial);
    serializeJson(jsonDoc, settingsFile);
  }
  settingsFile.close();
}

void clearSettings() { }

void loopSaveSettings() {
  /*  EVERY_N_SECONDS(60) {                           // too much ???
    if (_shouldSaveSettings == true)
    { 
      saveSettings(); 
      _shouldSaveSettings = false; 
    }
  } 
*/
}

void resetDefaults() {
  clearSettings();
  setDefaults();
  saveSettings();
}

void setDefaults() {
  DEBUG_GEN = false;
  DEBUG_COMMS = false;
  DEBUG_MESHSYNC = false;                   // show painless mesh sync by flashing some leds (no = count of active mesh nodes) 
  DEBUG_COMMS = false;                      // realtime serial debugging output - comms

  _ledGlobalBrightnessCur = 127;
  _ledRiseSpeed = 25;
  checkAndSetLedRiseSpeed();
  _gHue2CycleSaved = 50;
  checkAndSetGHue2CycleMillis();
  _hslEffect0 = HslColor(0.25f, 0.5f, 0.5f);
                    
  _stationChannel = STATION_CHANNEL;
}
