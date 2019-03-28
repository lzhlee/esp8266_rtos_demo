/* I2C example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"

#include "driver/gpio.h"

#include "oled.h"
#include "dht11.h"

static const char *TAG = "oled";

extern void gpio_init(void);
extern void dht11_working(void);
extern uint8_t get_data(char select);
extern uint8_t * get_data_string(char select);
extern void OLED_Init(void);
extern void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char Show_char);
extern void OLED_ShowString(unsigned char x, unsigned char y, unsigned char * Show_char);
extern void OLED_ShowIP(unsigned char x, unsigned char y, unsigned char*Array_IP);
extern uint8_t i2c0_pcf8591_adc_read(void);

void delay_ms(int C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}

void esp8266_init(void)
{
	gpio_init();
	OLED_Init();
}

static void oled_task_example(void *arg)
{
	int val = 0;
	uint8_t dat[3]={0};
//--------------------------------
// OLED显示字符串
//-------------------------------------------------------------------------------------------------------------------------
	OLED_ShowString(0,0,(unsigned char *)"Project=");		// 从(0,0)开始显示
	OLED_ShowString(64,0,(unsigned char *)"youyuan");		// 从(64,0)开始显示		// 因为，【Project=】一共8个字符，横向上占用【64】个点
//-------------------------------------------------------------------------------------------------------------------------
// OLED显示【点分十进制_IP地址】
//-------------------------------------------------------------------------------------------------------------------------
	uint8_t IP_Address[4];
	IP_Address[0] = 192;
	IP_Address[1] = 168;
	IP_Address[2] = 4;
	IP_Address[3] = 1;
	while(1)
	{
		dht11_working();
		OLED_ShowString(0,2,get_data_string(1));
		OLED_ShowString(0,4,get_data_string(0));
		val = i2c0_pcf8591_adc_read();
		dat[0] = ((255-val)*80/255)/10 + 48;
		dat[1] = ((255-val)*80/255)%10 + 48;
		dat[2] = 'C';
		OLED_ShowString(0,6,dat);
		if(get_data(1) > ((255-val)*80/255)){
			gpio_set_level(4,0);
		}else{
			gpio_set_level(4,1);
		}
    		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

void app_main(void)
{
    //start i2c task
  esp8266_init();
  xTaskCreate(oled_task_example, "oled_task_example", 2048, NULL, 10, NULL);
}
