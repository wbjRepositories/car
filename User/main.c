#include "stm32f10x.h"                  // Device header
#include "Servo.h"
#include "nf-04.h"
#include "delay.h"
#include "motor.h"
#include "Bluetooth.h"
#include "OLED.h"
#include "ASPPRO.h"
#include "InfraredSensor.h"
#include "ultrasonicSensor.h"

int main(void)
{
	OLED_Init();
	
	Motor_Init();
	Bluetooth_Init();
	NF04_Init();
	ASPPRO_Init();
	UltrasonicSensor_Init();
	InfraredSensor_Init();
	Servo_Init();
	uint8_t angle = 60;
	int8_t orttion = 1;	//1正向扫描 -1反向扫描
	
	uint16_t distanceArr[8] = {0};
	uint16_t distance = 0;	//超声波测的距离
	uint8_t i = 0;

	while(1)
	{	
		if(statu == 7)
		{
			Motor_SetSpeedMotor1(300);
			Motor_SetSpeedMotor2(300);
		}
		
		while(statu == 7)
		{
			//舵机反复扫描
			angle = angle + orttion*20;
			Servo_Angle(angle);
			if(angle>=220)
			{
				orttion = -1;
			}
			else if(angle<=80)
			{
				orttion = 1;
			}
			
			distance = 0.001*UltrasonicSensor_GetDistance()*343/2;	

			distanceArr[i] = distance;
			i = i + orttion;
			
			
			if(angle == 220 || angle == 80)
			{
				uint16_t tmpLeast = distanceArr[0];
				uint8_t tmpIndex = 0;
				for(int j = 0;j<8;j++)
				{
					if(distanceArr[j]<tmpLeast)
					{
						tmpLeast = distanceArr[j];
						tmpIndex = j;
					}
				}			
				if(tmpLeast<20)
				{
					if(tmpIndex<3)
					{
						Motor_SetSpeedMotor1(400);
						Motor_SetSpeedMotor2(0);
					}
					if(tmpIndex>4)
					{
						Motor_SetSpeedMotor1(0);
						Motor_SetSpeedMotor2(400);
					}
				}
				else
				{
					Motor_SetSpeedMotor1(300);
					Motor_SetSpeedMotor2(300);
				}
				
				if(tmpLeast<5)
				{
					if(distanceArr[3]<distanceArr[2]&&distanceArr[4]<distanceArr[5])
					{
						Motor_SetSpeedMotor1(-400);
						Motor_SetSpeedMotor2(-400);
						Delay_s(1);
						if(distanceArr[2] > distanceArr[5])
						{
							Motor_SetSpeedMotor1(-450);
							Motor_SetSpeedMotor2(450);
						}
						else
						{
							Motor_SetSpeedMotor1(450);
							Motor_SetSpeedMotor2(-450);
						}
						Delay_s(1);
					}
				}
			}
		}
		
		
		OLED_ShowNum(1,1,InfraredSensor_GetCounter1(),4);
		OLED_ShowNum(2,1,InfraredSensor_GetCounter2(),4);
		
		
		//接收无线数据
//		NF04_Receive();
		
//		OLED_ShowNum(1,1,distanceArr[0],4);
//		OLED_ShowNum(2,1,distanceArr[1],4);
//		OLED_ShowNum(3,1,distanceArr[2],4);
//		OLED_ShowNum(4,1,distanceArr[3],4);
//		OLED_ShowNum(1,7,distanceArr[4],4);
//		OLED_ShowNum(2,7,distanceArr[5],4);
//		OLED_ShowNum(3,7,distanceArr[6],4);
//		OLED_ShowNum(4,7,distanceArr[7],4);

		
//		uint16_t data = USART_ReceiveData(USART2);
//		OLED_ShowHexNum(2,1,data,4);
		
		
		
		
		
//		OLED_ShowNum(1,1,SI24R1_buff[0],3);
//		OLED_ShowNum(2,1,SI24R1_buff[1],3);
//		OLED_ShowNum(3,1,SI24R1_buff[2],3);
//		OLED_ShowNum(4,1,SI24R1_buff[3],3);
	

		
//		uint16_t commend = USART_ReceiveData(USART1);
//		OLED_ShowHexNum(2,1,commend,4);
//		char a = commend;
//		OLED_ShowString(3,1,&a);
	}
}



