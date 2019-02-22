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
#include <SPI.h>


class LEDMatrix{
    public:
        LEDMatrix();
        void clearMatrix();
        ICACHE_RAM_ATTR void T1IntHandler();

        uint32_t ticks;

    private:
        void fastDigitalWrite(int pin,bool State);
        volatile uint8_t currentRow = 0;
        const uint8_t Latch = D8;
        uint8_t rowMask[8];
        uint64_t IMAGES[57] = {
  0x0010107c10100000,
  0x0000003c00000000,
  0x006c38fe386c0000,
  0x00060c1830600000,
  0x60660c1830660600,
  0x00003c003c000000,
  0x000000365c000000,
  0x0000008244281000,
  0x6030180c18306000,
  0x060c1830180c0600,
  0x6030181818306000,
  0x060c1818180c0600,
  0x7818181818187800,
  0x1e18181818181e00,
  0x7018180c18187000,
  0x0e18183018180e00,
  0x0606000000000000,
  0x0018180018180000,
  0x0c18180018180000,
  0x060c0c0c00000000,
  0x180018183c3c1800,
  0x1800183860663c00,
  0x003c421a3a221c00,
  0xfc66a6143c663c00,
  0x103c403804781000,
  0x6c6cfe6cfe6c6c00,
  0x383838fe7c381000,
  0x10387cfe38383800,
  0x10307efe7e301000,
  0x1018fcfefc181000,
  0xfefe7c7c38381000,
  0x1038387c7cfefe00,
  0x061e7efe7e1e0600,
  0xc0f0fcfefcf0c000,
  0x7c92aa82aa827c00,
  0x7ceed6fed6fe7c00,
  0x10387cfefeee4400,
  0x10387cfe7c381000,
  0x381054fe54381000,
  0x38107cfe7c381000,
  0x00387c7c7c380000,
  0xffc7838383c7ffff,
  0x0038444444380000,
  0xffc7bbbbbbc7ffff,
  0x0c12129ca0c0f000,
  0x38444438107c1000,
  0x060e0c0808281800,
  0x066eecc88898f000,
  0x105438ee38541000,
  0x1038541054381000,
  0x6666006666666600,
  0x002844fe44280000,
  0x00000000286c6c00,
  0x006030180c060000,
  0x0000000060303000,
  0x0000000c18181800,
  0xfe8282c66c381000
};
const int IMAGES_LEN = sizeof(IMAGES)/8;
        
};