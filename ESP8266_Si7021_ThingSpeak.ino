#include <ESP8266WiFi.h>
#include <Adafruit_Si7021.h>
#include "ThingSpeak.h"
#include "secrets.h"
#include <PrintEx.h>

using namespace ios;
StreamEx s = Serial;

Adafruit_Si7021 sensor = Adafruit_Si7021();

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

const char* apiKey = SECRET_WRITE_APIKEY;
const char* resource = "/update?api_key=";
const char* server = "api.thingspeak.com";

WiFiClient client;

void failBlink(int j, int d) {
  for (int i = 0; i < j; i++)
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(d / 2);
  }
  digitalWrite(LED_BUILTIN, HIGH);
}

void passBlink(int j, int d) {
  for (int i = 0; i < j; i++)
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(d * 1.5);
  }
  digitalWrite(LED_BUILTIN, HIGH);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  if (!sensor.begin()) {
    s << "Sensor not found!" << endl;
    failBlink(4, 1000);
  } else {
    passBlink(4, 1000);
  }
  initWifi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    s << "Attempting to reconnect to SSID: " << SECRET_SSID << endl;
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(SECRET_SSID, SECRET_PASS);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      s << '.';
      delay(1000);
    }
    s << endl << "Connected." << endl;
  }

  ThingSpeak.setField(1, (float)(sensor.readTemperature() * 1.8 + 32.0));
  ThingSpeak.setField(2, sensor.readHumidity());
  ThingSpeak.setStatus("connected");

  int x = ThingSpeak.writeFields(SECRET_CH_ID, SECRET_WRITE_APIKEY);
  if (x == 200) {
    s << "Channel update successful." << endl;
    passBlink(10, 500);
  } else {
    s << "Problem updating channel. HTTP error code " << String(x) << endl;
    failBlink(10, 500);
  }
}

// Establish a Wi-Fi connection with your router
void initWifi() {
  s << "Connecting to: " << ssid << endl;
  WiFi.begin(SECRET_SSID, SECRET_PASS);

  int timeout = 10 * 6; // 10 seconds
  while (WiFi.status() != WL_CONNECTED  && (timeout-- > 0)) {
    delay(250);
    s << '.';
  }
  s << endl;

  if (WiFi.status() != WL_CONNECTED) {
    s << "Failed to connect, going back to sleep" << endl;
    failBlink(8, 1500);
  } else {
    s << "WiFi connected in: " << millis() << ", IP address: ";
    Serial.println(WiFi.localIP());
    passBlink(8, 1500);
  }
}

// Make an HTTP request to Thing Speak
/*
  void makeHTTPRequest()
  {
  float h = sensor.readHumidity(),
        t = sensor.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if  (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from sensor!"));
    strcpy(temperatureTemp, "Failed");
    strcpy(humidityTemp, "Failed");
    return;
  } else {
    s << "Humidity: " << h << "%\tTemperature: " << t << "*C" << endl;
  }
  s << "Connecting to " << server << endl;

  //  WiFiClient client;
  //  int retries = 5;/
  //  while (!client.connect(server, 80) && (retries-- > 0))
  //    s << '.';
  //  s << endl;
  //  if (!client.connected())
  //    s << "Failed to connect." << endl;
  //  s << "Request resource: " << resource << endl;
  //  client.print(String("GET ") + resource + apiKey +
  //               "&field1=" + humidityTemp +
  //               "&field2=" + temperatureTemp +
  //               " HTTP/1.1\r\n" + "Host: " + server + "\r\n" +
  //               "Connection: close\r\n\r\n");

  //  int timeout = 5 * 10; // 5 seconds
  //  while (!client.available() && (timeout-- > 0))
  //    delay(100);
  //  if (!client.available())
  //    s << ("No response.") << endl;
  //  while (client.available())
  //    Serial.write(client.read());
  //  s << endl <<  "closing connection"  << endl;
  //  client.stop();
  } */

