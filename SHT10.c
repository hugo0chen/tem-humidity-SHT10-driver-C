#include "SHT10.h"
#include <math.h>



/*
	ȫ�ֶ���
*/


#define SHT10_GetDef(no) (_sht10_defines[(no)])
#define ARRAY_SIZE(array, type) (sizeof(array)/sizeof(type))
	
struct SHT10Define _sht10_defines[SHT10_MAX_SIZE] = {0};
/* GPIO��غ궨�� */
/*
#define SHT10_DATA_PIN       GPIO_Pin_14
#define SHT10_SCK_PIN        GPIO_Pin_15
#define SHT10_DATA_PORT      GPIOD
#define SHT10_SCK_PORT       GPIOD
*/

#define SHT10_DATA_H(no)       GPIO_SetBits(SHT10_GetDef(no).pin_of_data_group, SHT10_GetDef(no).pin_of_data)                         //����DATA������
#define SHT10_DATA_L(no)       GPIO_ResetBits(SHT10_GetDef(no).pin_of_data_group, SHT10_GetDef(no).pin_of_data)                         //����DATA������
#define SHT10_DATA_R(no)       GPIO_ReadInputDataBit(SHT10_GetDef(no).pin_of_data_group, SHT10_GetDef(no).pin_of_data)         //��DATA������

#define SHT10_SCK_H(no)        GPIO_SetBits(SHT10_GetDef(no).pin_of_sck_group, SHT10_GetDef(no).pin_of_sck)                                 //����SCKʱ����
#define SHT10_SCK_L(no)        GPIO_ResetBits(SHT10_GetDef(no).pin_of_sck_group, SHT10_GetDef(no).pin_of_sck)                         //����SCKʱ����

/* ��������غ궨�� */
#define					NO_ACK        0
#define 				ACK          1
                                        //addr  command         r/w
#define STATUS_REG_W        0x06        //000         0011          0          д״̬�Ĵ���
#define STATUS_REG_R        0x07        //000         0011          1          ��״̬�Ĵ���
#define MEASURE_TEMP        0x03        //000         0001          1          �����¶�
#define MEASURE_HUMI        0x05        //000         0010          1          ����ʪ��
#define SOFTRESET           0x1E        //000         1111          0          ��λ



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
  Function   ��SHT10_Dly  
  Description��SHT10ʱ����Ҫ����ʱ
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_Dly(void)
{
	uint16_t i;
	for(i = 800; i > 0; i--);
}


/*************************************************************
  Function   ��SHT10_Config  
  Description����ʼ�� SHT10����
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
		
		//PD0 DATA �������        
		GPIO_InitStructure.GPIO_Pin = def.pin_of_data;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(def.pin_of_data_group, &GPIO_InitStructure);
		//PD1 SCK �������
		GPIO_InitStructure.GPIO_Pin = def.pin_of_sck;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;    
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(def.pin_of_sck_group, &GPIO_InitStructure);

		SHT10_ConReset(def.sht10_no);        //��λͨѶ
	}	
}


/*************************************************************
  Function   ��SHT10_DATAOut
  Description������DATA����Ϊ���
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_DATAOut(enum SHT10_NO sht10_no)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//PD0 DATA �������        
	GPIO_InitStructure.GPIO_Pin = SHT10_GetDef(sht10_no).pin_of_data;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		
	GPIO_Init(SHT10_GetDef(sht10_no).pin_of_data_group, &GPIO_InitStructure);
}


/*************************************************************
  Function   ��SHT10_DATAIn  
  Description������DATA����Ϊ����
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_DATAIn(enum SHT10_NO sht10_no)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//PD0 DATA ��������        
	GPIO_InitStructure.GPIO_Pin = SHT10_GetDef(sht10_no).pin_of_data;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(SHT10_GetDef(sht10_no).pin_of_data_group, &GPIO_InitStructure);
}


/*************************************************************
  Function   ��SHT10_WriteByte  
  Description��д1�ֽ�
  Input      : value:Ҫд����ֽ�        
  return     : err: 0-��ȷ  1-����    
*************************************************************/
uint8_t SHT10_WriteByte(enum SHT10_NO sht10_no, uint8_t value)
{
	uint8_t i, err = 0;

	SHT10_DATAOut(sht10_no);                          //����DATA������Ϊ���

	for(i = 0x80; i > 0; i /= 2)  //д1���ֽ�
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
	SHT10_DATAIn(sht10_no);                                  //����DATA������Ϊ����,�ͷ�DATA��
	SHT10_SCK_H(sht10_no);
	err = SHT10_DATA_R(sht10_no);                  //��ȡSHT10��Ӧ��λ
	SHT10_SCK_L(sht10_no);

	return err;
}

/*************************************************************
  Function   ��SHT10_ReadByte  
  Description����1�ֽ�����
  Input      : Ack: 0-��Ӧ��  1-Ӧ��        
  return     : err: 0-��ȷ 1-����    
*************************************************************/
uint8_t SHT10_ReadByte(enum SHT10_NO sht10_no, uint8_t Ack)
{
	uint8_t i, val = 0;

	SHT10_DATAIn(sht10_no);                                  //����DATA������Ϊ����
	for(i = 0x80; i > 0; i /= 2)  //��ȡ1�ֽڵ�����
	{
		SHT10_Dly();
		SHT10_SCK_H(sht10_no);
		SHT10_Dly();
		if(SHT10_DATA_R(sht10_no)){
					val = (val | i);
		}
		SHT10_SCK_L(sht10_no);
	}
	SHT10_DATAOut(sht10_no);                          //����DATA������Ϊ���
	if(Ack){
		SHT10_DATA_L(sht10_no);                          //Ӧ��������ȥ������ȥ������(У������)
	}else{
		SHT10_DATA_H(sht10_no);                          //��Ӧ���������˽���
	}
	SHT10_Dly();
	SHT10_SCK_H(sht10_no);
	SHT10_Dly();
	SHT10_SCK_L(sht10_no);
	SHT10_Dly();

	return val;                                          //���ض�����ֵ
}


/*************************************************************
  Function   ��SHT10_TransStart  
  Description����ʼ�����źţ�ʱ�����£�
                     _____         ________
               DATA:      |_______|
                         ___     ___
               SCK : ___|   |___|   |______        
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_TransStart(enum SHT10_NO sht10_no)
{
	SHT10_DATAOut(sht10_no);                          //����DATA������Ϊ���

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
  Function   ��SHT10_ConReset  
  Description��ͨѶ��λ��ʱ�����£�
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

	for(i = 0; i < 9; i++)                  //����SCKʱ��9c��
	{
		SHT10_SCK_H(sht10_no);
		SHT10_Dly();
		SHT10_SCK_L(sht10_no);
		SHT10_Dly();
	}
	SHT10_TransStart(sht10_no);                          //��������
}



/*************************************************************
  Function   ��SHT10_SoftReset  
  Description����λ
  Input      : none        
  return     : err: 0-��ȷ  1-����    
*************************************************************/
uint8_t SHT10_SoftReset(enum SHT10_NO sht10_no)
{
	uint8_t err = 0;
	SHT10_ConReset(sht10_no);	//ͨѶ��λ
	err += SHT10_WriteByte(sht10_no, SOFTRESET);	//дRESET��λ����
	return err;
}


/*************************************************************
  Function   ��SHT10_ReadStatusReg  
  Description����״̬�Ĵ���
  Input      : p_value-���������ݣ�p_checksun-������У������       
  return     : err: 0-��ȷ  0-����    
*************************************************************/
uint8_t SHT10_ReadStatusReg(enum SHT10_NO sht10_no, uint8_t *p_value, uint8_t *p_checksum)
{
	uint8_t err = 0;

	SHT10_TransStart(sht10_no);                                        //��ʼ����
	err = SHT10_WriteByte(sht10_no, STATUS_REG_R);//дSTATUS_REG_R��ȡ״̬�Ĵ�������
	*p_value = SHT10_ReadByte(sht10_no, ACK);                //��ȡ״̬����
	*p_checksum = SHT10_ReadByte(sht10_no, NO_ACK);//��ȡ���������

	return err;
}



/*************************************************************
  Function   ��SHT10_WriteStatusReg  
  Description��д״̬�Ĵ���
  Input      : p_value-Ҫд�������ֵ       
  return     : err: 0-��ȷ  1-����    
*************************************************************/
uint8_t SHT10_WriteStatusReg(enum SHT10_NO sht10_no, uint8_t *p_value)
{
	uint8_t err = 0;

	SHT10_TransStart(sht10_no);                                         //��ʼ����
	err += SHT10_WriteByte(sht10_no, STATUS_REG_W);//дSTATUS_REG_Wд״̬�Ĵ�������
	err += SHT10_WriteByte(sht10_no, *p_value);         //д������ֵ

	return err;
}



/*************************************************************
  Function   ��SHT10_Measure  
  Description������ʪ�ȴ�������ȡ��ʪ��
  Input      : p_value-������ֵ��p_checksum-������У����        
  return     : err: 0-��ȷ 1������    
*************************************************************/
uint8_t SHT10_Measure(enum SHT10_NO sht10_no, uint16_t *p_value, uint8_t *p_checksum, uint8_t mode)
{
	uint8_t err = 0;
	uint32_t i;
	uint8_t value_H = 0;
	uint8_t value_L = 0;

	SHT10_TransStart(sht10_no);                                                 //��ʼ����
	switch(mode)                                                         
	{
		case TEMP:{                                                                 //�����¶�
			err += SHT10_WriteByte(sht10_no, MEASURE_TEMP);//дMEASURE_TEMP�����¶�����
			break;
		}
		case HUMI:{
			err += SHT10_WriteByte(sht10_no, MEASURE_HUMI);//дMEASURE_HUMI����ʪ������
			break;
		}
		default:
			break;
	}
	SHT10_DATAIn(sht10_no);
	for(i = 0; i < 72000000; i++)                             //�ȴ�DATA�źű�����
	{
		if(SHT10_DATA_R(sht10_no) == 0) break;             //��⵽DATA�������ˣ�����ѭ��
	}
	if(SHT10_DATA_R(sht10_no) == 1)                                //����ȴ���ʱ�� TODO �����߼���Ҫȷ��
		err += 1;
	value_H = SHT10_ReadByte(sht10_no, ACK);
	value_L = SHT10_ReadByte(sht10_no, ACK);
	*p_checksum = SHT10_ReadByte(sht10_no, NO_ACK);           //��ȡУ������
	*p_value = (value_H << 8) | value_L;
	return err;
}


/*************************************************************
  Function   ��SHT10_Calculate  
  Description��������ʪ�ȵ�ֵ
  Input      : Temp-�Ӵ������������¶�ֵ��Humi-�Ӵ�����������ʪ��ֵ
               p_humidity-�������ʵ�ʵ�ʪ��ֵ��p_temperature-�������ʵ���¶�ֵ        
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

	float RH_Lin;                                                                                  //RH����ֵ        
	float RH_Ture;                                                                                 //RH��ʵֵ
	float temp_C;

	temp_C = d1 + d2 * t;                                                              //�����¶�ֵ        
	RH_Lin = C1 + C2 * rh + C3 * rh * rh;                            //����ʪ��ֵ
	RH_Ture = (temp_C -25) * (T1 + T2 * rh) + RH_Lin;        //ʪ�ȵ��¶Ȳ���������ʵ�ʵ�ʪ��ֵ

	if(RH_Ture > 100) {       //����ʪ��ֵ����                                                                
					RH_Ture        = 100;
	}
	if(RH_Ture < 0.1){ //����ʪ��ֵ����
					RH_Ture = 0.1;  
	}		

	*p_humidity = RH_Ture;
	*p_temperature = temp_C;

}


/*************************************************************
  Function   ��SHT10_CalcuDewPoint  
  Description������¶��
  Input      : h-ʵ�ʵ�ʪ�ȣ�t-ʵ�ʵ��¶�        
  return     : dew_point-¶��    
*************************************************************/
float SHT10_CalcuDewPoint(enum SHT10_NO sht10_no, float t, float h)
{
	float logEx, dew_point;
	logEx = 0.66077 + 7.5 * t / (237.3 + t) + (log10(h) - 2);
	dew_point = ((0.66077 - logEx) * 237.3) / (logEx - 8.16077);
	return dew_point; 
}



