#ifndef PCF8591_H_
#define PCF8591_H_

// ͷ�ļ�����
//==============================================================================
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"

#include "i2c_adc_master.h"	// IIC
//=============================================================================

// ��������
//=============================================================================
void iic_pcf8591_init(void);
uint16_t pcf8591_adc_read(void);
//=============================================================================

#endif /* OLED_H_ */
