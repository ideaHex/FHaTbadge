/*
MIT License

Copyright (c) 2019, Tilden Groves

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
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
#include "LEDMatrix.h"
#include "Config.h"
#include <Ticker.h>

// name and password for home network
const char* ssidForAP = "home_network_name";            // home network name
const char* passwordForAP = "home_network_password";    // home network password

// ESP8266 name
const char* ESP8266Name = "esp8266";
// use http://esp8266.local to connect to webpage in your browser (may not work in EDGE or android devices)

// name and password for connecting to ESP8266 as an Accesspoint
const char *password = "12345678";  // This is the Wifi Password (only numbers and letters,  not . , |)
const char *AP_Name = "FHaTbadge";  // This is the Wifi Name(SSID), some numbers will be added for clarity (mac address)

ESP8266WebServer server(80);
DNSServer dnsServer;
HandleTheOTA otaHandler(&dnsServer,AP_Name,password);
LEDMatrix matrix;
Ticker testTicker;
int testVeriable=0;
Ticker animationTimer;

void setup(){
    pinMode(SW1_Pin,INPUT_PULLUP);
    pinMode(SW2_Pin,INPUT_PULLUP);
    pinMode(SW3_Pin,INPUT_PULLUP);
    setupMatrix();
    matrix.setMatrix(uint64_t(0x181818422499423c),0); // show wifi symbol
    Serial.begin(115200);
    SPIFFS.begin();                 //start SPIFFS
    setupWiFi();                    //setup wifi
    Serial.println(F("\r\nSetup Complete"));
    //testTicker.attach_ms(800,test);//TODO: Delete this, its for testing only
    //test();
    scrollTest("HackerSpace Adelaide",70);
    rst_info *resetInfo;
    resetInfo = ESP.getResetInfoPtr();
    Serial.print((*resetInfo).reason);

 //REASON_DEFAULT_RST = 0, /* normal startup by power on */
 //REASON_WDT_RST = 1, /* hardware watch dog reset */
 //REASON_EXCEPTION_RST = 2, /* exception reset, GPIO status won't change */
 //REASON_SOFT_WDT_RST   = 3, /* software watch dog reset, GPIO status won't change */
 //REASON_SOFT_RESTART = 4, /* software restart ,system_restart , GPIO status won't change */
 //REASON_DEEP_SLEEP_AWAKE = 5, /* wake up from deep-sleep */
 //REASON_EXT_SYS_RST      = 6 /* external system reset */
    Serial.println(": Reset Reason: " + ESP.getResetReason());
    Serial.println(ESP.getResetInfo());
    Serial.println("VCC: " + ESP.getVcc());
 
}

void loop(){
    delay(1);                         // power saving in station mode drops power usage.
    ArduinoOTA.handle();              // handle OTA update requests.
    ArduinoOTA.onStart(disableTimer); // disable screen updates if there is an update in progress.
    dnsServer.processNextRequest();   // maintain DNS server.
    server.handleClient();            // handle client requests.
}
void test(){
  matrix.setMatrix(matrixFont[testVeriable],0);
  testVeriable++;
  if (testVeriable>fontLength-1)testVeriable=0;
}
void scrollTest(String text,unsigned long frameDelay){
  matrix.text = text;
  animationTimer.detach();
  animationTimer.attach_ms(frameDelay,animationCallback);
}
void animationCallback(){
  matrix.scroll();
}
void setupWiFi(){
    WiFi.mode(WIFI_STA);
    if (SPIFFS.exists("/APData.csv")){        // check to see if theres any data from the last AP change
        otaHandler.loadDatafile(ESP8266Name);
    }else{                                    //No Saved data File, load defaults
        WiFi.begin(ssidForAP, passwordForAP); // connect to home Accesspoint(Wifi)
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
  //disableTimer();
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
  //enableTimer();
}

void handleNotFound(){
  //disableTimer();
	sendFile(server.uri(),&server);
  //enableTimer();
}
// timer functions
void setupMatrix(){
    matrix.ticks = clockCyclesPerMs / frequency;
    disableTimer();
		enableTimer();
}
void disableTimer(){
    timer1_disable();
		timer1_detachInterrupt();
    matrix.clearMatrix();
}

void enableTimer(){
    timer1_isr_init();
		timer1_attachInterrupt(T1IntHandler);
		timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
		timer1_write(matrix.ticks); // ticks before interrupt fires, maximum ticks 8388607
}

ICACHE_RAM_ATTR void T1IntHandler(){
      matrix.T1IntHandler();
    }