/* gpio example

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

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_system.h"

static const char *TAG = "dht11";

/**
 * Brief:
 * This test code shows how to configure gpio and how to use gpio.
 *
 * GPIO status:
 * GPIO5: output/input.
 * GPIO4: output.
 * GPIO4: output, pulled up.
 * GPIO5: output/input, pulled up.
 *
 * Test:
 * Connect GPIO5 with DHT11
 * Connect GPIO4 with LED
 * Generate pulses on GPIO4/5.
 *
 */

#define GPIO_OUTPUT_IO_0     5
#define GPIO_OUTPUT_IO_1     4
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))

uint8_t dht11_data_array[6] = {0};	// DHT11数据数组
uint8_t dht11_data_char[2][10]={0};	// DHT11数据字符串【行：湿/温度】【列：数据字符串】

void dht11_delay_ms(uint32_t C_time)
{	
	for(;C_time>0;C_time--)
		os_delay_us(1000);
}

uint8_t dht11_start_signal(void)
{
	uint8_t delay_time = 0;	// 延时计时

	// IO5抬高
	//--------------------------------------------------------
	gpio_set_level(GPIO_OUTPUT_IO_0,1);	// DHT11信号线(IO5) == 输出高
	dht11_delay_ms(1);

	// IO5拉低(25ms)：起始信号
	//---------------------------------------------
	gpio_set_level(GPIO_OUTPUT_IO_0,0);	// IO5 = 0
	dht11_delay_ms(25);

	// IO5抬高【注：起始信号结束后的约13us，DHT11开始输出信号】
	//---------------------------------------------------------
	gpio_set_level(GPIO_OUTPUT_IO_0,1);	// IO5 = 1
	os_delay_us(5);		// 延时5us


	// 接收响应信号
	//……………………………………………………………………………………
	// IO5设为输入：接收DHT11数据
	//-------------------------------------------------
	// 等待响应信号的低电平【最迟等50us】
	//-------------------------------------------------------------
	while( gpio_get_level(GPIO_OUTPUT_IO_0)==1 && delay_time<50 )
	{
		os_delay_us(1);		// 1us计时
		delay_time++;
	}

	// 响应信号超时未收到
	//--------------------------------------------------
	if(delay_time >= 50)
		return 1;	// 失败：规定时间内未接收到响应信号


	delay_time = 0 ;		// 低电平计时开始

	// 响应信息的低电平时长计时【最多170us】
	//-------------------------------------------------------------
	while( gpio_get_level(GPIO_OUTPUT_IO_0)==0 && delay_time<170 )
	{
		os_delay_us(1);
		delay_time++;	// 低电平时长
	}

	// 响应信号的低电平时长超时
	//------------------------------------------------
	if(delay_time >= 170)
		return 2;	// 失败：响应信号的低电平时长超时

	// 响应信号的低电平成功接收
	//--------------------------
	else
		return 0;	// 成功
}

uint8_t dht11_read_bit(void)
{
	uint8_t delay_time = 0;	// 延时计时

	// 等待响应信息的低电平【最迟等150us】
	//-------------------------------------------------------------
	while( gpio_get_level(GPIO_OUTPUT_IO_0)==1 && delay_time<150 )
	{
		os_delay_us(1);		// 1us计时
		delay_time++;
	}

	delay_time = 0 ;		// 低电平计时开始

	// 数据位的低电平时长计时【最多200us】
	//-------------------------------------------------------------
	while( gpio_get_level(GPIO_OUTPUT_IO_0)==0 && delay_time<120 )
	{
		os_delay_us(1);
		delay_time++;	// 低电平时长
	}

	// 数据位的低电平结束后，是数据位的高电平
	// 数据"0"的高电平时长 == [23～27us]
	// 数据"1"的高电平时长 == [68～74us]
	//------------------------------------------------
	os_delay_us(45);	// 跳过数据"0"的高电平部分

	// 延时45us后，检测信号线电平
	// 如果此时信号线电平==1 => 数据=="1"
	// 如果此时信号线电平==0 => 数据=="0"
	//-------------------------------------
	return gpio_get_level(GPIO_OUTPUT_IO_0);
}

uint8_t dht11_read_byte(void)
{
	uint8_t C_Bit = 0;	// 位计数

	uint8_t T_dht11_byte_data = 0;	// DHT11字节数据

	for(; C_Bit<8; C_Bit++)		// 读取DHT11一个字节
	{
		T_dht11_byte_data <<= 1;

		T_dht11_byte_data |= dht11_read_bit();	// 一位一位的读取
	}

	return T_dht11_byte_data;	// 返回读取字节
}

uint8_t dht11_read_data_complete(void)
{
	uint8_t delay_time = 0;	// 延时计时

	// 启动DHT11传输_成功
	//------------------------------------------------------------------------
	if(dht11_start_signal() == 0)	// DHT11：输出起始信号－＞接收响应信号
	{
		dht11_data_array[0] = dht11_read_byte();	// 湿度_整数_部分
		dht11_data_array[1] = dht11_read_byte();	// 湿度_小数_部分
		dht11_data_array[2] = dht11_read_byte();	// 温度_整数_部分
		dht11_data_array[3] = dht11_read_byte();	// 温度_小数_部分
		dht11_data_array[4] = dht11_read_byte();	// 校验字节


		// 如果此时是最后一位数据的高电平，则等待它结束
		//-----------------------------------------------------------
		while(gpio_get_level(GPIO_OUTPUT_IO_0)==1 && delay_time<100)
		{
			os_delay_us(1);		// 1us计时
			delay_time++;
		}

		delay_time = 0 ;		// 低电平计时开始


		// 结束信号的低电平时长计时
		//-----------------------------------------------------------
		while(gpio_get_level(GPIO_OUTPUT_IO_0)==0 && delay_time<100)
		{
			os_delay_us(1);		// 1us计时
			delay_time++;
		}

		//-----------------------------------------------------------
		if(delay_time >= 100)
			return 1;		// 返回1，表示：结束信号的低电平时长超时


		// 数据校验
		//-----------------------------------------------
		if(	dht11_data_array[4] ==
			dht11_data_array[0] + dht11_data_array[1] +
			dht11_data_array[2] + dht11_data_array[3] )
		{
			// 判断温度是否为 0℃以上
			//----------------------------------------------
			if((dht11_data_array[3]&0x80) == 0)
			{
				dht11_data_array[5] = 1;		// >=0℃
			}
			else
			{
				dht11_data_array[5] = 0;		// <0℃

				dht11_data_array[3] &= 0x7F;	// 更正温度小数部分
			}


			return 0;	// 返回0，表示：温湿度读取成功
		}

		else return 3;		// 返回3，表示：校验错误
	}

	//-----------------------------------------------------
	else return 2;		// 返回2，表示：启动DHT11传输，失败
}

static void dht11_data_tostring(void)
{
	uint8_t count = 0;		// 字符计数

	// 湿度数据字符串
	//…………………………………………………………………………………………
	if(dht11_data_array[0]/100)			// 湿度整数的百位
		dht11_data_char[0][count++] = dht11_data_array[0]/100 + 48;

	if((dht11_data_array[0]%100)/10)	// 湿度整数的十位
		dht11_data_char[0][count++] = (dht11_data_array[0]%100)/10 + 48;

	// 湿度整数的个位
	//---------------------------------------------------------
	dht11_data_char[0][count++] = dht11_data_array[0]%10 + 48;

	dht11_data_char[0][count++] = '.';		// 小数点

	// 湿度整数的小数
	//---------------------------------------------------------
	dht11_data_char[0][count++] = dht11_data_array[1]%10 + 48;

	dht11_data_char[0][count++] = ' ';		// ' '
	dht11_data_char[0][count++] = '%';		// '%'
	dht11_data_char[0][count++] = 'R';		// 'R'
	dht11_data_char[0][count++] = 'H';		// 'H'
	dht11_data_char[0][count] 	 =  0 ;		// 添0
	//…………………………………………………………………………………………

	count = 0;		// 重置

	// 温度数据字符串
	//…………………………………………………………………………………………
	if(dht11_data_array[5]==0)			// 温度 < 0℃
		dht11_data_char[1][count++] = '-';

	if(dht11_data_array[2]/100)			// 湿度整数的百位
		dht11_data_char[1][count++] = dht11_data_array[2]/100 + 48;

	if((dht11_data_array[2]%100)/10)	// 湿度整数的十位
		dht11_data_char[1][count++] = (dht11_data_array[2]%100)/10 + 48;

	// 湿度整数的个位
	//---------------------------------------------------------
	dht11_data_char[1][count++] = dht11_data_array[2]%10 + 48;

	dht11_data_char[1][count++] = '.';		// 小数点

	// 湿度整数的小数
	//---------------------------------------------------------
	dht11_data_char[1][count++] = dht11_data_array[3]%10 + 48;

	dht11_data_char[1][count++] = ' ';		// ' '
	dht11_data_char[1][count++] = 'C';		// 'C'
	dht11_data_char[1][count]   =  0 ;		// 添0
	//…………………………………………………………………………………………
}

static void dht11_task_example(void *arg)
{
while(1){
	if(dht11_read_data_complete() == 0)		// 读取DHT11温湿度值
	{
		//-------------------------------------------------
		// dht11_data_array[0] == 湿度_整数_部分
		// dht11_data_array[1] == 湿度_小数_部分
		// dht11_data_array[2] == 温度_整数_部分
		// dht11_data_array[3] == 温度_小数_部分
		// dht11_data_array[4] == 校验字节
		// dht11_data_array[5] == 【1:温度>=0】【0:温度<0】
		//-------------------------------------------------


		// 温度超过30℃，LED亮
		//----------------------------------------------------
		if(dht11_data_array[5]==1 && dht11_data_array[2]>=30)
			gpio_set_level(GPIO_OUTPUT_IO_1,0);		// LED亮
		else
			gpio_set_level(GPIO_OUTPUT_IO_1,1);		// LED灭


		// 串口输出温湿度
		//---------------------------------------------------------------------------------
		if(dht11_data_array[5] == 1)			// 温度 >= 0℃
		{
			printf("hum  == %d.%d\n",dht11_data_array[0],dht11_data_array[1]);
			printf("temp == %d.%d\n", dht11_data_array[2],dht11_data_array[3]);
		}
		else // if(dht11_data_array[5] == 0)	// 温度 < 0℃
		{
			printf("hum  == %d.%d\n",dht11_data_array[0],dht11_data_array[1]);
			printf("temp == %d.%d\n",dht11_data_array[2],dht11_data_array[3]);
		}

		// OLED显示温湿度
		//---------------------------------------------------------------------------------
		dht11_data_tostring();	// DHT11数据值转成字符串
	}
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

static void gpio_init(void)
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT_OD;
    //bit mask of the pins that you want to set,e.g.GPIO15/16
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 1;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}

void app_main(void)
{
    gpio_init();
    xTaskCreate(dht11_task_example, "dht11_task_example", 2048, NULL, 10, NULL);
}


