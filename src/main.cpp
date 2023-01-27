#include <Arduino.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <queue>

const int sensorIn = 34;
const int bufferedReadings = 20000;
std::queue<float> readings;
unsigned long period = 50;

const char *ssid = "caravan";
const char *password = "imakestuff";
WebServer server(80);

float getReading() {
  float result;
  int readValue;
  int maxValue = 0;
  int minValue = 4096;

  // We first get peak-to-peak voltage over period
  uint32_t start_time = millis();
  while ((millis() - start_time) < period) {
    readValue = analogRead(sensorIn);
    if (readValue > maxValue) {
      maxValue = readValue;
    }
    if (readValue < minValue) {
      minValue = readValue;
    }
  }

  // We then convert it to power. For details refer to initial commit.
  result = ((maxValue - minValue) * 3.3) / 4096.0;
  result = (result / 2.0) * 0.707;
  result = ((result * 1000) / 185) - 0.3;
  result = (result * 240 / 1.2);
  return result;
}

unsigned long getTime() {
  time_t now;
  time(&now);
  return now;
}

void handle_root() {
  String content = "";
  content += "<!DOCTYPE html><html><body>";
  content += "<h4>readings.back()</h4>";
  content += readings.back();
  content += "<h4>readings.size()</h4>";
  content += readings.size();
  content += "</body></html>";
  server.send(200, "text/html", content);
}

void handle_data() {
  SPIFFS.remove("/data.csv");
  File fileWrite = SPIFFS.open("/data.csv", FILE_WRITE);
  fileWrite.println(period);
  fileWrite.println(getTime());

  int readLines = 0;
  while (!readings.empty()) {
    fileWrite.println(readings.front());
    readings.pop();
    readLines += 1;
  }

  fileWrite.close();
  File fileRead = SPIFFS.open("/data.csv", FILE_READ);
  server.send(200, "text/csv", fileRead.readString());
  fileRead.close();
}

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println(WiFi.localIP());
  server.on("/", handle_root);
  server.on("/data", handle_data);
  server.begin();

  SPIFFS.begin(true);
  configTime(0, 0, "pool.ntp.org");
  delay(1000);
}

void loop() {
  float reading = getReading();

  Serial.println(reading);
  readings.push(reading);
  if (readings.size() > bufferedReadings) {
    readings.pop();
  }

  server.handleClient();
}
