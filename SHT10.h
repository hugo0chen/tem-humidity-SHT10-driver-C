#ifndef __SHT10_H__
#define __SHT10_H__
#include "stm32l1xx.h"



enum {TEMP, HUMI};


enum SHT10_NO{
	SHT10_NO_0 = 0,
	SHT10_NO_1 = 1,
	SHT10_NO_2 = 2,
	SHT10_NO_3 = 3
};
#define SHT10_MAX_SIZE (4)

struct SHT10Define{
	enum SHT10_NO sht10_no;
	GPIO_TypeDef* pin_of_data_group;
	uint16_t pin_of_data;
	GPIO_TypeDef* pin_of_sck_group;
	uint16_t pin_of_sck;
};

void init_sht10(void);
void SHT10_Config(struct SHT10Define* defines, uint8_t len);

/*
	@brief 重置通讯
*/
void SHT10_ConReset(enum SHT10_NO sht10_no);

/*
	@brief 软复位
*/
uint8_t SHT10_SoftReset(enum SHT10_NO sht10_no);


uint8_t SHT10_Measure(enum SHT10_NO sht10_no, uint16_t *p_value, uint8_t *p_checksum, uint8_t mode);
void SHT10_Calculate(enum SHT10_NO sht10_no, uint16_t t, uint16_t rh,float *p_temperature, float *p_humidity);
float SHT10_CalcuDewPoint(enum SHT10_NO sht10_no, float t, float h);


#endif

