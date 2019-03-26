#ifndef OLED_H_
#define OLED_H_

// ͷ�ļ�����
//==============================================================================
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"

#include "i2c_oled_master.h"	// IIC
//==============================================================================

// �궨��
//=============================================================================
#define		OLED_CMD  	0		// ����
#define		OLED_DATA 	1		// ����

#define 	SIZE 		16		//��ʾ�ַ��Ĵ�С
#define 	Max_Column	128		//�������
#define		Max_Row		64		//�������
#define		X_WIDTH 	128		//X��Ŀ��
#define		Y_WIDTH 	64	    //Y��Ŀ��
#define		IIC_ACK		0		//Ӧ��
#define		IIC_NO_ACK	1		//��Ӧ��
//=============================================================================

// ��������
//=============================================================================
void delay_ms(int C_time);

void IIC_Init_JX(void);
char OLED_Write_Command(unsigned char OLED_Byte);

char OLED_Write_Data(unsigned char OLED_Byte);

void OLED_WR_Byte(unsigned char OLED_Byte, unsigned char OLED_Type);

void OLED_Clear(void);

void OLED_Set_Pos(unsigned char x, unsigned char y);

void OLED_Init(void);

void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char Show_char);

void OLED_ShowString(unsigned char x, unsigned char y, unsigned char * Show_char);

uint8_t i2c0_pcf8591_adc_read(void);
//=============================================================================

#endif /* OLED_H_ */
