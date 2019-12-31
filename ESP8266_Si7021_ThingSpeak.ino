#include "ThingSpeak.h"
#include "secrets.h"
#include <WiFi.h>

#ifdef BOX1
#include <Wire.h>
#include "Adafruit_HTU21DF.h"
Adafruit_HTU21DF sensor = Adafruit_HTU21DF();
#else
#include "Adafruit_Si7021.h"
Adafruit_Si7021 sensor = Adafruit_Si7021();
#endif


const char *ssid = SECRET_SSID;
const char *password = SECRET_PASS;

const char *apiKey = SECRET_WRITE_APIKEY;
const char *resource = "/update?api_key=";
const char *server = "api.thingspeak.com";

WiFiClient client;

void setup() {
  WiFi.mode(WIFI_STA);
  initWifi();
  ThingSpeak.begin(client); // Initialize ThingSpeak
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED) {
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(SECRET_SSID, SECRET_PASS); // Connect to WPA/WPA2 network. Change this line
      delay(1000);
    }
  }

  if (!sensor.begin()) {
    ThingSpeak.setStatus("Sensor Error");
    ThingSpeak.writeFields(SECRET_CH_ID, SECRET_WRITE_APIKEY);
    delay(5000);
    return;
  }

  float h = sensor.readHumidity();

  if (isnan(h)) {
    ThingSpeak.setStatus("Sensor Error");
    ThingSpeak.writeFields(SECRET_CH_ID, SECRET_WRITE_APIKEY);
    delay(5000);
    return;
  } else {
#ifdef BOX1
    ThingSpeak.setField(1, h);
#else
    ThingSpeak.setField(2, h);
#endif
  }

  ThingSpeak.setStatus("OK");

  int x = ThingSpeak.writeFields(SECRET_CH_ID, SECRET_WRITE_APIKEY);
  delay(15000);
}

void initWifi() {
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  int timeout = 10 * 6; // 10 seconds
  while (WiFi.status() != WL_CONNECTED && (timeout-- > 0)) {
    delay(250);
  }
}


