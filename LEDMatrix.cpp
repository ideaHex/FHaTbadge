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
        SPI.beginTransaction(SPISettings(20000000,MSBFIRST,SPI_MODE0));// get bit errors above 39MHZ

        for(int a=0;a<8;a++){
            rowMask[a]=65280 ^ (1<<a+8);
            currentMatrix[a] = 0;
        }
        clearMatrix();
}
void LEDMatrix::clearMatrix(){
            fastDigitalWrite(Latch, LOW);
            SPI.write(B00000000);
            SPI.write(B11111111);
            fastDigitalWrite(Latch, HIGH);
            // switch back to 16 bit mode
            const uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
            SPI1U1 = ((SPI1U1 & mask) | ((15 << SPILMOSI) | (15 << SPILMISO)));
}
ICACHE_RAM_ATTR void LEDMatrix::fastDigitalWrite(int pin,bool State){
	    if (State){
		    GPOS = (1 << pin); // HIGH
		    return;
	    }
	    GPOC = (1 << pin); // LOW
}
ICACHE_RAM_ATTR void LEDMatrix::T1IntHandler(){
            
            uint16_t row = (currentMatrix[currentBrightness] >> (currentRow * 8)) & 0xFF;

            /* Equal to 
            SPI.write(row);
            SPI.write(rowMask[currentRow]);
            but saves 2 whiles and 1 SPI1CMD |= SPIBUSY;
            */
            GPOC = (1 << Latch);// Latch LOW
            SPI1W0 = (rowMask[currentRow] | row);
            SPI1CMD |= SPIBUSY;
            while(SPI1CMD & SPIBUSY) {}
            GPOS = (1 << Latch);// Latch HIGH

            currentRow++;
            if (currentRow>7){
              currentRow = 0;
              currentBrightness++;
              if (currentBrightness>7)currentBrightness=0;
            }
}
void LEDMatrix::setMatrix(uint64_t IMAGE, int angle){ // add image without brightness
      if (angle != 0){
        for (int a=0;a<8;a++){
            currentMatrix[a] = IMAGE;
        }
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
void LEDMatrix::newScrollText(String newText){
      scrollText = "";
      text = newText;
}
void LEDMatrix::scroll(){
      if (scrollText==""){                          // setup scrolling
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
        if(scrollShift==8){                         // add extra line
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
          }
        }
        if (scrollText !=""){
          unsigned long long newrow = (nextMatrix[a] >> (scrollShift * 8)) & 0xFF;
          currentMatrix[a] = (currentMatrix[a]  | (newrow << 56));
        }
      }
      scrollShift++;
}
void LEDMatrix::createAnimation(uint64_t IMAGE,int frameNumber, unsigned long currentDelay){
      currentAnimationImagesNumberOfFrames = frameNumber;
      currentAnimationImages[currentAnimationImagesNumberOfFrames] = IMAGE;
      currentAnimationDelay = currentDelay;
      currentFrame = 0;
}
void LEDMatrix::animate(){
      for (int a=0;a<8;a++){
          currentMatrix[a] = currentAnimationImages[currentFrame];
        }
      currentFrame++;
      if (currentFrame > currentAnimationImagesNumberOfFrames) currentFrame = 0; //LOOP
}
void LEDMatrix::displayMatrix(int time, int dtime) {// matrix waterfall display

    bool shift = 0;
    scrollShift++;
    if (scrollShift > 2){
       shift = 1;
       scrollShift =0;
    }
		for (int r = NUM_ROW - 2; r >= 0; r--)
			for (int c = 0; c < NUM_COL; c++) {
				if (display[r][c] == 0)
					display[r + shift][c] = 0;
				else{
					display[r + shift][c] = display[r][c] - random(0,((display[r][c]*0.4)+1.6));//- random(2) * 2;
          if (display[r + shift][c] > 7)display[r + shift][c] = 7;
          else if (display[r + shift][c] == 0)display[r + shift][c] = 7;
        }
			}

    if (shift){
		uint8_t rnd = byte(random(256));
		for (int c = 0; c < 8; c++)
			display[0][c] = rnd & 1 << c ? 7 : 0;
    }
  convertIMAGEFromLastFHaTBadge();
}
void LEDMatrix::convertIMAGEFromLastFHaTBadge(){
      uint64_t row=0;
      uint64_t currentMatrixImage=0;
      for (uint8_t matrixLevel = 0;matrixLevel < 8;matrixLevel++){
        currentMatrixImage = 0;
        for (int c=0;c<8;c++){
          row=0;
          for (int a=0;a<8;a++){
            if (display[a][c] > matrixLevel){ // brighter than current level
              row = row | (1 << (7-a));
            }
          }
          currentMatrixImage = (currentMatrixImage | (row << (c * 8)));
        }
          currentMatrix[matrixLevel] = currentMatrixImage;
      }
}
void LEDMatrix::update(){
      switch(mode){
        case animationMode:
          animate();
          break;
        case textScrollMode:
          scroll();
          break;

        case staticMode:

          break;

        case matrixWaterfall:
          displayMatrix(10,125);
          break;

        default:
          break;
      }
}
void LEDMatrix::setMode(int newMode){
      mode = newMode;
}