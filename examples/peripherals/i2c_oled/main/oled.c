// ͷ�ļ�����
//==============================================================================
#include "oled.h"
#include "oledfont.h"	// �ַ���
//==============================================================================

// OLED��غ���
//==============================================================================
// IIC��ʼ��( SCL==IO14��SDA==IO2 )
//-----------------------------------------------------------
void IIC_Init_JX(void)
{
	i2c_oled_master_gpio_init();		// ��ʼ��SCL(IO14)��SDA(IO2)
}
//-----------------------------------------------------------

// ��OLEDд��ָ���ֽ�
//----------------------------------------------------------------------------
char OLED_Write_Command(unsigned char OLED_Byte)
{
	i2c_oled_master_start();					// ������ʼ�ź�

	i2c_oled_master_writeByte(0x78);			// ѡ�������ϵ�OLED[0111 100X B]
	if(i2c_oled_master_checkAck()==0)
	{i2c_oled_master_stop(); return 0;}

	i2c_oled_master_writeByte(0x00);			// [0x00]��ʾ��һ�ֽ�д�����[ָ��]
	if(i2c_oled_master_checkAck()==0)
	{i2c_oled_master_stop(); return 0;}

	i2c_oled_master_writeByte(OLED_Byte);	// [ָ�����]
	if(i2c_oled_master_checkAck()==0)
	{i2c_oled_master_stop(); return 0;}

	i2c_oled_master_stop();					// ����ֹͣ�ź�

	return 1;						// ����д��ɹ�
}
//----------------------------------------------------------------------------

// ��OLEDд�������ֽ�
//----------------------------------------------------------------------------
char OLED_Write_Data(unsigned char OLED_Byte)
{
	i2c_oled_master_start();					// ������ʼ�ź�

	i2c_oled_master_writeByte(0x78);			// ѡ�������ϵ�OLED[0111 100X B]
	if(i2c_oled_master_checkAck()==0)
	{i2c_oled_master_stop(); return 0;}

	i2c_oled_master_writeByte(0x40);			// [0x40]��ʾ��һ�ֽ�д�����[����]
	if(i2c_oled_master_checkAck()==0)
	{i2c_oled_master_stop(); return 0;}

	i2c_oled_master_writeByte(OLED_Byte);	// [��������]
	if(i2c_oled_master_checkAck()==0)
	{i2c_oled_master_stop(); return 0;}

	i2c_oled_master_stop();					// ����ֹͣ�ź�

	return 1;						// ����д��ɹ�
}
//----------------------------------------------------------------------------

// ��OLEDд��һ�ֽ�����/ָ��
//---------------------------------------------------------------
void OLED_WR_Byte(unsigned char OLED_Byte, unsigned char OLED_Type)
{
	if(OLED_Type)
		OLED_Write_Data(OLED_Byte);		// д������
	else
		OLED_Write_Command(OLED_Byte);	// д��ָ��
}
//---------------------------------------------------------------

// ����д��ĳֵ
//------------------------------------------------------------------------
void OLED_Clear(void)
{
	unsigned char N_Page, N_row;

	for(N_Page=0; N_Page<8; N_Page++)
	{
		OLED_WR_Byte(0xb0+N_Page,OLED_CMD);	// ��0��7ҳ����д��
		OLED_WR_Byte(0x00,OLED_CMD);      	// �е͵�ַ
		OLED_WR_Byte(0x10,OLED_CMD);      	// �иߵ�ַ

		for(N_row=0; N_row<128; N_row++)OLED_WR_Byte(0x00,OLED_DATA);
	}
}
//------------------------------------------------------------------------

// ��������д�����ʼ�С���
//------------------------------------------------------------------------
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
	OLED_WR_Byte(0xb0+y,OLED_CMD);				// д��ҳ��ַ

	OLED_WR_Byte((x&0x0f),OLED_CMD);  			// д���еĵ�ַ(�Ͱ��ֽ�)

	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);	// д���еĵ�ַ(�߰��ֽ�)
}
//------------------------------------------------------------------------

// ��ʼ��OLED
//-----------------------------------------------------------------------------
void OLED_Init(void)
{
	IIC_Init_JX();					// ��ʼ��IIC

	delay_ms(100);					// �ӳ�(����Ҫ��)

	OLED_WR_Byte(0xAE,OLED_CMD);	// �ر���ʾ

	OLED_WR_Byte(0x00,OLED_CMD);	// ���õ��е�ַ
	OLED_WR_Byte(0x10,OLED_CMD);	// ���ø��е�ַ
	OLED_WR_Byte(0x40,OLED_CMD);	// ������ʼ�е�ַ
	OLED_WR_Byte(0xB0,OLED_CMD);	// ����ҳ��ַ

	OLED_WR_Byte(0x81,OLED_CMD); 	// �Աȶ����ã�����������
	OLED_WR_Byte(0xFF,OLED_CMD);	// 265

	OLED_WR_Byte(0xA1,OLED_CMD);	// ���ö�(SEG)����ʼӳ���ַ
	OLED_WR_Byte(0xA6,OLED_CMD);	// ������ʾ��0xa7����ʾ

	OLED_WR_Byte(0xA8,OLED_CMD);	// ��������·����16~64��
	OLED_WR_Byte(0x3F,OLED_CMD);	// 64duty

	OLED_WR_Byte(0xC8,OLED_CMD);	// ��ӳ��ģʽ��COM[N-1]~COM0ɨ��

	OLED_WR_Byte(0xD3,OLED_CMD);	// ������ʾƫ��
	OLED_WR_Byte(0x00,OLED_CMD);	// ��ƫ��

	OLED_WR_Byte(0xD5,OLED_CMD);	// ����������Ƶ
	OLED_WR_Byte(0x80,OLED_CMD);	// ʹ��Ĭ��ֵ

	OLED_WR_Byte(0xD9,OLED_CMD);	// ���� Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);	// ʹ�ùٷ��Ƽ�ֵ

	OLED_WR_Byte(0xDA,OLED_CMD);	// ���� com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);	// ʹ��Ĭ��ֵ

	OLED_WR_Byte(0xDB,OLED_CMD);	// ���� Vcomh���ɵ������ȣ�Ĭ�ϣ�
	OLED_WR_Byte(0x40,OLED_CMD);	// ʹ�ùٷ��Ƽ�ֵ

	OLED_WR_Byte(0x8D,OLED_CMD);	// ����OLED��ɱ�
	OLED_WR_Byte(0x14,OLED_CMD);	// ����ʾ

	OLED_WR_Byte(0xAF,OLED_CMD);	// ����OLED�����ʾ

	OLED_Clear();        			// ����

	OLED_Set_Pos(0,0); 				// ��������д�����ʼ�С���
}

// ��ָ�����괦��ʾһ���ַ�
//-----------------------------------------------------------------------------
void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char Show_char)
{
	unsigned char c=0,i=0;

	c=Show_char-' '; 				// ��ȡ�ַ���ƫ����

	if(x>Max_Column-1){x=0;y=y+2;}	// ������������Χ��������2ҳ

	if(SIZE ==16) 					// �ַ���СΪ[8*16]��һ���ַ�����ҳ
	{
		// ����һҳ
		//-------------------------------------------------------
		OLED_Set_Pos(x,y);						// ���û�����ʼ��
		for(i=0;i<8;i++)  						// ѭ��8��(8��)
		OLED_WR_Byte(F8X16[c*16+i],OLED_DATA); 	// �ҵ���ģ

		// ���ڶ�ҳ
		//-------------------------------------------------------
		OLED_Set_Pos(x,y+1); 					// ҳ����1
		for(i=0;i<8;i++)  						// ѭ��8��
		OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);// �ѵڶ�ҳ����
	}
}
//-----------------------------------------------------------------------------

// ��ָ��������ʼ����ʾ�ַ���
//-------------------------------------------------------------------
void OLED_ShowString(unsigned char x, unsigned char y, unsigned char * Show_char)
{
	unsigned char N_Char = 0;		// �ַ����

	while (Show_char[N_Char]!='\0') 	// ����������һ���ַ�
	{
		OLED_ShowChar(x,y,Show_char[N_Char]); 	// ��ʾһ���ַ�

		x += 8;					// ������8��һ���ַ�ռ8��

		if(x>=128){x=0;y+=2;} 	// ��x>=128������һҳ

		N_Char++; 				// ָ����һ���ַ�
	}
}
//-------------------------------------------------------------------


// ��ָ��λ����ʾIP��ַ(���ʮ����)
//-----------------------------------------------------------------------------
void OLED_ShowIP(unsigned char x, unsigned char y, unsigned char*Array_IP)
{
	unsigned char N_IP_Byte = 0;		// IP�ֽ����

	// ѭ����ʾ4��IP�ֽ�(�ɸߵ����ֽ���ʾ)
	//----------------------------------------------------------
	for(; N_IP_Byte<4; N_IP_Byte++)
	{
		// ��ʾ��λ/ʮλ
		//------------------------------------------------------
		if(Array_IP[N_IP_Byte]/100)		// �жϰ�λ?=0
		{
			OLED_ShowChar(x,y,48+Array_IP[N_IP_Byte]/100); x+=8;

			// ��ʾʮλ����λ!=0��
			//---------------------------------------------------------
			//if(Array_IP[N_IP_Byte]%100/10)
			{ OLED_ShowChar(x,y,48+Array_IP[N_IP_Byte]%100/10); x+=8; }

		}

		// ��ʾʮλ����λ==0��
		//---------------------------------------------------------
		else if(Array_IP[N_IP_Byte]%100/10)		// �ж�ʮλ?=0
		{ OLED_ShowChar(x,y,48+Array_IP[N_IP_Byte]%100/10); x+=8; }


		// ��ʾ��λ
		//---------------------------------------------------------
		//if(Array_IP[C_IP_Byte]%100%10)
		{ OLED_ShowChar(x,y,48+Array_IP[N_IP_Byte]%100%10); x+=8; }

		// ��ʾ��.��
		if(N_IP_Byte<3)
		{ OLED_ShowChar(x,y,'.'); x+=8; }
	}
}
//-----------------------------------------------------------------------------
uint8_t i2c0_pcf8591_adc_read(uint8_t ch)
{
        int val = 0;
        i2c_oled_master_start();                                 // ������ʼ�ź�
        i2c_oled_master_writeByte(0x90);                 // ѡ�������ϵ�OLED[0111 100X B]
        if(i2c_oled_master_checkAck()==0)
        {i2c_oled_master_stop(); return 0;}
        i2c_oled_master_writeByte(0x40+ch);                 // [0x00]��ʾ��һ�ֽ�д�����[ָ��]
        if(i2c_oled_master_checkAck()==0)
        {i2c_oled_master_stop(); return 0;}
        i2c_oled_master_stop();
        i2c_oled_master_start();
        i2c_oled_master_writeByte(0x91);                 // ѡ�������ϵ�OLED[0111 100X B]
        if(i2c_oled_master_checkAck()==0)
        {i2c_oled_master_stop(); return 0;}
        val = i2c_oled_master_readByte();        // [ָ�����]
        i2c_oled_master_send_nack();
        i2c_oled_master_stop();                                  // ����ֹͣ�ź�
        return val;                                             // ����д��ɹ�
}
//==============================================================================
