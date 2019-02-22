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
        SPI.beginTransaction(SPISettings(25000000,MSBFIRST,SPI_MODE0));

        for(int a=0;a<8;a++){
            rowMask[a]=B11111111 ^ (1<<a);
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
    void LEDMatrix::fastDigitalWrite(int pin,bool State){
	    if (State){
		    GPOS = (1 << pin); // HIGH
		    return;
	    }
	    GPOC = (1 << pin); // LOW
    }
    ICACHE_RAM_ATTR void LEDMatrix::T1IntHandler(){
            fastDigitalWrite(Latch, LOW);
            byte row = (IMAGES[23] >> currentRow * 8) & 0xFF; //(millis() / 1000) * ((millis() / 1000)<57)
            SPI.write(row);
            SPI.write(rowMask[currentRow]);
            fastDigitalWrite(Latch, HIGH);
            currentRow++;
            if (currentRow>7)currentRow = 0;
    }
/*
void displayImage(uint64_t image) {
  for (int i = 0; i < 8; i++) {
    byte row = (image >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      display.setLed(0, i, j, bitRead(row, j));
    }
  }
}
*/