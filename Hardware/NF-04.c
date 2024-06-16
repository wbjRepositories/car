#include "stm32f10x.h"                  // Device header
#include "si24r1.h"
#include "OLED.h"
#include "motor.h"

uint8_t SI24R1_buff[32] = {128,128,128,128};

void NF04_Init(void)
{
	SI24R1_Init();
	SI24R1_InitTypeDef SI24R1_InitStruct;
    SI24R1_InitStruct.ADR_WIDTH = 0x03;			// 5位接收地址
    SI24R1_InitStruct.RX_ADDR_Px = RX_ADDR_P0;	// 接收通道0
    SI24R1_InitStruct.EN_RXADDR_Px = 0x01;		// 使能接收通道0
    SI24R1_InitStruct.RF_CHANNEL = 40;			// 射频频道选择2440MHz
    SI24R1_InitStruct.RX_PW_Px = RX_PW_P0;		// 选择设置接收通道0负载数据宽度
    SI24R1_InitStruct.RF_SET = 0x0f;			// 传输速率功率设置，2Mbps 7dbm
    SI24R1_InitStruct.RF_FEATURE = 0x01;		// 使能NOACK
    SI24R1_InitStruct.SI24R1_CONFIG = 0x0f;		// CRC使能，16位CRC校验，上电，接收模式

	SI24R1_Mode_NOACK(&SI24R1_InitStruct,RX_MODE);
	//中断引脚选择
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource11);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

void NF04_Receive(void)
{
	SI24R1_RxPacket(SI24R1_buff);
}

void EXTI15_10_IRQHandler(void)
{
//	OLED_ShowNum(1,1,SI24R1_buff[0],3);
//	OLED_ShowNum(2,1,SI24R1_buff[1],3);	左大
//	OLED_ShowNum(3,1,SI24R1_buff[2],3); 前进小
//	OLED_ShowNum(4,1,SI24R1_buff[3],3);	
	int16_t speed = 0;
	NF04_Receive();
	speed = 2000-1.0*SI24R1_buff[2]/255*4000;
	//前进或后退
	if(SI24R1_buff[2]>135||SI24R1_buff[2]<120)
	{
		
		Motor_SetSpeedMotor1(speed);
		Motor_SetSpeedMotor2(speed);
	}
	else
	{
		Motor_SetSpeedMotor1(0);
		Motor_SetSpeedMotor2(0);
	}
	//直行时左转或右转
	if(SI24R1_buff[1]>135||SI24R1_buff[1]<120)
	{
		Motor_SetSpeedMotor1(speed+((1.0*SI24R1_buff[1]-128)/128*speed));	//右轮
		Motor_SetSpeedMotor2(speed-((1.0*SI24R1_buff[1]-128)/128*speed)); 	//左轮
	}
	
	//原地左转或右转
	if(SI24R1_buff[3]<120 || SI24R1_buff[3] > 135)
	{
		Motor_SetSpeedMotor1(-1.0*(128-SI24R1_buff[3])/128*1000);
		Motor_SetSpeedMotor2(1.0*(128-SI24R1_buff[3])/128*1000);
	}
	EXTI_ClearITPendingBit(EXTI_Line11);
}


