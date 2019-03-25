#include "pcf8591.h"

void pcf8591_delay_ms(int C_time)
{       for(;C_time>0;C_time--)
                os_delay_us(1000);
}

void iic_pcf8591_init(void)
{
	i2c_adc_master_gpio_init();		// ��ʼ��SCL(IO14)��SDA(IO2)
}
uint16_t pcf8591_adc_read(void)
{
	int val = 0;
	i2c_adc_master_start();					// ������ʼ�ź�
	
	i2c_adc_master_writeByte(0x90);			// ѡ�������ϵ�OLED[0111 100X B]
	if(i2c_adc_master_checkAck()==0)
	{i2c_adc_master_stop(); return 0;}
	i2c_adc_master_writeByte(0x41);			// [0x00]��ʾ��һ�ֽ�д�����[ָ��]
	if(i2c_adc_master_checkAck()==0)
	{i2c_adc_master_stop(); return 0;}
	i2c_adc_master_stop();
	i2c_adc_master_start();
	i2c_adc_master_writeByte(0x91);                 // ѡ�������ϵ�OLED[0111 100X B]
        if(i2c_adc_master_checkAck()==0)
        {i2c_adc_master_stop(); return 0;}
	val = i2c_adc_master_readByte();	// [ָ�����]
	i2c_adc_master_send_nack();
	i2c_adc_master_stop();					// ����ֹͣ�ź�
	return val*10;						// ����д��ɹ�
}
