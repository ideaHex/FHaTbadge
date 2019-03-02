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
#ifndef _LED_MATRIX_
#define _LED_MATRIX_

#pragma GCC optimize("-O2")

#include <ESP8266WiFi.h>
#include <SPI.h>

#define bitReadU(value, bit) (((value) >> (bit)) & 0x01)
#define bitSetU(value, bit) ((value) |= (1ULL << (bit)))
#define bitClearU(value, bit) ((value) &= ~(1ULL << (bit)))
#define bitWriteU(value, bit, bitvalue) (bitvalue ? bitSetU(value, bit) : bitClearU(value, bit))

#define animationMode 0
#define textScrollMode 1
#define staticMode 3

class LEDMatrix{
    public:
        LEDMatrix();
        void clearMatrix();
        ICACHE_RAM_ATTR void T1IntHandler();
        void setMatrix(uint64_t IMAGE, int angle);
        void setMatrix(uint64_t* IMAGE, int angle);
        uint64_t rotateCW(uint64_t IMAGE);
        void scroll();
        void createAnimation(uint64_t IMAGE,int frameNumber, unsigned long currentDelay);
        void update();
        void setMode(int newMode);
        void newScrollText(String newText);

        uint32_t ticks;
        String text;
        

    private:
        ICACHE_RAM_ATTR void fastDigitalWrite(int pin,bool State);
        void animate(void);

        volatile uint8_t currentRow = 0;
        const uint8_t Latch = D8;
        uint16_t rowMask[8];
        volatile int currentBrightness = 0;
        volatile int brightnessCount = 0;
        uint64_t currentMatrix[8];//Each segment of this array will contain different brightness data,
        //level 0 will have all pixels on, as it progresses pixels will be set to 0 to make them dimmer
        uint64_t nextMatrix[8];
        int scrollShift=0;
        String scrollText;
        uint64_t currentAnimationImages[100];
        uint8_t currentAnimationImagesNumberOfFrames = 0;
        int currentAnimationDelay=75;
        uint8_t currentFrame=0;
        int mode = textScrollMode;
};

    static uint64_t matrixFont[] = {
  0x0000000000000000,   // Space
  0x180018183c3c1800,   // !
  0x00000000286c6c00,   // "
  0x6c6cfe6cfe6c6c00,   // #
  0x103c403804781000,   // $
  0x60660c1830660600,   // %
  0xfc66a6143c663c00,   // &
  0x000000000c060600,   // '
  0x6030181818306000,   // (
  0x060c1818180c0600,   // )
  0x006c38fe386c0000,   // *
  0x0010107c10100000,   // +
  0x060c0c0c00000000,   // ,
  0x0000003c00000000,   // -
  0x0606000000000000,   // .
  0x00060c1830600000,   // /
  0x3c66666e76663c00,   // 0
  0x7e1818181c181800,   // 1
  0x7e060c3060663c00,   // 2
  0x3c66603860663c00,   // 3
  0x30307e3234383000,   // 4
  0x3c6660603e067e00,   // 5
  0x3c66663e06663c00,   // 6
  0x1818183030667e00,   // 7
  0x3c66663c66663c00,   // 8
  0x3c66607c66663c00,   // 9
  0x0018180018180000,   // :
  0x0c18180018180000,   // ;
  0x6030180c18306000,   // <
  0x00003c003c000000,   // =
  0x060c1830180c0600,   // >
  0x1800183860663c00,   // ?
  0x001e037b7b7b633e,   //@
  0x6666667e66663c00,   //A
  0x3e66663e66663e00,
  0x3c66060606663c00,
  0x3e66666666663e00,
  0x7e06063e06067e00,
  0x0606063e06067e00,
  0x3c66760606663c00,
  0x6666667e66666600,
  0x3c18181818183c00,
  0x1c36363030307800,
  0x66361e0e1e366600,
  0x7e06060606060600,
  0xc6c6c6d6feeec600,
  0xc6c6e6f6decec600,
  0x3c66666666663c00,
  0x06063e6666663e00,
  0x603c766666663c00,
  0x66361e3e66663e00,
  0x3c66603c06663c00,
  0x18181818185a7e00,
  0x7c66666666666600,
  0x183c666666666600,
  0xc6eefed6c6c6c600,
  0xc6c66c386cc6c600,
  0x1818183c66666600,
  0x7e060c1830607e00,
  0x7818181818187800,
  0x1e18181818181e00,
  0x006030180c060000,
  0x0000008244281000,
  0x7e7e000000000000,
  0x000000000c060600,
  0x7c667c603c000000,
  0x3e66663e06060600,
  0x3c6606663c000000,
  0x7c66667c60606000,
  0x3c067e663c000000,
  0x0c0c3e0c0c6c3800,
  0x3c607c66667c0000,
  0x6666663e06060600,
  0x3c18181800180000,
  0x1c36363030003000,
  0x66361e3666060600,
  0x3c18181818181c00,
  0xd6d6feeec6000000,
  0x6666667e3e000000,
  0x3c6666663c000000,
  0x06063e66663e0000,
  0xf0b03c36363c0000,
  0x060666663e000000,
  0x3e403c027c000000,
  0x1818187e18180000,
  0x7c66666666000000,
  0x183c666600000000,
  0x7cd6d6d6c6000000,
  0x663c183c66000000,
  0x3c607c6666000000,
  0x3c0c18303c000000,   // z
  0x7018180c18187000,   // {
  0x0018181818181800,   // |
  0x0e18183018180e00,   // }
  0x000000365c000000    // ~
};
const int fontLength = sizeof(matrixFont)/8;
#endif