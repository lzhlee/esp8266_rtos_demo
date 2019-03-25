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

#include "oled.h"
#include "dht11.h"
#include "pcf8591.h"

static const char *TAG = "oled";

extern void gpio_init(void);
extern void dht11_working(void);
extern unsigned char * get_data_string(char select);
extern void iic_pcf8591_init(void);
extern uint16_t pcf8591_adc_read(void);
extern void OLED_Init(void);
extern void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char Show_char);
extern void OLED_ShowString(unsigned char x, unsigned char y, unsigned char * Show_char);
extern void OLED_ShowIP(unsigned char x, unsigned char y, unsigned char*Array_IP);

void delay_ms(int C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}

static void oled_task_example(void *arg)
{
	int val = 0;
	unsigned char dat[3]={0};
//--------------------------------
	iic_pcf8591_init();
	OLED_Init();	// OLED初始化
// OLED显示字符串
//-------------------------------------------------------------------------------------------------------------------------
	OLED_ShowString(0,0,(unsigned char *)"Project=");		// 从(0,0)开始显示
	OLED_ShowString(64,0,(unsigned char *)"youyuan");		// 从(64,0)开始显示		// 因为，【Project=】一共8个字符，横向上占用【64】个点
//-------------------------------------------------------------------------------------------------------------------------
// OLED显示【点分十进制_IP地址】
//-------------------------------------------------------------------------------------------------------------------------
	unsigned char IP_Address[4];
	   IP_Address[0] = 192;
	   IP_Address[1] = 168;
	   IP_Address[2] = 4;
	   IP_Address[3] = 1;
//	OLED_ShowString(0,2,(unsigned char *)"IP:");				// 从(0,2)开始显示		// 因为【Project=IIC_OLED】在纵向上占用了【2】页(2*8个点)
//	OLED_ShowIP(24, 2, IP_Address);			// 从(24,2)开始显示		// 因为，【IP:】一共3个字符，横向上占用【24】个点
//	OLED_ShowString(0,4,(unsigned char *)"0123456789ABCDEFGHIJKLMN");	// 从(0,4)开始显示
														// 因为【Project=IIC_OLED】【IP:192.168.4.1】在纵向上占用了【4】页(4*8个点)
    while(1)
    {
	dht11_working();
	OLED_ShowString(0,2,get_data_string(1));
	OLED_ShowString(0,4,get_data_string(0));
	val = pcf8591_adc_read();
	dat[0] = val/1000;
	dat[1] = val%1000/100;
	dat[2] = val%100/10;
	OLED_ShowString(0,6,dat);
    	vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    //start i2c task
    gpio_init();
    xTaskCreate(oled_task_example, "oled_task_example", 2048, NULL, 10, NULL);
}
