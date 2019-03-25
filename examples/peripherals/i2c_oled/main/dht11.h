#ifndef __I2C_OLED_MASTER_H__
#define __I2C_OLED_MASTER_H__
unsigned char dht11_data_array[6] = {0};      // DHT11数据数组
unsigned char dht11_data_char[2][10]={0};     // DHT11数据字符串【行：湿/温度】【列：数据字符串】
uint8_t dht11_read_data_complete(void);
void dht11_working(void);
unsigned char * get_data_string(char select);
void gpio_init(void);
#endif
