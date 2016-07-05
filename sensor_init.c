#include "sensor_init.h"

uint8_t Sensor_Module_Address;

struct Sensor_protype sensor_pkg[14] = 
{
    {"null      "}, 
    {"relay     "},  
    {"sht10     "},  
    {"light     "}, 
    {"infrared  "},  
    {"hall      "},  
    {"alcohol   "},  
    {"pressure  "},  
    {"rfid      "}, 
    {"shock     "},  
    {"rain      "}, 
    {"ultrasonic"},  
    {"accel     "}, 
		{"gas       "},
};
static void Sensor_Check_GPIO_Init(void);

void sensor_init(void)
{
	Sensor_Check_GPIO_Init();	
	init_sht10();
}

/*
 * 函数名：Sensor_Check_GPIO_Init(void)
 * 描述  ：按键值处理
 * 输入  ：无
 * 输出  ：无
 */
void Sensor_Check_GPIO_Init(void) //IO初始化
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

	//init GPIOC.13  上拉输入 SENSOR_EN
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	// Connect Button EXTI Line to  GPIO Pin 13
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource13);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;  
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	// ADD3: ADD2 : ADD1: ADD0
	// ADD3 =PC12 ADD2 =PB9 ADD1 = PC14 ADD0 = PD2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD,ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
  GPIO_Init(GPIOD, &GPIO_InitStructure);	
}
/*
 * 函数名：Sensor_Check_GPIO_Init(void)
 * 描述  ：读取端口的值PC12:PB9:PC14:PD2  对应的是ADD3:ADD2:ADD1:ADD0
				   组成一个字节的低位
 * 输入  ：无
 * 输出  ：无
 */
uint8_t Sensor_Address_Read(void)
{
	uint8_t Read_Address;
	uint8_t i;
	
	Read_Address = 0;
	i = GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12);
	Read_Address=i;
	i = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9);
	Read_Address<<=1 ;
	Read_Address+= i;
	i = GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14);
	Read_Address<<=1 ;
	Read_Address+= i;
	i = GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2);
	Read_Address<<=1 ;
	Read_Address+= i;
	return Read_Address;
}

