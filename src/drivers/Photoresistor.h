#ifndef PHOTORESISTOR_H_
#define PHOTORESISTOR_H_
class Photoresistor {
  private:
    int pin; // ADC引脚
    float R; // 并联电阻的阻值，单位欧姆
    float Vcc; // 供电电压，单位伏特
  public:
    // 构造函数，传入引脚，电阻和电压
    Photoresistor(int pin, float R, float Vcc) {
      this->pin = pin;
      this->R = R;
      this->Vcc = Vcc;
      pinMode(pin, INPUT); // 设置引脚为输入模式
    }
    // 获取光敏电阻的阻值，单位欧姆
    float getResistance() {
      int adc = analogRead(pin); // 读取ADC值，范围0-4095
      float Vout = adc * Vcc / 4095.0; // 计算输出电压，单位伏特
      float Rp = R * Vout / (Vcc - Vout); // 计算光敏电阻的阻值，单位欧姆
      return Rp;
    }
    // 获取光敏电阻的光强度，单位勒克斯
    float getLux() {
      float Rp = getResistance(); // 获取光敏电阻的阻值，单位欧姆
      float lux = 500 / Rp; // 根据GL5539的数据手册，计算光强度，单位勒克斯
      return lux;
    }
};
#endif /* PHOTORESISTOR_H_ */