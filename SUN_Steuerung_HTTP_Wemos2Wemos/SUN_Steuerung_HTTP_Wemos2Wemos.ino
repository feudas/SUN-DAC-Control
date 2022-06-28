// der Code ist aus diversen Schnippseln zusammenkopiert 
// ich habe die quellen leider nicht mehr alle (Foren/Hilfeseiten/HowTo´s...)
// fmotto hat aber maßgeblich dazu beigetragen, den code zusammenzustellen
// falls jemand seinen code erkennt und genannt werden will, soll sich melden

// in dem code sind diverse sachen sicherlich durcheinander oder überflüssig
// mir gehts aber darum das es funktioniert und ich bin kein Programmierer ;)
// falls es verbesserungsvorschläge gibt, gerne her damit 
//---------------------------------------------------------------------------

#include <ESP8266WiFi.h>
#include <PubSubClient.h> 
#include <Adafruit_MCP4725.h>
#include <Wire.h>
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

Adafruit_MCP4725 dac;

const char* ssid = "deine_ssid";
const char* password = "dein_passwort";

HTTPClient sender;
WiFiClient espClient;

typedef struct {
uint16_t power;
uint16_t dacValue;
} dacDictionary;

static const dacDictionary dacTable[] = {
{ 64, 50 },{ 50, 64 },{ 60, 80 },{ 70, 94 },{ 80, 105 },{ 90, 112 },{ 100, 118 },{ 110, 126 },
{ 120, 138 },{ 130, 159 },{ 140, 179 },{ 150, 203 },{ 160, 224 },{ 170, 325 },{ 180, 362 },{ 190, 400 },
{ 200, 434 },{ 210, 470 },{ 220, 520 },{ 230, 575 },{ 240, 614 },{ 250, 636 },{ 260, 658 },{ 270, 676 },
{ 280, 695 },{ 290, 715 },{ 300, 736 },{ 310, 758 },{ 320, 776 },{ 330, 797 },{ 340, 819 },{ 350, 838 },
{ 360, 857 },{ 370, 876 },{ 380, 896 },{ 390, 915 },{ 400, 936 },{ 410, 954 },{ 420, 972 },{ 430, 989 },
{ 440, 1013 },{ 450, 1031 },{ 460, 1047 },{ 470, 1064 },{ 480, 1082 },{ 490, 1102 },{ 500, 1124 },{ 510, 1143 },
{ 520, 1162 },{ 530, 1182 },{ 540, 1202 },{ 550, 1221 },{ 560, 1240 },{ 570, 1261 },{ 580, 1280 },{ 590, 1298 },
{ 600, 1319 }, 
{ 610, 1328 }, { 620, 1341 }, { 630, 1366 }, { 640, 1371 }, { 650, 1392 }, { 660, 1400 }, { 670, 1419 },{ 680, 1437 }, 
{ 690, 1457 }, { 700, 1480 }, { 710, 1483 }, { 720, 1511 }, { 730, 1510 }, { 740, 1532 }, { 750, 1545 }, { 760, 1579 }, 
{ 770, 1594 }, { 780, 1618 }, { 790, 1640 }, { 800, 1650 }, { 810, 1672 }, { 820, 1680 }, { 830, 1700 }, { 840, 1728 }, 
{ 850, 1738 }, { 860, 1750 }, { 870, 1791 }, { 880, 1797 }, { 890, 1810 }, { 900, 1832 }, { 910, 1860 }, { 920, 1853 },
{ 930, 1873 }, { 940, 1883 }, { 950, 1939 }
};



void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("Verbunden!");
  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
  
  dac.begin(0x60);
  delay(1000);
  dac.setVoltage(0, false);
}

float powersum = 0;
unsigned int devices = 1;
unsigned int rawvalue = 0;


// der kommende teil ist von mir zusammengefrickelt (Hilfe aus dem Inet etc.)

void loop() {
  if (sender.begin(espClient, "http://IP_DES_WEMOS_AM_HAUSZÄHLER/cm?cmnd=Status%208")) {  //hier muss die ip des Wemos am Hauszähler eingetragen werden
          // HTTP-Code der Antwort speichern
          int httpCode = sender.GET();
          
          if (httpCode == HTTP_CODE_OK) {

              // Hier wurden die Daten vom Server empfangen
              // jetzt den json vom Webseiteninhalt speichern
              // sieht in meinem Fall so aus:
              // {"StatusSNS":{"Time":"2022-05-24T11:37:57","":{"Zählerstand":1744205.99488000,"Leistung gesamt":127.83,"Leistung L1":76.81,"Leistung L2":323.52,"Leistung L3":-272.50,"Spannung L1":237.4,"Spannung L2":232.1,"Spannung L3":233.6,"0_0_0":"1EBZ0101234567"}}}
           
           String payload = sender.getString();

        //empfangenen json auseinandernehmen

        DynamicJsonDocument doc(400);
       
        DeserializationError error = deserializeJson(doc, payload);

          if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            Serial.println("Set DAC to zero voltage.");
            dac.setVoltage(0, false);
            return;
          }

          const char* StatusSNS_Time = doc["StatusSNS"]["Time"]; // "2022-04-10T21:26:48"

          JsonObject StatusSNS_ = doc["StatusSNS"][""];
          double StatusSNS_Z_hlerstand = StatusSNS_["Zählerstand"]; // 1390868.11862
          float StatusSNS_Leistung_gesamt = StatusSNS_["Leistung gesamt"]; // 330.04
          float StatusSNS_Leistung_L1 = StatusSNS_["Leistung L1"]; // 9.42
          float StatusSNS_Leistung_L2 = StatusSNS_["Leistung L2"]; // 191.49
          float StatusSNS_Leistung_L3 = StatusSNS_["Leistung L3"]; // 129.13
          float StatusSNS_Spannung_L1 = StatusSNS_["Spannung L1"]; // 231.1
          float StatusSNS_Spannung_L2 = StatusSNS_["Spannung L2"]; // 229.7
          float StatusSNS_Spannung_L3 = StatusSNS_["Spannung L3"]; // 228.9
          const char* StatusSNS_0_0_0 = StatusSNS_["0_0_0"]; // "1EBZ0101234567"

          
       // Leistung ausgelesen jetzt anzeigen und dann verarbeiten        
          Serial.println(StatusSNS_Leistung_gesamt);



// ab hier der code von fmotto, glaube da kann auch noch einiges weg aber ich habs drin gelassen

          powersum = (float) (StatusSNS_Leistung_gesamt);
          Serial.print("powersum: ");
          Serial.println(powersum);

    if (doc.containsKey("devices")){
      devices = (unsigned int) doc["devices"];
      if (devices == 0){
        devices = 1;
      }
      Serial.print("devices: ");
      Serial.println(devices);
    }

    if (doc.containsKey("rawvalue")){
      rawvalue = (unsigned int) doc["rawvalue"];
      Serial.print("rawvalue: ");
      Serial.println(rawvalue);
    }

    Serial.println("");
    Serial.print("power request: ");
    Serial.println(powersum);

    uint16_t maxPower = 0;
    int maxPowerDacTablePos = 0;
    int dacTablePos = 0;
    uint16_t dacValue = 0;

    for(uint16_t i = 0; i < sizeof(dacTable)/sizeof(dacDictionary); ++i) {
      float forpower = dacTable[i].power;

      if (forpower > maxPower) {
        maxPower = forpower;
        maxPowerDacTablePos = i;
      }

      if ((powersum / devices) - forpower < 0) {
        dacTablePos = i;
        dacValue = dacTable[i].dacValue;
        break;
      }
    }

    if (dacValue == 0 && (powersum / devices) > maxPower) {
      dacTablePos = maxPowerDacTablePos;
      dacValue = dacTable[dacTablePos].dacValue;
    }

    Serial.print("Position: ");
    Serial.println(dacTablePos);

    if (rawvalue > 0) {
      dacValue = rawvalue;
      Serial.print("RAW DAC Value: ");
      Serial.println(dacValue);
    } else {
      if (powersum == 0) {
        dacValue = 0;
      }
      Serial.print("DAC Value: ");
      Serial.println(dacValue);
    }

    Serial.println("------------------------------");

    dac.setVoltage(dacValue, false);


// hier der rest von dem was ich oben gebastelt habe
          
   } else {
        // Falls HTTP-Error
        Serial.printf("HTTP-Error: ", sender.errorToString(httpCode).c_str());   
  }
        // Wenn alles abgeschlossen ist, wird die Verbindung wieder beendet
        sender.end();
 }
        else {
        Serial.printf("HTTP-Verbindung konnte nicht hergestellt werden!");
        }
}
