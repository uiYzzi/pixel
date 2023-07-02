#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>
#include <time.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <esp_wifi.h>
#include <esp_system.h>
#include "drivers/Button.h"
#include "drivers/LEDMatrix.h"
#include "drivers/Buzzer.h"
#include "drivers/Photoresistor.h"
#include "Program/TimeProgram"
#include "Program/CodeRainProgram"
#include "Program/BilibiliFansProgram"
#include "Program/TemperatureProgram"
#include "Program/HumidityProgram"
#include "Program/BaseProgram.h"
#include "config.h"
#include "../asset/Img.h"

Adafruit_NeoPixel ledStrip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
LEDMatrix screen(ledStrip, SCREEN_WIDTH, SCREEN_HEIGHT);
Buzzer buzzer(BUZZER_PIN);
Button backButton(BACK_BUTTON_PIN,5000);
Button goButton(GO_BUTTON_PIN);
Button upButton(UP_BUTTON_PIN);
Button downButton(DOWN_BUTTON_PIN);
std::vector<int> wifi_connect_x = {24, 26, 28};
// Configure NTP client
WiFiUDP udp;
NTPClient ntpClient(udp, NTP_SERVER, TIME_ZONE_OFFSET_IN_SECONDS, UPDATE_INTERVALMILLIS);
DHT dht(DHT_PIN,DHTTYPE);
WebServer server(80);
Photoresistor pr(PHOTORESISTOR_PIN, 4700, 1.1);
TimeProgram timeProgram(screen,ntpClient);
CodeRainProgram codeRainProgram(screen);
BilibiliFansProgram bilibiliFansProgram(screen);
TemperatureProgram temperatureProgram(screen,dht);
HumidityProgram humidityProgram(screen,dht);
BaseProgram* programs[] = { &timeProgram, &codeRainProgram, &bilibiliFansProgram, &temperatureProgram, &humidityProgram };
double bright = 0.5;
int appIndex = 0;   // 当前应用的索引
unsigned long lastSwitchTime = 0;   // 上一次切换应用的时间

template <class T>
int getArrayLen(T &array){return sizeof(array) / sizeof(array[0]);}

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

void handleRoot() {
  server.send(200, "text/plain", "Hello from ESP32!");
}

void handleAnimation() {
  if (server.args() == 5) {
    int16_t xPos = server.arg(0).toInt();
    int16_t yPos = server.arg(1).toInt();
    const char* json = server.arg(2).c_str();
    float bright = server.arg(3).toFloat();
    int16_t animationDelay = server.arg(4).toInt();

    // 调用 playAnimation 函数进行显示
    screen.fill();
    screen.playAnimation(xPos, yPos, json, animationDelay, bright);
    server.send(200, "text/plain", "Animation played!");
  } else {
    server.send(400, "text/plain", "Invalid number of arguments.");
  }
}

void setup() {
    screen.fill();
    screen.drawImage(0,0,img_wifi,0,bright);
    screen.drawChar('W',10,0,255,255,255,bright);
    screen.drawChar('I',14,0,255,255,255,bright);
    screen.drawChar('F',18,0,255,255,255,bright);
    screen.drawChar('I',22,0,255,255,255,bright);
    screen.show();
    Serial.begin(115200);
    delay(100);
    if (!AutoConfig())
    {
      SmartConfig();
    }

    gpio_install_isr_service(0);
    backButton.begin();
    goButton.begin();
    upButton.begin();
    downButton.begin();
    ledStrip.begin();
    ledStrip.show();
    screen.fill();
    screen.drawImage(0,0,img_wifi,0,bright);
    screen.drawImage(16,0,img_true,0,bright);
    screen.show();
    delay(1000);
    for(int i = 0;i<getArrayLen(programs);i++)
    {
      programs[i]->begin();
    }
    server.on("/", handleRoot);
    server.on("/animation", handleAnimation);
    server.begin();
    Serial.println("HTTP server started");
    ntpClient.update();
}

void loop() {
    server.handleClient();
    backButton.update();
    goButton.update();
    upButton.update();
    downButton.update();

    float r = pr.getResistance();
    // 使用一个滑动平均滤波器
    static float r_array[10];
    static int index = 0;
    r_array[index] = r;
    index = (index + 1) % 10;
    float r_avg = 0;
    for (int i = 0; i < 10; i++) {
      r_avg += r_array[i];
    }
    r_avg /= 10;
    
    // 使用一个线性函数
    float bright = (18000 - r_avg) / (18000 - 4500);
    
    // 使用一个限幅函数
    bright = constrain(bright, 0, 1);

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
    if (upButton.wasPressed()) {
      Serial.println("Up Button was pressed!");
      if(appIndex <= 0)
      {
        appIndex = getArrayLen(programs) - 1;
      }else{
        appIndex -= 1;
      }
    }
    if (downButton.wasPressed()) {
      Serial.println("Down Button was pressed!");
      appIndex = (appIndex + 1) % getArrayLen(programs);   // 切换到下一个应用
    }
    // size_t freeHeap = esp_get_free_heap_size();
    // Serial.print("Free heap: ");
    // Serial.println(freeHeap);
    // 当前时间
    unsigned long currentTime = millis();

    if (currentTime - lastSwitchTime >= 100000 || (appIndex != 0 && currentTime - lastSwitchTime >= 50000)) {
        appIndex = (appIndex + 1) % getArrayLen(programs);   // 切换到下一个应用
        
        lastSwitchTime = currentTime;
    }

    programs[appIndex]->update(bright);

    // 延迟一段时间，避免过于频繁地切换应用
    delay(100);
}
