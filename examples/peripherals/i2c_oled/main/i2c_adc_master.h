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

#ifndef __I2C_ADC_MASTER_H__
#define __I2C_ADC_MASTER_H__

#define I2C_MASTER_SDA_GPIO 12
#define I2C_MASTER_SCL_GPIO 16
#define I2C_MASTER_SDA_FUNC FUNC_GPIO12
#define I2C_MASTER_SCL_FUNC FUNC_GPIO16

#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<I2C_MASTER_SDA_GPIO) | (1ULL<<I2C_MASTER_SCL_GPIO))
void i2c_adc_master_gpio_init(void);
void i2c_adc_master_init(void);
void i2c_adc_master_start(void);
void i2c_adc_master_stop(void);
void i2c_adc_master_setAck(unsigned char level);
unsigned char i2c_adc_master_getAck(void);
#define i2c_adc_master_wait    os_delay_us
char i2c_adc_master_checkAck(void);
void i2c_adc_master_send_ack(void);
void i2c_adc_master_send_nack(void);
unsigned char i2c_adc_master_readByte(void);
void i2c_adc_master_writeByte(unsigned char wrdata);
#endif
