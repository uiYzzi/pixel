#include "Photoresistor.h"
#include <driver/adc.h>

Photoresistor::Photoresistor() {
    adc1_config_width(ADC_WIDTH_BIT_13);
    adc1_config_channel_atten(ADC1_CHANNEL_6 ,ADC_ATTEN_0db);
}

float Photoresistor::read() {
    float read_raw=adc1_get_raw(ADC1_CHANNEL_6); //采集ADC1 6通道的ADC值
    return (read_raw*1100)/4096; // 以毫伏为单位的电压值
}
