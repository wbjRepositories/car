#ifndef si24r1_H
#define si24r1_H

#include "stm32f10x.h"
//#include "bsp_usart.h"

#define SI24R1_debug				1
#define SPIT_FLAG_TIMEOUT			((uint32_t)0x10000)
#define SI24R1_INFO(fmt,arg...)		printf("<<-SI24R1-INFO->> "fmt"\n",##arg)
#define SI24R1_ERROR(fmt,arg...)	printf("<<-SI24R1-ERROR->> "fmt"\n",##arg)
#define SI24R1_DEBUG(fmt,arg...)	do{\
									if(SI24R1_debug)\
									printf("<<-SI24R1-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
									}while(0)

//********************************************************************************************************************//
//                                          FUNCTION's STRUCT                                                         //
//********************************************************************************************************************//
typedef struct
{
	uint8_t ADR_WIDTH;
	uint8_t RX_ADDR_Px;
	uint8_t EN_RXADDR_Px;
	uint8_t RF_CHANNEL;
	uint8_t RX_PW_Px;
	uint8_t RF_SET;
	uint8_t SI24R1_CONFIG;
	uint8_t RF_FEATURE;
}SI24R1_InitTypeDef;

//********************************************************************************************************************//								
//SI24R1 PIN DEFINITION
#define SI24R1_GPIO_RCC RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA
#define SI24R1_SPI_RCC	RCC_APB1Periph_SPI2
#define SI24R1_SPI		SPI2
#define SI24R1_GPIOA 	GPIOA
#define SI24R1_GPIOB 	GPIOB
#define	SI24R1_MOSI		GPIO_Pin_15		// Master Out, Slave In pin (output)
#define	SI24R1_MISO  	GPIO_Pin_14   	// Master In, Slave Out pin (input)
#define	SI24R1_SCK     	GPIO_Pin_13    	// Serial Clock pin, (output)
#define	SI24R1_CSN     	GPIO_Pin_12    	// Slave Select pin, (output to CSN)
#define	SI24R1_CE    	GPIO_Pin_8    	// Chip Enable pin signal (output)
#define	SI24R1_IRQ    	GPIO_Pin_11   	// Interrupt signal, from nRF24L01 (input)

#define SI24R1_SCK_Clr() 	GPIO_ResetBits(SI24R1_GPIOB,SI24R1_SCK)//SCK
#define SI24R1_SCK_Set() 	GPIO_SetBits(SI24R1_GPIOB,SI24R1_SCK)

#define SI24R1_MOSI_Clr() 	GPIO_ResetBits(SI24R1_GPIOB,SI24R1_MOSI)//MOSI
#define SI24R1_MOSI_Set() 	GPIO_SetBits(SI24R1_GPIOB,SI24R1_MOSI)

#define SI24R1_MISO_Clr() 	GPIO_ResetBits(SI24R1_GPIOB,SI24R1_MISO)//MISO
#define SI24R1_MISO_Set() 	GPIO_SetBits(SI24R1_GPIOB,SI24R1_MISO)

#define SI24R1_CSN_Clr()  	GPIO_ResetBits(SI24R1_GPIOB,SI24R1_CSN)//CSN
#define SI24R1_CSN_Set()  	GPIO_SetBits(SI24R1_GPIOB,SI24R1_CSN)

#define SI24R1_CE_Clr()		GPIO_ResetBits(SI24R1_GPIOA,SI24R1_CE)//CE
#define SI24R1_CE_Set()  	GPIO_SetBits(SI24R1_GPIOA,SI24R1_CE)

#define SI24R1_IRQ_Clr()  	GPIO_ResetBits(SI24R1_GPIOA,SI24R1_IRQ)//IRQ
#define SI24R1_IRQ_Set()  	GPIO_SetBits(SI24R1_GPIOA,SI24R1_IRQ)

#define TX_ADR_WIDTH   	5  		// 5字节宽度的发送/接收地址
#define TX_PLOAD_WIDTH 	32  	// 数据通道有效数据宽度
#define RX_MODE 0
#define TX_MODE 1

//********************************************************************************************************************//
// SPI(SI24R1) commands
#define R_REGISTER		0x00  // Define read command to register 000A AAAA
#define W_REGISTER     	0x20  // Define write command to register 001A AAAA
#define RD_RX_PLOAD     0x61  // Define RX payload register address
#define WR_TX_PLOAD     0xA0  // Define TX payload register address
#define WR_TX_PLOAD_NACK     0xB0  // Define TX payload register address
#define FLUSH_TX        0xE1  // Define flush TX register command
#define FLUSH_RX        0xE2  // Define flush RX register command
#define REUSE_TX_PL     0xE3  // Define reuse TX payload register command
#define R_RX_PL_WID		0x60  // Define numbers of bytes of data received commamd
#define NOP             0xFF  // Define No Operation, might be used to read status register

//********************************************************************************************************************//
// SPI(SI24R1) registers(addresses)
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define RSSI            0x09  // 'Received Signal Strength Indecator' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address
#define DYNPD			0x1C  //  使能动态负载长度
#define FEATURE			0x1D  //  特征寄存器

//********************************************************************************************************************//
// STATUS Register
#define RX_DR			0x40  /* RX FIFO有值标志位，写’1’清除。*/
#define TX_DS			0x20  /* 发射端发射完成中断位，如果是ACK模式，
								 则收到ACK确认信号后TX_DS位置’1’，写’1’清除。*/
#define MAX_RT			0x10  /* 达到最大重发次数中断位，写’1’清除。*/

//********************************************************************************************************************//
//                                        FUNCTION's PROTOTYPES                                                       //
//********************************************************************************************************************//
//SI24R1 API Functions
void SI24R1_Init(void); 	//SI24R1 Pin Init
uint8_t SI24R1_Write_Reg(uint8_t reg, uint8_t value);
uint8_t SI24R1_Write_Buf(uint8_t reg, const uint8_t *pBuf, uint8_t bytes);
uint8_t SI24R1_Read_Reg(uint8_t reg);
uint8_t SI24R1_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes);

void SI24R1_Mode_NOACK(SI24R1_InitTypeDef* SI24R1_InitStruct,uint8_t Mode);
uint8_t SI24R1_RxPacket(uint8_t *rxbuf);
uint8_t SI24R1_TxPacket(uint8_t *txbuf);

void SI24R1_Shutdown(void);
void SI24R1_Standby(void);

//********************************************************************************************************************//
#endif /*__bsp_si24r1_H*/

