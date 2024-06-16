#include "si24r1.h"

static __IO uint32_t  SPITimeout = SPIT_FLAG_TIMEOUT;
static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);

uint8_t static TX_ADDRESS[TX_ADR_WIDTH] = {0x0A,0x01,0x07,0x0E,0x01};  // 定义一个静态发送地址

static uint8_t SPI_RW(uint8_t byte)
{
	uint8_t date;
	SPITimeout = SPIT_FLAG_TIMEOUT;
    /* 等待发送缓冲区为空，TXE事件 */
    while (SPI_I2S_GetFlagStatus(SI24R1_SPI, SPI_I2S_FLAG_TXE) == RESET)
    {
        if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(1);
    }
	SPI_I2S_SendData(SI24R1_SPI, byte);
	/* 等待接收缓冲区为空，RXNE事件 */
    while (SPI_I2S_GetFlagStatus(SI24R1_SPI, SPI_I2S_FLAG_RXNE) == RESET)
    {
        if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(2);
    }
	date = SPI_I2S_ReceiveData(SI24R1_SPI);
	/* 等待发送缓冲区为空，TXE事件 */
    while (SPI_I2S_GetFlagStatus(SI24R1_SPI, SPI_I2S_FLAG_TXE) == RESET)
    {
        if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(3);
    }
	/* 等待发送完，BSY事件 */
    while (SPI_I2S_GetFlagStatus(SI24R1_SPI, SPI_I2S_FLAG_BSY) == SET)
    {
        if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(4);
    }
	return date;
}

/********************************************************
函数功能：SI24R1引脚初始化
入口参数：无
返回  值：无
*********************************************************/
void SI24R1_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI1_InitStructure;
	
    RCC_APB2PeriphClockCmd(SI24R1_GPIO_RCC, ENABLE);		//使能GPIO端口时钟
	RCC_APB1PeriphClockCmd(SI24R1_SPI_RCC, ENABLE);			//使能SPI端口时钟
	
    GPIO_InitStructure.GPIO_Pin = SI24R1_MOSI|SI24R1_SCK|SI24R1_MISO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//复用输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		//速度10MHz
    GPIO_Init(SI24R1_GPIOB, &GPIO_InitStructure);			//初始化GPIOA
	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//推挽输出
    GPIO_InitStructure.GPIO_Pin = SI24R1_CSN;
    GPIO_Init(SI24R1_GPIOB, &GPIO_InitStructure);	  		//初始化GPIOA
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//推挽输出
    GPIO_InitStructure.GPIO_Pin = SI24R1_CE;
    GPIO_Init(SI24R1_GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
    GPIO_InitStructure.GPIO_Pin = SI24R1_IRQ;
    GPIO_Init(SI24R1_GPIOA, &GPIO_InitStructure);	  		//初始化GPIOB
	
	SPI1_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI1_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI1_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI1_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI1_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI1_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI1_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI1_InitStructure.SPI_CRCPolynomial = 7;
	SPI1_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_Init(SI24R1_SPI, &SPI1_InitStructure);
	SPI_Cmd(SI24R1_SPI, ENABLE);
	
    SI24R1_SCK_Clr();	//SPI时钟线拉低 SCK = 0
    SI24R1_CSN_Set();	//CSN = 1
    SI24R1_CE_Clr();	//CE = 0
    SI24R1_MOSI_Clr();	//MOSI = 0
}

/********************************************************
函数功能：写寄存器的值（单字节）
入口参数：reg:寄存器映射地址（格式：WRITE_REG｜reg）
		 value:寄存器的值
返回  值：状态寄存器的值
*********************************************************/
uint8_t SI24R1_Write_Reg(uint8_t reg, uint8_t value)
{
    uint8_t status;
    SI24R1_CSN_Clr();	//CSN = 0
    status = SPI_RW(reg);
    SPI_RW(value);
    SI24R1_CSN_Set();	//CSN = 1
    return(status);
}

/********************************************************
函数功能：写寄存器的值（多字节）
入口参数：reg:寄存器映射地址（格式：WRITE_REG｜reg）
		 pBuf:写数据首地址
		 bytes:写数据字节数
返回  值：状态寄存器的值
*********************************************************/
uint8_t SI24R1_Write_Buf(uint8_t reg, const uint8_t *pBuf, uint8_t bytes)
{
    uint8_t status,byte_ctr;
    SI24R1_CSN_Clr();	//CSN = 0
    status = SPI_RW(reg);
    for(byte_ctr=0; byte_ctr<bytes; byte_ctr++)
        SPI_RW(*pBuf++);
    SI24R1_CSN_Set();	//CSN = 1
    return(status);
}

/********************************************************
函数功能：读取寄存器的值（单字节）
入口参数：reg:寄存器映射地址（格式：READ_REG｜reg）
返回  值：寄存器值
*********************************************************/
uint8_t SI24R1_Read_Reg(uint8_t reg)
{
    uint8_t value;
    SI24R1_CSN_Clr();	//CSN = 0
    SPI_RW(reg);
    value = SPI_RW(0);
    SI24R1_CSN_Set();	//CSN = 1
    return(value);
}

/********************************************************
函数功能：读取寄存器的值（多字节）
入口参数：reg:寄存器映射地址（READ_REG｜reg）
					pBuf:接收缓冲区的首地址
					bytes:读取字节数
返回  值：状态寄存器的值
*********************************************************/
uint8_t SI24R1_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
    uint8_t status,byte_ctr;
    SI24R1_CSN_Clr();	//CSN = 0
    status = SPI_RW(reg);
    for(byte_ctr=0; byte_ctr<bytes; byte_ctr++)
        pBuf[byte_ctr] = SPI_RW(0);	//读取数据，低字节在前
    SI24R1_CSN_Set();	//CSN = 1
    return(status);
}

/********************************************************
函数功能：SI24R1接收模式初始化
入口参数：SI24R1_InitTypeDef
		 Mode RX_MODE or TX_MODE
返回  值：无
*********************************************************/
void SI24R1_Mode_NOACK(SI24R1_InitTypeDef* SI24R1_InitStruct,uint8_t Mode)
{
    //CE = 0
    SI24R1_CE_Clr();
    // 设置地址宽度为5bytes
    SI24R1_Write_Reg(W_REGISTER + SETUP_AW, SI24R1_InitStruct->ADR_WIDTH);
	if(Mode == RX_MODE)
	{
	// 接收使用接收通道0和接收地址
    SI24R1_Write_Buf(W_REGISTER + SI24R1_InitStruct->RX_ADDR_Px, TX_ADDRESS, TX_ADR_WIDTH);
	// 使能接收通道0
    SI24R1_Write_Reg(W_REGISTER + EN_RXADDR, SI24R1_InitStruct->EN_RXADDR_Px);
	// 设置接收通道0负载数据宽度
    SI24R1_Write_Reg(W_REGISTER + SI24R1_InitStruct->RX_PW_Px, TX_PLOAD_WIDTH);
	}
	else
	{
	// 写入发送地址
    SI24R1_Write_Buf(W_REGISTER + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);
	// 使能NOACK	
    SI24R1_Write_Reg(W_REGISTER + FEATURE, SI24R1_InitStruct->RF_FEATURE);
	}		
    // 选择射频通道40
    SI24R1_Write_Reg(W_REGISTER + RF_CH, SI24R1_InitStruct->RF_CHANNEL);
    // 数据传输率2Mbps，发射功率7dBm
    SI24R1_Write_Reg(W_REGISTER + RF_SETUP, SI24R1_InitStruct->RF_SET);
	if(Mode == RX_MODE)
	// CRC使能，16位CRC校验，上电，接收模式
    SI24R1_Write_Reg(W_REGISTER + CONFIG, (SI24R1_InitStruct->SI24R1_CONFIG)|0x01);
	else
	// CRC使能，16位CRC校验，上电，发射模式
    SI24R1_Write_Reg(W_REGISTER + CONFIG, (SI24R1_InitStruct->SI24R1_CONFIG)&0xfe);
    // 拉高CE启动接收设备
    SI24R1_CE_Set();
//#if SI24R1_debug
//	if(Mode == RX_MODE)
//    printf("\nSI24R1 RXinitialze success\n");
//	else
//	printf("\nSI24R1 TXinitialze success\n");
//#endif
}

/********************************************************
函数功能：读取接收数据
入口参数：rxbuf:接收数据存放首地址
返回  值：1:接收到数据
         0:没有接收到数据
*********************************************************/
uint8_t SI24R1_RxPacket(uint8_t *rxbuf)
{
    uint8_t state;
    uint8_t pload_width;
    state = SI24R1_Read_Reg(STATUS);  			      	// 读取状态寄存器的值
    SI24R1_Write_Reg(W_REGISTER + STATUS,state);        // 清除RX_DR中断标志
    if(state & RX_DR)								    // 接收到数据
    {
        pload_width = SI24R1_Read_Reg(R_RX_PL_WID);		// 读取收到的数据字节数
//		#if SI24R1_debug
//			printf("\nSI24R1 RX:0x%X\n",state);
//			printf("\nSI24R1 Readpload_width：%d\n",pload_width);
//		#endif
        SI24R1_Read_Buf(RD_RX_PLOAD,rxbuf,pload_width);	// 读取数据
        SI24R1_Write_Reg(FLUSH_RX,0xff);				// 清除RX FIFO寄存器
        return 1;
    }
    return 0;                                         	// 没收到任何数据
}

/********************************************************
函数功能：发送一个数据包
入口参数：txbuf:要发送的数据
返回  值：0x10:达到最大重发次数，发送失败
         0x20:发送成功
         0xff:发送失败
*********************************************************/
uint8_t SI24R1_TxPacket(uint8_t *txbuf)
{
    uint8_t state;
    SI24R1_CE_Clr();										// CE拉低，使能SI24R1配置
    SI24R1_Write_Buf(WR_TX_PLOAD_NACK, txbuf, TX_PLOAD_WIDTH);	// 写数据到TX FIFO,32个字节
    SI24R1_CE_Set();										// CE置高，使能发送
    SPITimeout = SPIT_FLAG_TIMEOUT;
    while(GPIO_ReadInputDataBit(SI24R1_GPIOB, SI24R1_IRQ))	// 等待发送完成
    {
        if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
    }
    state = SI24R1_Read_Reg(STATUS);  						// 读取状态寄存器的值
    SI24R1_Write_Reg(W_REGISTER + STATUS, state); 			// 清除TX_DS或MAX_RT中断标志
    if(state & MAX_RT)										// 达到最大重发次数
    {
        SI24R1_Write_Reg(FLUSH_TX,0xff);					// 清除TX FIFO寄存器
//#if SI24R1_debug
//        printf("\nSI24R1 TX:0x%X\n",MAX_RT);
//#endif
        return MAX_RT;
    }
    if(state & TX_DS)										// 发送完成
    {
//#if SI24R1_debug
//        printf("\nSI24R1 TX:0x%X\n",TX_DS);
//#endif
        return TX_DS;
    }
//#if SI24R1_debug
//    printf("\nSI24R1 TX:0x%X\n",state);
//#endif
    return state;											//发送失败
}

void SI24R1_Shutdown(void)
{
	SI24R1_Write_Reg(W_REGISTER + CONFIG, 0x0d);
}

void SI24R1_Standby(void)
{
	SI24R1_Write_Reg(W_REGISTER + CONFIG, 0x0f);
}

static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
    /* 等待超时后的处理,输出错误信息 */
//    SI24R1_ERROR("SI24R1 等待超时!errorCode = %d",errorCode);
    return 0;
}
