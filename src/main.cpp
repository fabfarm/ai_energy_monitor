#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <queue>
#include <SPIFFS.h>

const int sensorIn = 34;
const int bufferedReadings = 10000;
std::queue<short int> readings;
std::queue<unsigned long> timestamps;

const char* ssid = "caravan";
const char* password = "imakestuff";
WebServer server(80);

short int getReading() {
  return analogRead(sensorIn);
}

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;

  time(&now);
  return now;
}

void handle_root() {
  // TODO: return more structured formal (e.g. csv) instead
  String content = "";
  content += "<!DOCTYPE html><html><body>";
  content += "<h4>readings.front()</h4>";
  content += readings.front();
  content += "<h4>timestamps.front()</h4>";
  content += timestamps.front();
  content += "<h4>readings.size()</h4>";
  content += readings.size();
  content += "<h4>timestamps.size()</h4>";
  content += timestamps.size();
  content += "</body></html>";
  server.send(200, "text/html", content);
}

void handle_data() {
  File fileWrite = SPIFFS.open("/data.csv", FILE_WRITE);
  fileWrite.println("timestamp,reading");
  while (!readings.empty()) {
    String line = "";
    line += timestamps.front();
    line += ",";
    line += readings.front();

    readings.pop();
    timestamps.pop();

    fileWrite.println(line);
  }

  fileWrite.close();
  File fileRead = SPIFFS.open("/data.csv", FILE_READ);
  server.send(200, "text/csv", fileRead.readString());
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
  timestamps.push(getTime());
  readings.push(getReading());
  if (readings.size() > bufferedReadings) {
    readings.pop();
    timestamps.pop();
  }

  server.handleClient();
}
