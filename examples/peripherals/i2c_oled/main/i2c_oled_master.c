/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
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
#include "i2c_oled_master.h"

unsigned char m_nLastSDA;
unsigned char m_nLastSCL;

void i2c_oled_master_gpio_init(void);
void i2c_oled_master_init(void);
void i2c_oled_master_stop(void);
void i2c_oled_master_start(void);
void i2c_oled_master_setAck(unsigned char level);
unsigned char i2c_oled_master_getAck(void);
unsigned char i2c_oled_master_readByte(void);
void i2c_oled_master_writeByte(unsigned char wrdata);
char i2c_oled_master_checkAck(void);
void i2c_oled_master_send_ack(void);
void i2c_oled_master_send_nack(void);

/******************************************************************************
 * FunctionName : i2c_oled_master_setDC
 * Description  : Internal used function -
 *                    set i2c SDA and SCL bit value for half clk cycle
 * Parameters   : unsigned char SDA
 *                unsigned char SCL
 * Returns      : NONE
*******************************************************************************/
void i2c_oled_master_setDC(unsigned char SDA, unsigned char SCL)
{
    SDA	&= 0x01;
    SCL	&= 0x01;
    m_nLastSDA = SDA;
    m_nLastSCL = SCL;

    if ((0 == SDA) && (0 == SCL)) {
        gpio_set_level(I2C_MASTER_SDA_GPIO,0);
		gpio_set_level(I2C_MASTER_SCL_GPIO,0);
    } else if ((0 == SDA) && (1 == SCL)) {
        gpio_set_level(I2C_MASTER_SDA_GPIO,0);
		gpio_set_level(I2C_MASTER_SCL_GPIO,1);
    } else if ((1 == SDA) && (0 == SCL)) {
        gpio_set_level(I2C_MASTER_SDA_GPIO,1);
		gpio_set_level(I2C_MASTER_SCL_GPIO,0);
    } else {
        gpio_set_level(I2C_MASTER_SDA_GPIO,1);
		gpio_set_level(I2C_MASTER_SCL_GPIO,1);
    }
}

/******************************************************************************
 * FunctionName : i2c_oled_master_getDC
 * Description  : Internal used function -
 *                    get i2c SDA bit value
 * Parameters   : NONE
 * Returns      : unsigned char - SDA bit value
*******************************************************************************/
unsigned char i2c_oled_master_getDC(void)
{
    unsigned char sda_out;
    sda_out = gpio_get_level(I2C_MASTER_SDA_GPIO);
    return sda_out;
}

/******************************************************************************
 * FunctionName : i2c_oled_master_init
 * Description  : initilize I2C bus to enable i2c operations
 * Parameters   : NONE
 * Returns      : NONE
*******************************************************************************/
void i2c_oled_master_init(void)
{
    unsigned char i;

    i2c_oled_master_setDC(1, 0);
    i2c_oled_master_wait(5);

    // when SCL = 0, toggle SDA to clear up
    i2c_oled_master_setDC(0, 0) ;
    i2c_oled_master_wait(5);
    i2c_oled_master_setDC(1, 0) ;
    i2c_oled_master_wait(5);

    // set data_cnt to max value
    for (i = 0; i < 28; i++) {
        i2c_oled_master_setDC(1, 0);
        i2c_oled_master_wait(5);	// sda 1, scl 0
        i2c_oled_master_setDC(1, 1);
        i2c_oled_master_wait(5);	// sda 1, scl 1
    }

    // reset all
    i2c_oled_master_stop();
    return;
}

/******************************************************************************
 * FunctionName : i2c_oled_master_gpio_init
 * Description  : config SDA and SCL gpio to open-drain output mode,
 *                mux and gpio num defined in i2c_oled_master.h
 * Parameters   : NONE
 * Returns      : NONE
*******************************************************************************/
void i2c_oled_master_gpio_init(void)
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
    i2c_oled_master_init();
}

/******************************************************************************
 * FunctionName : i2c_oled_master_start
 * Description  : set i2c to send state
 * Parameters   : NONE
 * Returns      : NONE
*******************************************************************************/
void i2c_oled_master_start(void)
{
    i2c_oled_master_setDC(1, m_nLastSCL);
    i2c_oled_master_wait(5);
    i2c_oled_master_setDC(1, 1);
    i2c_oled_master_wait(5);	// sda 1, scl 1
    i2c_oled_master_setDC(0, 1);
    i2c_oled_master_wait(5);	// sda 0, scl 1
}

/******************************************************************************
 * FunctionName : i2c_oled_master_stop
 * Description  : set i2c to stop sending state
 * Parameters   : NONE
 * Returns      : NONE
*******************************************************************************/
void i2c_oled_master_stop(void)
{
    i2c_oled_master_wait(5);

    i2c_oled_master_setDC(0, m_nLastSCL);
    i2c_oled_master_wait(5);	// sda 0
    i2c_oled_master_setDC(0, 1);
    i2c_oled_master_wait(5);	// sda 0, scl 1
    i2c_oled_master_setDC(1, 1);
    i2c_oled_master_wait(5);	// sda 1, scl 1
}

/******************************************************************************
 * FunctionName : i2c_oled_master_setAck
 * Description  : set ack to i2c bus as level value
 * Parameters   : unsigned char level - 0 or 1
 * Returns      : NONE
*******************************************************************************/
void i2c_oled_master_setAck(unsigned char level)
{
    i2c_oled_master_setDC(m_nLastSDA, 0);
    i2c_oled_master_wait(5);
    i2c_oled_master_setDC(level, 0);
    i2c_oled_master_wait(5);	// sda level, scl 0
    i2c_oled_master_setDC(level, 1);
    i2c_oled_master_wait(8);	// sda level, scl 1
    i2c_oled_master_setDC(level, 0);
    i2c_oled_master_wait(5);	// sda level, scl 0
    i2c_oled_master_setDC(1, 0);
    i2c_oled_master_wait(5);
}

/******************************************************************************
 * FunctionName : i2c_oled_master_getAck
 * Description  : confirm if peer send ack
 * Parameters   : NONE
 * Returns      : unsigned char - ack value, 0 or 1
*******************************************************************************/
unsigned char i2c_oled_master_getAck(void)
{
    unsigned char retVal;
    i2c_oled_master_setDC(m_nLastSDA, 0);
    i2c_oled_master_wait(5);
    i2c_oled_master_setDC(1, 0);
    i2c_oled_master_wait(5);
    i2c_oled_master_setDC(1, 1);
    i2c_oled_master_wait(5);

    retVal = i2c_oled_master_getDC();
    i2c_oled_master_wait(5);
    i2c_oled_master_setDC(1, 0);
    i2c_oled_master_wait(5);

    return retVal;
}

/******************************************************************************
* FunctionName : i2c_oled_master_checkAck
* Description  : get dev response
* Parameters   : NONE
* Returns      : 1 : get ack ; 0 : get nack
*******************************************************************************/
char i2c_oled_master_checkAck(void)
{
    if(i2c_oled_master_getAck()){
        return 0;
    }else{
        return 1;
    }
}

/******************************************************************************
* FunctionName : i2c_oled_master_send_ack
* Description  : response ack
* Parameters   : NONE
* Returns      : NONE
*******************************************************************************/
void i2c_oled_master_send_ack(void)
{
    i2c_oled_master_setAck(0x0);
}
/******************************************************************************
* FunctionName : i2c_oled_master_send_nack
* Description  : response nack
* Parameters   : NONE
* Returns      : NONE
*******************************************************************************/
void i2c_oled_master_send_nack(void)
{
    i2c_oled_master_setAck(0x1);
}

/******************************************************************************
 * FunctionName : i2c_oled_master_readByte
 * Description  : read Byte from i2c bus
 * Parameters   : NONE
 * Returns      : unsigned char - readed value
*******************************************************************************/
unsigned char i2c_oled_master_readByte(void)
{
    unsigned char retVal = 0;
    unsigned char k, i;

    i2c_oled_master_wait(5);
    i2c_oled_master_setDC(m_nLastSDA, 0);
    i2c_oled_master_wait(5);	// sda 1, scl 0

    for (i = 0; i < 8; i++) {
        i2c_oled_master_wait(5);
        i2c_oled_master_setDC(1, 0);
        i2c_oled_master_wait(5);	// sda 1, scl 0
        i2c_oled_master_setDC(1, 1);
        i2c_oled_master_wait(5);	// sda 1, scl 1

        k = i2c_oled_master_getDC();
        i2c_oled_master_wait(5);

        if (i == 7) {
            i2c_oled_master_wait(3);   ////
        }

        k <<= (7 - i);
        retVal |= k;
    }

    i2c_oled_master_setDC(1, 0);
    i2c_oled_master_wait(5);	// sda 1, scl 0

    return retVal;
}

/******************************************************************************
 * FunctionName : i2c_oled_master_writeByte
 * Description  : write wrdata value(one byte) into i2c
 * Parameters   : unsigned char wrdata - write value
 * Returns      : NONE
*******************************************************************************/
void i2c_oled_master_writeByte(unsigned char wrdata)
{
    unsigned char dat;
    int i;

    i2c_oled_master_wait(5);

    i2c_oled_master_setDC(m_nLastSDA, 0);
    i2c_oled_master_wait(5);

    for (i = 7; i >= 0; i--) {
        dat = wrdata >> i;
        i2c_oled_master_setDC(dat, 0);
        i2c_oled_master_wait(5);
        i2c_oled_master_setDC(dat, 1);
        i2c_oled_master_wait(5);

        if (i == 0) {
            i2c_oled_master_wait(3);   ////
        }

        i2c_oled_master_setDC(dat, 0);
        i2c_oled_master_wait(5);
    }
}
