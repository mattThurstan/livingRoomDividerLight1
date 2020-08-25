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
                    _colorHSL.H = jsonDoc["colorHSL_H"];
                    _colorHSL.S = jsonDoc["colorHSL_S"];
                    _colorHSL.L = jsonDoc["colorHSL_L"];
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
  json["colorHSL_H"] = _colorHSL.H;
  json["colorHSL_S"] = _colorHSL.S;
  json["colorHSL_L"] = _colorHSL.L;

  File settingsFile = SPIFFS.open("/settings.json", "w");
  if (!settingsFile && DEBUG_GEN) { Serial.println("failed to open user settings file for writing"); }
  if (DEBUG_GEN) { 
    serializeJson(jsonDoc, Serial);
    serializeJson(jsonDoc, settingsFile);
  }
  settingsFile.close();
}
