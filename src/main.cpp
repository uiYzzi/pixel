#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>
#include <time.h>
#include <Adafruit_NeoPixel.h>
#include <esp_wifi.h>
#include <esp_system.h>
#include "drivers/Button.h"
#include "drivers/LEDMatrix.h"
#include "drivers/Buzzer.h"
#include "Program/TimeProgram"
#include "Program/CodeRainProgram"
#include "Program/BilibiliFansProgram"
#include "Program/TemperatureProgram"
#include "Program/HumidityProgram"
#include "Program/BaseProgram.h"
#include "config.h"
#include "../asset/WifiImg.h"


// 定义音符频率
#define C4 262
#define D4 294
#define E4 330
#define F4 349
#define G4 392
#define A4 440
#define B4 494
#define C5 523

// 定义音符时值（单位为毫秒）
#define WHOLE_NOTE 1000
#define HALF_NOTE 500
#define QUARTER_NOTE 250
#define EIGHTH_NOTE 125

Adafruit_NeoPixel ledStrip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
LEDMatrix screen(ledStrip, SCREEN_WIDTH, SCREEN_HEIGHT);
Buzzer buzzer(BUZZER_PIN);
Button backButton(BACK_BUTTON_PIN,5000);
Button goButton(GO_BUTTON_PIN);
std::vector<int> wifi_connect_x = {24, 26, 28};
// Configure NTP client
WiFiUDP udp;
NTPClient ntpClient(udp, NTP_SERVER, TIME_ZONE_OFFSET_IN_SECONDS, UPDATE_INTERVALMILLIS);
DHT dht(DHT_PIN,DHTTYPE);
TimeProgram timeProgram(screen,ntpClient);
CodeRainProgram codeRainProgram(screen);
BilibiliFansProgram bilibiliFansProgram(screen);
TemperatureProgram temperatureProgram(screen,dht);
HumidityProgram humidityProgram(screen,dht);
BaseProgram* programs[] = { &timeProgram, &codeRainProgram, &bilibiliFansProgram, &temperatureProgram, &humidityProgram };
double bright = 0.1;
int appIndex = 0;   // 当前应用的索引
unsigned long lastSwitchTime = 0;   // 上一次切换应用的时间

// 定义小星星旋律数组（每个元素包含音符频率和时值）
int melody[][2] = {
  {C4, QUARTER_NOTE}, {C4, QUARTER_NOTE}, {G4, QUARTER_NOTE}, {G4, QUARTER_NOTE},
  {A4, QUARTER_NOTE}, {A4, QUARTER_NOTE}, {G4, HALF_NOTE},
  {F4, QUARTER_NOTE}, {F4, QUARTER_NOTE}, {E4, QUARTER_NOTE}, {E4, QUARTER_NOTE},
  {D4, QUARTER_NOTE}, {D4, QUARTER_NOTE}, {C4, HALF_NOTE}
};
// 定义小星星旋律数组的长度
int melodyLength = sizeof(melody) / sizeof(melody[0]);

template <class T>
int getArrayLen(T &array){return sizeof(array) / sizeof(array[0]);}

// 定义一个函数，用于播放小星星旋律
void playTwinkleTwinkle() {
  // 遍历旋律数组中的每个元素
  for (int i = 0; i < melodyLength; i++) {
    // 获取当前元素中的音符频率和时值
    int frequency = melody[i][0];
    int duration = melody[i][1];
    // 调用Buzzer类的beep方法，播放当前音符
    buzzer.beep(frequency, duration);
    // 播放完毕后，暂停一小会儿，增加节奏感
    delay(50);
  }
}

void SmartConfig()
{
   WiFi.mode(WIFI_STA);
   Serial.println("\r\n wait for smartconfig....");
   WiFi.beginSmartConfig();
   while(1)
   {
    Serial.print(".");
    delay(500);
    if ( WiFi.smartConfigDone())
    {
      Serial.println("SmartConfig Success");
      Serial.printf("SSID:%s\r\n",WiFi.SSID().c_str());
      Serial.printf("PSW:%s\r\n",WiFi.psk().c_str());
      break;      
    }
   }  
}

bool AutoConfig()
{
  WiFi.begin();
  for (int i=0; i<20; i++)
  {
    int wstatus = WiFi.status();
    if (wstatus == WL_CONNECTED )  
       {
          Serial.println("wifi smartConfig success");
          Serial.printf("SSID:%s",WiFi.SSID().c_str());
          Serial.printf(",PWS:%s\r\n",WiFi.psk().c_str());
          Serial.print("localIP:");
          Serial.println(WiFi.localIP());
          Serial.print(",GateIP:");
          Serial.println(WiFi.gatewayIP());
          return true;
          
       }
       else
       {
          Serial.print("WIFI AutoConfig Waiting ....");
          Serial.println(wstatus);
          delay(1000);
        
       }
    
  }
  Serial.println("Wifi autoconfig faild!");
  return false;
}

void setup() {
    screen.fill();
    screen.drawImage(0,0,"[0,0,0,65318,58761,0,0,0,0,0,65517,65517,65318,58761,0,0,0,0,65517,65512,65318,58761,0,0,0,0,65318,65318,65318,65318,0,0,0,58761,58761,58761,58761,62853,62853,0,0,58761,65318,65318,65318,58761,58761,0,65318,65318,62853,62853,62853,62853,58761,58761,0,0,62853,62853,65318,58761,0,0]",bright);
    //screen.drawImage(0, 0, wifi22x8[0], bright);
    screen.show();
    Serial.begin(115200);
    delay(100);
    if (!AutoConfig())
    {
      SmartConfig();
    }
    // playTwinkleTwinkle();
    gpio_install_isr_service(0);
    backButton.begin();
    goButton.begin();
    ledStrip.begin();
    ledStrip.show();
    screen.drawImage(23, 0, wifi22x8[1], bright);
    ntpClient.update();
    screen.show();
    delay(1000);
}

void loop() {
    if (backButton.wasPressed()) {
      Serial.println("Back Button was pressed!");
    }
    if (backButton.longPressDetected()) {
      esp_wifi_restore();
      Serial.println("Wifi autoconfig restore!");
      esp_restart();
    }
    if (goButton.wasPressed()) {
      Serial.println("Go Button was pressed!");
    }

    // 当前时间
    unsigned long currentTime = millis();
    //100000
    if (currentTime - lastSwitchTime >= 2000 || (appIndex != 0 && currentTime - lastSwitchTime >= 2000)) {
        appIndex = (appIndex + 1) % getArrayLen(programs);   // 切换到下一个应用
        size_t freeHeap = esp_get_free_heap_size();
        Serial.print("Free heap: ");
        Serial.println(freeHeap);
        
        lastSwitchTime = currentTime;
    }

    programs[appIndex]->run(bright);

    // 延迟一段时间，避免过于频繁地切换应用
    delay(100);
}
