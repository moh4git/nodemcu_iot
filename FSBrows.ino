/*
  FSWebServer - Example WebServer with SPIFFS backend for esp8266
  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the ESP8266WebServer library for Arduino environment.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  upload the contents of the data folder with MkSPIFFS Tool ("ESP8266 Sketch Data Upload" in Tools menu in Arduino IDE)
  or you can upload the contents of a folder if you CD in that folder and run the following command:
  for file in `\ls -A1`; do curl -F "file=@$PWD/$file" esp8266fs.local/edit; done

  access the sample web page at http://esp8266fs.local
  edit the page by going to http://esp8266fs.local/edit
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include "DHT.h"

#define DHTTYPE DHT11
#define DBG_OUTPUT_PORT Serial

#ifndef APSSID
// Access point Config
#define APSSID "MySmartHome"
#define APPSK  "12345678"
// Station Config
#define STASSID "MyAccessPoint"
#define STAPSK  "12345678"
#endif

const char* apssid = APSSID;
const char* appassword = APPSK;

const char* ssid = STASSID;
const char* password = STAPSK;
const char* host = "esp8266fs";

ESP8266WebServer server(80);

// DHT Sensor
uint8_t DHTPin = D4;
int relayPinOne = 14;
int relayPinTwo = 12;

bool relayone_status = false;
bool relaytwo_status = false;
               
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);                

//holds the current upload
File fsUploadFile;

//format bytes
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

String getContentType(String filename) {
  if (server.hasArg("download")) {
    return "application/octet-stream";
  } else if (filename.endsWith(".htm")) {
    return "text/html";
  } else if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".xml")) {
    return "text/xml";
  } else if (filename.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filename.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filename.endsWith(".gz")) {
    return "application/x-gzip";
  }
  return "text/plain";
}

bool handleFileRead(String path) {
  DBG_OUTPUT_PORT.println("handleFileRead: " + path);
  if (path.endsWith("/")) {
    path += "index.htm";
  }
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz)) {
      path += ".gz";
    }
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleFileUpload() {
  if (server.uri() != "/edit") {
    return;
  }
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) {
      filename = "/" + filename;
    }
    DBG_OUTPUT_PORT.print("handleFileUpload Name: "); DBG_OUTPUT_PORT.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
    if (fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {
      fsUploadFile.close();
    }
    DBG_OUTPUT_PORT.print("handleFileUpload Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);
  }
}

void handleFileDelete() {
  if (server.args() == 0) {
    return server.send(500, "text/plain", "BAD ARGS");
  }
  String path = server.arg(0);
  DBG_OUTPUT_PORT.println("handleFileDelete: " + path);
  if (path == "/") {
    return server.send(500, "text/plain", "BAD PATH");
  }
  if (!SPIFFS.exists(path)) {
    return server.send(404, "text/plain", "FileNotFound");
  }
  SPIFFS.remove(path);
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileCreate() {
  if (server.args() == 0) {
    return server.send(500, "text/plain", "BAD ARGS");
  }
  String path = server.arg(0);
  DBG_OUTPUT_PORT.println("handleFileCreate: " + path);
  if (path == "/") {
    return server.send(500, "text/plain", "BAD PATH");
  }
  if (SPIFFS.exists(path)) {
    return server.send(500, "text/plain", "FILE EXISTS");
  }
  File file = SPIFFS.open(path, "w");
  if (file) {
    file.close();
  } else {
    return server.send(500, "text/plain", "CREATE FAILED");
  }
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileList() {
  if (!server.hasArg("dir")) {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = server.arg("dir");
  DBG_OUTPUT_PORT.println("handleFileList: " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (output != "[") {
      output += ',';
    }
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }

  output += "]";
  server.send(200, "text/json", output);
}

String readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println((int) t);
    return String((int) t);
  }
}

String readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(h);
    return String(h);
  }
}

void setup(void) {
  DBG_OUTPUT_PORT.begin(115200);
  pinMode(DHTPin, INPUT);
  pinMode(relayPinOne, OUTPUT);
  pinMode(relayPinTwo, OUTPUT);
  DBG_OUTPUT_PORT.print("\n");
  DBG_OUTPUT_PORT.setDebugOutput(true);
  SPIFFS.begin();
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    DBG_OUTPUT_PORT.printf("\n");
  }


  //WIFI INIT
  DBG_OUTPUT_PORT.printf("Connecting to %s\n", ssid);
  WiFi.mode(WIFI_AP_STA);
  if (String(WiFi.SSID()) != String(ssid)) {
    // Station mode connection
    WiFi.begin(ssid, password);
  }

  int i = 1;
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED && i <= 5 ) {
    delay(500);
    Serial.print(".");
    i++;
  }

  if (WiFi.status() != WL_CONNECTED && i == 5 ) {
    Serial.print("Station connection failed");
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // Print local IP address and start web server
  Serial.println("");

  //Access point connection
  Serial.print("Connected to ");
  Serial.println(apssid);
  Serial.print(" Access point");
  WiFi.softAP(apssid, appassword);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  MDNS.begin(host);
  DBG_OUTPUT_PORT.print("Open http://");
  DBG_OUTPUT_PORT.print(host);
  DBG_OUTPUT_PORT.println(".local/edit to see the file browser");


  //SERVER INIT
  //list directory
  server.on("/list", HTTP_GET, handleFileList);
  //load editor
  server.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) {
      server.send(404, "text/plain", "FileNotFound");
    }
  });
  //create file
  server.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  server.on("/edit", HTTP_POST, []() {
    server.send(200, "text/plain", "");
  }, handleFileUpload);

  //called when the url is not defined here
  //use it to load content from SPIFFS
  server.onNotFound([]() {
    if (!handleFileRead(server.uri())) {
      server.send(404, "text/plain", "FileNotFound");
    }
  });

  //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/get_dh11_data", HTTP_GET, []() {
    String json = "{";
    json += "\"temperature\":" + readDHTTemperature();
    json += ", \"humidity\":" + readDHTHumidity();
    json += "}";
    server.send(200, "text/json", json);
    json = String();
  });

  server.on("/check_relay_one_status", []() {
      String json = "{";
      if(relayone_status)
      {
        json += "\"relayone_status\":\"true\"";
      }
      else
      {
        json += "\"relayone_status\":\"false\"";
      }
      json += "}";
      server.send(200, "text/json", json);
      json = String(); 
  });

  server.on("/relay_one", []() {
      relayone_status = !relayone_status;
      digitalWrite(relayPinOne, relayone_status); //Set the pin to HIGH (3.3V)
      String json = "{";
      if(relayone_status)
      {
        json += "\"relayone_status\":\"true\"";
      }
      else
      {
        json += "\"relayone_status\":\"false\"";
      }
      json += "}";
      server.send(200, "text/json", json);
      json = String(); 
  });

  server.on("/check_relay_two_status", []() {
      String json = "{";
      if(relaytwo_status)
      {
        json += "\"relaytwo_status\":\"true\"";
      }
      else
      {
        json += "\"relaytwo_status\":\"false\"";
      }
      json += "}";
      server.send(200, "text/json", json);
      json = String(); 
  });

  server.on("/relay_two", []() {
      relaytwo_status = !relaytwo_status;
      digitalWrite(relayPinTwo, relaytwo_status); //Set the pin to HIGH (3.3V)
      String json = "{";
      if(relaytwo_status)
      {
        json += "\"relaytwo_status\":\"true\"";
      }
      else
      {
        json += "\"relaytwo_status\":\"false\"";
      }
      json += "}";
      server.send(200, "text/json", json);
      json = String(); 
  });
  
  dht.begin();
  server.begin();
  DBG_OUTPUT_PORT.println("HTTP server started");

}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
