#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <queue>
#include <SPIFFS.h>

const int sensorIn = 34;
const int bufferedReadings = 20000;
std::queue<unsigned short int> readings;
unsigned long period = 100;

const char* ssid = "caravan";
const char* password = "imakestuff";
WebServer server(80);

short int getReading() {
  return analogRead(sensorIn);
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
  unsigned long start = millis();
  unsigned long reading = 0;
  unsigned long readingCount = 0;

  while (millis() - start < period) {
    reading += getReading();
    readingCount += 1;
  }

  readings.push(reading / readingCount);
  if (readings.size() > bufferedReadings) {
    readings.pop();
  }

  server.handleClient();
}
