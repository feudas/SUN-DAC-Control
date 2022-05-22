#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_MCP4725.h>
#include <Wire.h>
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>

Adafruit_MCP4725 dac;

const char* wifi_ssid = "xxx";
const char* wifi_password = "xxx";
const char* mqtt_server = "192.168.178.x";
const char* mqtt_user = "xxx";      
const char* mqtt_password = "xxx";
  
const char* topic = "SunDAC/set";
#define ESP8266Client "SunDAC"
#define wifihostname "SunDAC"

WiFiClient espClient;
PubSubClient client(espClient);

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
  WiFi.mode(WIFI_STA);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  dac.begin(0x60);
  delay(1000);
  dac.setVoltage(0, false);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void setup_wifi() {
  WiFi.setHostname(wifihostname);
  delay(10);
  Serial.print("Connecting to ");
  Serial.print(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);
  int x = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...Topic:");

    if (client.connect(ESP8266Client, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

float powersum = 0;
unsigned int devices = 1;
unsigned int rawvalue = 0;

void callback(char* topic, byte * payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("], ");
  Serial.print("Length is ");
  Serial.println(length);
  char *callbackString = (char *) payload;
  callbackString[length] = '\0';
  payload[length] = '\0';
  String strPayload = String((char*)payload);
  Serial.print("Callback payload: ");
  Serial.println(strPayload);
  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, payload, length);

  if (err){
    Serial.print(F("deserializeJson() failed with code: "));
    Serial.println(err.c_str());
    Serial.println("Set DAC to zero voltage.");
    dac.setVoltage(0, false);
  } else {
    powersum = (float) doc["powersum"];
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
  }
}
