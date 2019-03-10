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
#include "WebFiles.h"
#include <FS.h>

void sendFile(String path, ESP8266WebServer * server){
  while(path.indexOf("%20") != -1){ // fix spaces in path
    path.replace("%20"," ");
  }
  if(path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
	  server->setContentLength(file.size());
    size_t sent = server->streamFile(file, contentType);
    file.close();
    return;
  }
  server->send(404, "text/plain", "Failed to read file.");
  return;
}

String getContentType(String path) { // get content type
    String dataType = F("text/html");
    String lowerPath = path.substring(path.length() - 4, path.length());
    lowerPath.toLowerCase();
    if (lowerPath.endsWith(".src")) lowerPath = lowerPath.substring(0, path.lastIndexOf("."));
      else if (lowerPath.endsWith(".gz")) dataType = F("application/x-gzip");
      else if (lowerPath.endsWith(".html")) dataType = F("text/html");
      else if (lowerPath.endsWith(".htm")) dataType = F("text/html");
      else if (lowerPath.endsWith(".png")) dataType = F("image/png");
      else if (lowerPath.endsWith(".js")) dataType = F("application/javascript");
      else if (lowerPath.endsWith(".css")) dataType = F("text/css");
      else if (lowerPath.endsWith(".gif")) dataType = F("image/gif");
      else if (lowerPath.endsWith(".jpg")) dataType = F("image/jpeg");
      else if (lowerPath.endsWith(".ico")) dataType = F("image/x-icon");
      else if (lowerPath.endsWith(".svg")) dataType = F("image/svg+xml");
      else if (lowerPath.endsWith(".mp3")) dataType = F("audio/mpeg");
      else if (lowerPath.endsWith(".wav")) dataType = F("audio/wav");
      else if (lowerPath.endsWith(".ogg")) dataType = F("audio/ogg");
      else if (lowerPath.endsWith(".xml")) dataType = F("text/xml");
      else if (lowerPath.endsWith(".pdf")) dataType = F("application/x-pdf");
      else if (lowerPath.endsWith(".zip")) dataType = F("application/x-zip");
    return dataType;
  }

void sendNoContent(ESP8266WebServer * server){
  server->send(204,"HTTP/1.1","NO CONTENT");
}