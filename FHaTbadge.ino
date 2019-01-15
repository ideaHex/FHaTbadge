/*
MIT License


Copyright (c) 2019, Tilden Groves


Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to dea
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:


The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.


THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma GCC optimize("-O2")

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <FS.h>
#include "WebFiles.h"
#include <ArduinoOTA.h>
#include "OTAHandler.h"

// name and password for home network
const char* ssidForAP = "home_network_name";            // home network name
const char* passwordForAP = "home_network_password";    // home network password

// ESP8266 name
const char* ESP8266Name = "esp8266";
// use http://esp8266.local to connect to webpage in your browser (may not work in EDGE)

// name and password for connecting to ESP8266 as an Accesspoint
const char *password = "12345678";   // This is the Wifi Password (only numbers and letters,  not . , |)
const char *AP_Name = "ESP8266WSOTA";// This is the Wifi Name(SSID), some numbers will be added for clarity (mac address)

ESP8266WebServer server(80);
DNSServer dnsServer;
HandleTheOTA otaHandler(&dnsServer,AP_Name,password);

void setup(){
    Serial.begin(115200);
    SPIFFS.begin();                 //start SPIFFS
    setupWiFi();                    //setup wifi
}

void loop(){
    delay(1);                       // power saving in station mode drops power usage.
    ArduinoOTA.handle();            // handle OTA update requests
    dnsServer.processNextRequest(); // maintain DNS server
    server.handleClient();          // handle client requests 
}

void setupWiFi(){
    WiFi.mode(WIFI_STA);
    if (SPIFFS.exists("/APData.csv")){// check to see if theres any data from the last AP change
        otaHandler.loadDatafile(ESP8266Name);
    }else{//No Saved data File, load defaults
        WiFi.begin(ssidForAP, passwordForAP);// connect to home Accesspoint(Wifi)
        WiFi.hostname(ESP8266Name);
    }
    otaHandler.connectToAP();
    ArduinoOTA.setHostname(ESP8266Name);
    ArduinoOTA.begin();

    // setup server callbacks                 
	server.on("/", handleRoot);
	server.onNotFound(handleNotFound);
	server.begin();
}

// server callbacks
void handleRoot(){
  if (server.hasArg("NAME") && server.arg("NAME") != "" && server.arg("PASSWORD") != "")
  {
    sendFile("/restarting.html", &server);
    otaHandler.saveAPData(server.arg("NAME"),server.arg("PASSWORD"));
    delay(1000);
    ESP.restart();
  }
  else
  {
    sendFile(server.uri(), &server);
  }
}

void handleNotFound(){
	sendFile(server.uri(),&server);
}