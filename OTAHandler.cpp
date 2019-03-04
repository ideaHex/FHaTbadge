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

#include "OTAHandler.h"

     HandleTheOTA::HandleTheOTA(DNSServer * dnsServer, const char* AP_Name, const char* password){
         dnsServerH = dnsServer;
         AP_NameH = AP_Name;
         passwordH = password;
    }
    void HandleTheOTA::saveAPData(String name2,String password2){
	    String fileName = "/APData.csv";
	    File dataFile = SPIFFS.open(fileName, "w");
	    if (!dataFile){
		    //Serial.println(F("Failed to create file"));
	    }
	    dataFile.println(name2 + "," + password2);
	    dataFile.close();
    }
    void HandleTheOTA::loadDatafile(const char* ESPname){         // load SSID and password from SPIFFS
        File dataFile = SPIFFS.open("/APData.csv", "r");
        String dataString;
        for (uint8_t a=0; a<dataFile.size();a++){
				dataString += char(dataFile.read());
			}
        String name = dataString.substring(0,(dataString.indexOf(",")));
        String password1 = dataString.substring((dataString.indexOf(",")+1),dataString.length()-2);
        WiFi.begin(name.c_str(), password1.c_str());
        WiFi.hostname(ESPname);
        dataFile.close();
    }
    void HandleTheOTA::connectToAP(){
        unsigned long stationModeTimeout = millis() + 10000;
        while ( WiFi.status() != WL_CONNECTED ) {
	        if (millis() > stationModeTimeout){//Failed to connect, Switching to AP mode
                WiFi.mode(WIFI_AP);
                int channel = random(1, 13 + 1); // have to add 1 or will be 1 - 12
                const byte DNS_PORT = 53;
                IPAddress subnet(255, 255, 255, 0);
                IPAddress apIP(192, 168, 4, 1);
                WiFi.softAPConfig(apIP, apIP, subnet);
                WiFi.softAP(AP_NameH, passwordH, channel, false);
                dnsServerH->start(DNS_PORT, "*", apIP);
		        break;
	        }
            delay(50);
        }
    }
