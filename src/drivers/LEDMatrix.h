#ifndef LEDMATRIX_H_
#define LEDMATRIX_H_

#include <vector>
#include <Adafruit_NeoPixel.h>

class LEDMatrix {
public:
    LEDMatrix(Adafruit_NeoPixel &led, uint16_t bufferWidth, uint16_t bufferHeight);
    void drawPixel(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b, float bright=0.5);
    void drawChar(char c, int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b, float bright=0.5);
    void drawCharForIndex(int16_t index, int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b, float bright=0.5);
    void drawImage(int16_t x, int16_t y, std::vector<std::vector<std::vector<int>>> imgData, float bright);
    void drawImage(int16_t xPos, int16_t yPos,const char* json, float bright);
    void intToRGB(int value, int& r, int& g, int& b);
    void fill(uint8_t r=0, uint8_t g=0, uint8_t b=0);
    void show();
private:
    void hexToBin(unsigned char hex, bool *binArray);
    Adafruit_NeoPixel &_led;
    uint16_t _index;
    uint16_t _w;
    uint16_t _h;
};
#endif /* LEDMATRIX_H_ */
