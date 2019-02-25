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

#include "LEDMatrix.h"

    LEDMatrix::LEDMatrix(){// constructor

        pinMode(Latch, OUTPUT);
        fastDigitalWrite(Latch,HIGH);

        SPI.begin();
        SPI.beginTransaction(SPISettings(39000000,MSBFIRST,SPI_MODE0));// get bit errors above 39MHZ

        for(int a=0;a<8;a++){
            rowMask[a]=B11111111 ^ (1<<a);
        }

        for (int a=0;a<8;a++){
            currentMatrix[a] = 0;
        }
        clearMatrix();

    }
    void LEDMatrix::clearMatrix(){
            fastDigitalWrite(Latch, LOW);
            SPI.write(B00000000);
            SPI.write(B11111111);
            fastDigitalWrite(Latch, HIGH);
            // Wifi symbol
            //0x181818422499423c
    }
    ICACHE_RAM_ATTR void LEDMatrix::fastDigitalWrite(int pin,bool State){
	    if (State){
		    GPOS = (1 << pin); // HIGH
		    return;
	    }
	    GPOC = (1 << pin); // LOW
    }
    ICACHE_RAM_ATTR void LEDMatrix::T1IntHandler(){
            
            byte row = (currentMatrix[currentBrightness] >> (currentRow * 8)) & 0xFF;
            fastDigitalWrite(Latch, LOW);
            SPI.write(row);
            SPI.write(rowMask[currentRow]);
            fastDigitalWrite(Latch, HIGH);
            currentRow++;
            if (currentRow>7){
              currentRow = 0;
              currentBrightness++;
              if (currentBrightness>7)currentBrightness=0;
            }
    }
    void LEDMatrix::setMatrix(uint64_t IMAGE, int angle){ // add image without brightness
      if (angle != 0){
        //TODO: modify pixels and modify brightness for pixels partly in next place
      }else{
        IMAGE = rotateCW(IMAGE);
        for (int a=0;a<8;a++){
            currentMatrix[a] = IMAGE;
        }
      }
    }
    void LEDMatrix::setMatrix(uint64_t* IMAGE, int angle){
      if (angle != 0){
          
      }else{
        for (int a=0;a<8;a++){
            currentMatrix[a] = IMAGE[a];
        }
      }
    }
    uint64_t LEDMatrix::rotateCW(uint64_t IMAGE){
      uint64_t result = 0;
      int r=1,c=1;
      for (int a=0;a<64;a++){
        bitWriteU(result,(r*8 - c),bitReadU(IMAGE,a));
        r++;
        if (r>8){
          r=1;
          c++;
        }
      }
      return result;
    }
    void LEDMatrix::scroll(){
      if (scrollText==""){// setup scrolling
        scrollShift=0;
        scrollText = text;
        scrollText +=" ";
        setMatrix(uint64_t(0),0);
        uint64_t nextLetter = rotateCW(matrixFont[uint8_t(scrollText.charAt(0))-32]);
        for (int a=0;a<8;a++){
          nextMatrix[a] = nextLetter;
        }
      }
      for(int a=0;a<8;a++){
        currentMatrix[a] = (currentMatrix[a] >> 8); // scroll left
        if(scrollShift==8){// add extra line
          nextMatrix[a]=0;
        }
        if(scrollShift>8){
          scrollShift=0;
          if (scrollText !=""){
            scrollText.remove(0,1);
            uint64_t nextLetter = rotateCW(matrixFont[uint8_t(scrollText.charAt(0))-32]);
            for (int b=0;b<8;b++){
              nextMatrix[b] = nextLetter;
            }
          }else{
            break;
          }
        }
        unsigned long long newrow = (nextMatrix[a] >> (scrollShift * 8)) & 0xFF;//unsigned long long newrow = (currentMatrix[a] >> currentRow * 8) & 0xFF;
        currentMatrix[a] = (currentMatrix[a]  | (newrow << 56));//0xFFULL
      }
      scrollShift++;
    }