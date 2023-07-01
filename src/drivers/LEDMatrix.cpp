#include "LEDMatrix.h"
#include "../asset/Font.h"

template <class T>
int getArrayLen(T &array){return sizeof(array) / sizeof(array[0]);}

LEDMatrix::LEDMatrix(Adafruit_NeoPixel &led, uint16_t bufferWidth, uint16_t bufferHeight) : _led(led), _index(0), _w(bufferWidth), _h(bufferHeight)
{
}

void LEDMatrix::drawPixel(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b, float bright)
{
    if (bright > 1)
    {
        bright = 1;
    }
    else if (bright < 0)
    {
        bright = 0;
    }

    if (0 <= x && x < _w && 0 <= y && y < _h)
    { // 判断xy是否超出屏幕范围
        if (0 <= x && x <= 7)
        { // 因为灯板的安装方式，导致led索引排布不规律，需要单独判断
            _index = (199 - x) + (7 - y) * 8;
        }
        else if (8 <= x && x <= 15)
        {
            _index = (143 - x) + (7 - y) * 8;
        }
        else if (16 <= x && x <= 23)
        {
            _index = (87 - x) + (7 - y) * 8;
        }
        else if (24 <= x && x <= 31)
        {
            _index = (31 - x) + (7 - y) * 8;
        }
    }
    if (-8 <= x && x < 0)
    { // 屏幕左边超出屏幕范围，为了方便做动画出入
        _index = (255 - x) + (7 - y) * 8;
    }
    if (_w <= x && x <= 39)
    { // 屏幕右边超出屏幕范围，为了方便做动画出入
        _index = (-25 - x) + (7 - y) * 8;
    }
    _led.setPixelColor(_index, r * bright, g * bright, b * bright);
}

void LEDMatrix::drawImage(int16_t x, int16_t y, std::vector<std::vector<std::vector<int>>> imgData, float bright)
{
    for (int i = 0; i < imgData.size(); i++)
    {
        for (int j = 0; j < imgData[0].size(); j++)
        {
            if (i >= _h || j >= _w)  // Skip drawing image data that is out of screen range
                continue;
            drawPixel(x + j, y + i, imgData[i][j][0], imgData[i][j][1], imgData[i][j][2], bright);
        }
    }
}

void LEDMatrix::drawImage(int16_t xPos, int16_t yPos, const char* json, int frameIndex, float bright) {
  DynamicJsonDocument doc(7000);
  deserializeJson(doc, "[" + std::string(json) + "]");

  JsonArray frameArray = doc[frameIndex];
  int frameHeight = 8;
  int frameWidth = frameArray.size() / frameHeight;

  drawFrame(xPos, yPos, frameArray, frameWidth, frameHeight, bright);
}

void LEDMatrix::playAnimation(int16_t xPos, int16_t yPos, const char* json, int16_t animationDelay, float bright) {
  DynamicJsonDocument doc(7000);
  deserializeJson(doc, "[" + std::string(json) + "]");
  int totalFrames = doc.size();
  
  for (int i = 0; i < totalFrames; i++) {
    JsonArray frameArray = doc[i];
    int frameHeight = 8;
    int frameWidth = frameArray.size() / frameHeight;
    
    drawFrame(xPos, yPos, frameArray, frameWidth, frameHeight, bright);
    show();
    delay(animationDelay);
  }
}

void LEDMatrix::drawFrame(int16_t xPos, int16_t yPos, JsonArray frameArray, int frameWidth, int frameHeight, float bright) {
  if (frameWidth > 8) {
    for (int i = 0; i < frameHeight; i++) {
      for (int j = 0; j < frameWidth; j++) {
        int index = i * frameWidth + j;
        int value = frameArray[index];
        int r, g, b;
        intToRGB(value, r, g, b);
        drawPixel(j + xPos, i + yPos, r, g, b);
      }
    }
  } else {
    for (int i = 0; i < frameHeight; i++) {
      for (int j = 0; j < frameWidth; j++) {
        int index = i * frameWidth + j;
        int value = frameArray[index];
        int r, g, b;
        intToRGB(value, r, g, b);
        drawPixel(j + xPos, i + yPos, r, g, b,bright);
      }
    }
  }
}



int LEDMatrix::getTotalFrames(const char* json) {
  DynamicJsonDocument doc(3000);
  deserializeJson(doc, "[" + std::string(json) + "]");
  return doc.size();
}

// Function to convert hex to binary string
void LEDMatrix::hexToBin(unsigned char hex, bool *binArray)
{
    // Loop through each bit of the hex value
    for (int i = 7; i >= 0; i--)
    {
        // Shift the hex value right by i bits and AND with 1 to get the ith bit
        int bit = (hex >> i) & 1;

        // Store the bit in the binArray
        *binArray++ = bit;
    }
}

void LEDMatrix::drawChar(char c, int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b, float bright)
{
    int16_t index = -1;

    // Find the corresponding index
    for (int i = 0; i < getArrayLen(idxArray); i++)
    {
        if (idxArray[i] == c)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        // Character not found in fontArray
        return;
    }

    // Initialize 8x8 binary array
    bool binArray[8][8];

    // Loop through each byte in the fontArray
    for (int i = 0; i < 8; i++)
    {
        // Convert the hex value to an 8-bit binary string
        hexToBin(fontArray[index * 8 + i], binArray[i]);
    }

    // Display the binArray
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (binArray[i][j])
            {
                int16_t xPos = x + j;
                int16_t yPos = y + i;
                this->drawPixel(xPos, yPos, r, g, b, bright);
            }
        }
    }
}

void LEDMatrix::drawCharForIndex(int16_t index, int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b, float bright)
{
    // Initialize 8x8 binary array
    bool binArray[8][8];

    // Loop through each byte in the fontArray
    for (int i = 0; i < 8; i++)
    {
        // Convert the hex value to an 8-bit binary string
        hexToBin(fontArray[index * 8 + i], binArray[i]);
    }

    // Display the binArray
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (binArray[i][j])
            {
                int16_t xPos = x + j;
                int16_t yPos = y + i;
                this->drawPixel(xPos, yPos, r, g, b, bright);
            }
        }
    }
}

void LEDMatrix::intToRGB(int value, int& r, int& g, int& b) {
	// 将16位整数分解为红、绿、蓝三个通道的亮度值
	r = (value >> 11) & 0x1F;
	g = (value >> 5) & 0x3F;
	b = value & 0x1F;
	
	// 缩放每个通道的亮度值到0-255的范围内
	r = (r * 255) / 31;
	g = (g * 255) / 63;
	b = (b * 255) / 31;
}

void LEDMatrix::fill(uint8_t r, uint8_t g, uint8_t b)
{
    _led.fill(_led.Color(r, g, b));
}

void LEDMatrix::show()
{
    _led.show();
}
