#include "stm32f10x.h"                  // Device header
#include "Motor.h"
#include "ASPPRO.h"
uint16_t speed = 1000;
int16_t LeftSpeed = 0;
int16_t RightSpeed = 0;
uint8_t BluetoothStatu = 0;	//0停止；1直线前进；2后退；3左转；4右转；

void Bluetooth_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&USART_InitStructure);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	USART_Cmd(USART1,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

void Bluetooth_Send(uint16_t Data)
{
	USART_SendData(USART1,Data);
}	


//W:前进	S:停止
void USART1_IRQHandler(void)
{
	uint16_t command = USART_ReceiveData(USART1);
	
	if(command == 'w')
	{
		BluetoothStatu = 1;
		LeftSpeed = speed;
		RightSpeed = speed;
	}
	else if(command == 'a')
	{
		BluetoothStatu = 3;
		LeftSpeed = speed-speed/4;
		RightSpeed = speed+speed/4;
	}
	else if(command == 'A')
	{
		LeftSpeed = speed;
		RightSpeed = speed;
	}
	else if(command == 's')
	{
		BluetoothStatu = 2;
		LeftSpeed = -speed;
		RightSpeed = -speed;
	}
	else if(command == 'd')
	{
		BluetoothStatu = 4;
		LeftSpeed = speed+speed/4;
		RightSpeed = speed-speed/4;
	}
	else if(command == 'D')
	{
		LeftSpeed = speed;
		RightSpeed = speed;
	}
	else if(command == 'q')
	{
		BluetoothStatu = 0;
		LeftSpeed = 0;
		RightSpeed = 0;
		Motor_SetSpeedMotor1(RightSpeed);
		Motor_SetSpeedMotor2(LeftSpeed);
	}
	else if(command == 'p')
	{
		if(speed<2000)
		{
			speed = speed + 100;
		}
		LeftSpeed = speed;
		RightSpeed = speed;
		Bluetooth_Send(speed);
	}
	else if(command == 'm')
	{
		if(speed>0)
		{
			speed = speed - 100;
		}
		LeftSpeed = speed;
		RightSpeed = speed;
		Bluetooth_Send(speed);
	}
	else if(command == 'e')
	{
		statu = 7;
	}
	else if(command == 'E')
	{
		statu = 0;
	}
	
	
	if(BluetoothStatu != 0)
	{
		Motor_SetSpeedMotor1(RightSpeed);
		Motor_SetSpeedMotor2(LeftSpeed);
	}
	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
}
