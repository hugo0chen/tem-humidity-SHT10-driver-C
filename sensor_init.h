#ifndef _SENSOR_INIT
#define _SENSOR_INIT

#include "sht10.h"

struct Sensor_protype 
{
	uint8_t sensor_name[16];
	
};
extern uint8_t Sensor_Module_Address; // 传感器的地址存储变量
extern struct Sensor_protype sensor_pkg[14];
void sensor_init(void);
extern uint8_t Sensor_Address_Read(void);

#endif

