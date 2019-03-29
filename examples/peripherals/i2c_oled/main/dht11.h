#ifndef __I2C_OLED_MASTER_H__
#define __I2C_OLED_MASTER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_system.h"

#define GPIO_OUTPUT_IO_0     5 
#define GPIO_OUTPUT_IO_1     4
#define GPIO_OUTPUT_IO_2     12
#define GPIO_OUTPUT_PIN_SEL_1  (1ULL<<GPIO_OUTPUT_IO_2)
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))
uint8_t dht11_data_array[6] = {0};      // DHT11数据数组
uint8_t dht11_data_char[2][10]={0};     // DHT11数据字符串【行：湿/温度】【列：数据字符串】
uint8_t dht11_read_data_complete(void);
void dht11_working(void);
uint8_t * get_data_string(char select);
uint8_t get_data(char select);
void gpio_init(void);
void gpio_init1(void);
#endif
