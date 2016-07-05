#include "SHT10.h"
#include <math.h>



/*
	全局定义
*/


#define SHT10_GetDef(no) (_sht10_defines[(no)])
#define ARRAY_SIZE(array, type) (sizeof(array)/sizeof(type))
	
struct SHT10Define _sht10_defines[SHT10_MAX_SIZE] = {0};
/* GPIO相关宏定义 */
/*
#define SHT10_DATA_PIN       GPIO_Pin_14
#define SHT10_SCK_PIN        GPIO_Pin_15
#define SHT10_DATA_PORT      GPIOD
#define SHT10_SCK_PORT       GPIOD
*/

#define SHT10_DATA_H(no)       GPIO_SetBits(SHT10_GetDef(no).pin_of_data_group, SHT10_GetDef(no).pin_of_data)                         //拉高DATA数据线
#define SHT10_DATA_L(no)       GPIO_ResetBits(SHT10_GetDef(no).pin_of_data_group, SHT10_GetDef(no).pin_of_data)                         //拉低DATA数据线
#define SHT10_DATA_R(no)       GPIO_ReadInputDataBit(SHT10_GetDef(no).pin_of_data_group, SHT10_GetDef(no).pin_of_data)         //读DATA数据线

#define SHT10_SCK_H(no)        GPIO_SetBits(SHT10_GetDef(no).pin_of_sck_group, SHT10_GetDef(no).pin_of_sck)                                 //拉高SCK时钟线
#define SHT10_SCK_L(no)        GPIO_ResetBits(SHT10_GetDef(no).pin_of_sck_group, SHT10_GetDef(no).pin_of_sck)                         //拉低SCK时钟线

/* 传感器相关宏定义 */
#define					NO_ACK        0
#define 				ACK          1
                                        //addr  command         r/w
#define STATUS_REG_W        0x06        //000         0011          0          写状态寄存器
#define STATUS_REG_R        0x07        //000         0011          1          读状态寄存器
#define MEASURE_TEMP        0x03        //000         0001          1          测量温度
#define MEASURE_HUMI        0x05        //000         0010          1          测量湿度
#define SOFTRESET           0x1E        //000         1111          0          复位



void init_sht10(void){
	/*
	struct SHT10Define{
		enum SHT10_NO sht10_no;
		GPIO_TypeDef* pin_of_data_group;
		uint16_t pin_of_data;
		GPIO_TypeDef* pin_of_sck_group;
		uint16_t pin_of_sck;
	};
	*/
	struct SHT10Define conf[] = {
		{SHT10_NO_1, GPIOB, GPIO_Pin_11, GPIOB, GPIO_Pin_10}
	};
	SHT10_Config(conf, ARRAY_SIZE(conf, struct SHT10Define));
	
	SHT10_ConReset(SHT10_NO_1);
}

/*************************************************************
  Function   ：SHT10_Dly  
  Description：SHT10时序需要的延时
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_Dly(void)
{
	uint16_t i;
	for(i = 800; i > 0; i--);
}


/*************************************************************
  Function   ：SHT10_Config  
  Description：初始化 SHT10引脚
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_Config(struct SHT10Define* defines, uint8_t len)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	uint8_t s=0;
	for(s=0;s<len;++s){
		struct SHT10Define def = defines[s];
		_sht10_defines[def.sht10_no] = def;
		
		//PD0 DATA 推挽输出        
		GPIO_InitStructure.GPIO_Pin = def.pin_of_data;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(def.pin_of_data_group, &GPIO_InitStructure);
		//PD1 SCK 推挽输出
		GPIO_InitStructure.GPIO_Pin = def.pin_of_sck;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;    
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(def.pin_of_sck_group, &GPIO_InitStructure);

		SHT10_ConReset(def.sht10_no);        //复位通讯
	}	
}


/*************************************************************
  Function   ：SHT10_DATAOut
  Description：设置DATA引脚为输出
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_DATAOut(enum SHT10_NO sht10_no)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//PD0 DATA 推挽输出        
	GPIO_InitStructure.GPIO_Pin = SHT10_GetDef(sht10_no).pin_of_data;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		
	GPIO_Init(SHT10_GetDef(sht10_no).pin_of_data_group, &GPIO_InitStructure);
}


/*************************************************************
  Function   ：SHT10_DATAIn  
  Description：设置DATA引脚为输入
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_DATAIn(enum SHT10_NO sht10_no)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//PD0 DATA 浮动输入        
	GPIO_InitStructure.GPIO_Pin = SHT10_GetDef(sht10_no).pin_of_data;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(SHT10_GetDef(sht10_no).pin_of_data_group, &GPIO_InitStructure);
}


/*************************************************************
  Function   ：SHT10_WriteByte  
  Description：写1字节
  Input      : value:要写入的字节        
  return     : err: 0-正确  1-错误    
*************************************************************/
uint8_t SHT10_WriteByte(enum SHT10_NO sht10_no, uint8_t value)
{
	uint8_t i, err = 0;

	SHT10_DATAOut(sht10_no);                          //设置DATA数据线为输出

	for(i = 0x80; i > 0; i /= 2)  //写1个字节
	{
		if(i & value){
			SHT10_DATA_H(sht10_no);
		}else{
			SHT10_DATA_L(sht10_no);
		}
		SHT10_Dly();
		SHT10_SCK_H(sht10_no);
		SHT10_Dly();
		SHT10_SCK_L(sht10_no);
		SHT10_Dly();
	}
	SHT10_DATAIn(sht10_no);                                  //设置DATA数据线为输入,释放DATA线
	SHT10_SCK_H(sht10_no);
	err = SHT10_DATA_R(sht10_no);                  //读取SHT10的应答位
	SHT10_SCK_L(sht10_no);

	return err;
}

/*************************************************************
  Function   ：SHT10_ReadByte  
  Description：读1字节数据
  Input      : Ack: 0-不应答  1-应答        
  return     : err: 0-正确 1-错误    
*************************************************************/
uint8_t SHT10_ReadByte(enum SHT10_NO sht10_no, uint8_t Ack)
{
	uint8_t i, val = 0;

	SHT10_DATAIn(sht10_no);                                  //设置DATA数据线为输入
	for(i = 0x80; i > 0; i /= 2)  //读取1字节的数据
	{
		SHT10_Dly();
		SHT10_SCK_H(sht10_no);
		SHT10_Dly();
		if(SHT10_DATA_R(sht10_no)){
					val = (val | i);
		}
		SHT10_SCK_L(sht10_no);
	}
	SHT10_DATAOut(sht10_no);                          //设置DATA数据线为输出
	if(Ack){
		SHT10_DATA_L(sht10_no);                          //应答，则会接下去读接下去的数据(校验数据)
	}else{
		SHT10_DATA_H(sht10_no);                          //不应答，数据至此结束
	}
	SHT10_Dly();
	SHT10_SCK_H(sht10_no);
	SHT10_Dly();
	SHT10_SCK_L(sht10_no);
	SHT10_Dly();

	return val;                                          //返回读到的值
}


/*************************************************************
  Function   ：SHT10_TransStart  
  Description：开始传输信号，时序如下：
                     _____         ________
               DATA:      |_______|
                         ___     ___
               SCK : ___|   |___|   |______        
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_TransStart(enum SHT10_NO sht10_no)
{
	SHT10_DATAOut(sht10_no);                          //设置DATA数据线为输出

	SHT10_DATA_H(sht10_no);
	SHT10_SCK_L(sht10_no);
	SHT10_Dly();
	SHT10_SCK_H(sht10_no);
	SHT10_Dly();
	SHT10_DATA_L(sht10_no);
	SHT10_Dly();
	SHT10_SCK_L(sht10_no);
	SHT10_Dly();
	SHT10_SCK_H(sht10_no);
	SHT10_Dly();
	SHT10_DATA_H(sht10_no);
	SHT10_Dly();
	SHT10_SCK_L(sht10_no);

}


/*************************************************************
  Function   ：SHT10_ConReset  
  Description：通讯复位，时序如下：
                     _____________________________________________________         ________
               DATA:                                                      |_______|
                        _    _    _    _    _    _    _    _    _        ___     ___
               SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_ConReset(enum SHT10_NO sht10_no)
{
	uint8_t i;

	SHT10_DATAOut(sht10_no);

	SHT10_DATA_H(sht10_no);
	SHT10_SCK_L(sht10_no);

	for(i = 0; i < 9; i++)                  //触发SCK时钟9c次
	{
		SHT10_SCK_H(sht10_no);
		SHT10_Dly();
		SHT10_SCK_L(sht10_no);
		SHT10_Dly();
	}
	SHT10_TransStart(sht10_no);                          //启动传输
}



/*************************************************************
  Function   ：SHT10_SoftReset  
  Description：软复位
  Input      : none        
  return     : err: 0-正确  1-错误    
*************************************************************/
uint8_t SHT10_SoftReset(enum SHT10_NO sht10_no)
{
	uint8_t err = 0;
	SHT10_ConReset(sht10_no);	//通讯复位
	err += SHT10_WriteByte(sht10_no, SOFTRESET);	//写RESET复位命令
	return err;
}


/*************************************************************
  Function   ：SHT10_ReadStatusReg  
  Description：读状态寄存器
  Input      : p_value-读到的数据；p_checksun-读到的校验数据       
  return     : err: 0-正确  0-错误    
*************************************************************/
uint8_t SHT10_ReadStatusReg(enum SHT10_NO sht10_no, uint8_t *p_value, uint8_t *p_checksum)
{
	uint8_t err = 0;

	SHT10_TransStart(sht10_no);                                        //开始传输
	err = SHT10_WriteByte(sht10_no, STATUS_REG_R);//写STATUS_REG_R读取状态寄存器命令
	*p_value = SHT10_ReadByte(sht10_no, ACK);                //读取状态数据
	*p_checksum = SHT10_ReadByte(sht10_no, NO_ACK);//读取检验和数据

	return err;
}



/*************************************************************
  Function   ：SHT10_WriteStatusReg  
  Description：写状态寄存器
  Input      : p_value-要写入的数据值       
  return     : err: 0-正确  1-错误    
*************************************************************/
uint8_t SHT10_WriteStatusReg(enum SHT10_NO sht10_no, uint8_t *p_value)
{
	uint8_t err = 0;

	SHT10_TransStart(sht10_no);                                         //开始传输
	err += SHT10_WriteByte(sht10_no, STATUS_REG_W);//写STATUS_REG_W写状态寄存器命令
	err += SHT10_WriteByte(sht10_no, *p_value);         //写入配置值

	return err;
}



/*************************************************************
  Function   ：SHT10_Measure  
  Description：从温湿度传感器读取温湿度
  Input      : p_value-读到的值；p_checksum-读到的校验数        
  return     : err: 0-正确 1—错误    
*************************************************************/
uint8_t SHT10_Measure(enum SHT10_NO sht10_no, uint16_t *p_value, uint8_t *p_checksum, uint8_t mode)
{
	uint8_t err = 0;
	uint32_t i;
	uint8_t value_H = 0;
	uint8_t value_L = 0;

	SHT10_TransStart(sht10_no);                                                 //开始传输
	switch(mode)                                                         
	{
		case TEMP:{                                                                 //测量温度
			err += SHT10_WriteByte(sht10_no, MEASURE_TEMP);//写MEASURE_TEMP测量温度命令
			break;
		}
		case HUMI:{
			err += SHT10_WriteByte(sht10_no, MEASURE_HUMI);//写MEASURE_HUMI测量湿度命令
			break;
		}
		default:
			break;
	}
	SHT10_DATAIn(sht10_no);
	for(i = 0; i < 72000000; i++)                             //等待DATA信号被拉低
	{
		if(SHT10_DATA_R(sht10_no) == 0) break;             //检测到DATA被拉低了，跳出循环
	}
	if(SHT10_DATA_R(sht10_no) == 1)                                //如果等待超时了 TODO 代码逻辑需要确认
		err += 1;
	value_H = SHT10_ReadByte(sht10_no, ACK);
	value_L = SHT10_ReadByte(sht10_no, ACK);
	*p_checksum = SHT10_ReadByte(sht10_no, NO_ACK);           //读取校验数据
	*p_value = (value_H << 8) | value_L;
	return err;
}


/*************************************************************
  Function   ：SHT10_Calculate  
  Description：计算温湿度的值
  Input      : Temp-从传感器读出的温度值；Humi-从传感器读出的湿度值
               p_humidity-计算出的实际的湿度值；p_temperature-计算出的实际温度值        
  return     : none    
*************************************************************/
void SHT10_Calculate(enum SHT10_NO sht10_no, uint16_t t, uint16_t rh, float *p_temperature, float *p_humidity)
{
	const float d1 = -39.7;
	const float d2 = +0.01;
	const float C1 = -2.0468;
	const float C2 = +0.0367;
	const float C3 = -0.0000015955;        
	const float T1 = +0.01;
	const float T2 = +0.00008;

	float RH_Lin;                                                                                  //RH线性值        
	float RH_Ture;                                                                                 //RH真实值
	float temp_C;

	temp_C = d1 + d2 * t;                                                              //计算温度值        
	RH_Lin = C1 + C2 * rh + C3 * rh * rh;                            //计算湿度值
	RH_Ture = (temp_C -25) * (T1 + T2 * rh) + RH_Lin;        //湿度的温度补偿，计算实际的湿度值

	if(RH_Ture > 100) {       //设置湿度值上限                                                                
					RH_Ture        = 100;
	}
	if(RH_Ture < 0.1){ //设置湿度值下限
					RH_Ture = 0.1;  
	}		

	*p_humidity = RH_Ture;
	*p_temperature = temp_C;

}


/*************************************************************
  Function   ：SHT10_CalcuDewPoint  
  Description：计算露点
  Input      : h-实际的湿度；t-实际的温度        
  return     : dew_point-露点    
*************************************************************/
float SHT10_CalcuDewPoint(enum SHT10_NO sht10_no, float t, float h)
{
	float logEx, dew_point;
	logEx = 0.66077 + 7.5 * t / (237.3 + t) + (log10(h) - 2);
	dew_point = ((0.66077 - logEx) * 237.3) / (logEx - 8.16077);
	return dew_point; 
}



