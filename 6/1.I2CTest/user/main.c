#include "stm32f10x.h"
#include "stm32f10x_pal.h"
#include "stm32f10x_pal_i2c.h"

static void App_I2C_Init(void);
static ErrorStatus App_I2C_MasterTransmit(uint8_t SlaveAddr, const uint8_t *pData, uint16_t Size);
static ErrorStatus App_I2C_MasterReceive(uint8_t SlaveAddr, uint8_t *pDataOut, uint16_t Size);

static PalI2C_HandleTypeDef hi2c1;

int main(void)
{
	uint8_t oled_init_command[] = {
		0x00, // Command Stream
		0xa8, 0x3f, // Set MUX Ratio
		0xd3, 0x00, // Set Display Offset
		0x40, // Set Display Start Line
		0xa0, // Set Segment re-map
		0xc0, // Set COM Output Scan Direction
		0xda, 0x02, // Set COM Pins hardware configuration
		0x81, 0x7f, // Set Contrast Control
		0xa5, // Enable Entire Display On
		0xa6, // Set Normal Display
		0xd5, 0x80, // Set OSC Frequency
		0x8d, 0x14, // Enable charge pump regulator
		0xaf, // Display on
	};
	
	uint8_t oled_off_command[] = { 0x00, 0xae};
	uint8_t oled_on_command[] = { 0x00, 0xaf};
	
	uint8_t buffer[8];
	
	
	PAL_Init();
	
	hi2c1.Init.I2Cx = I2C1;
	hi2c1.Init.I2C_ClockSpeed = 400000;
	hi2c1.Init.I2C_DutyCycle = I2C_DutyCycle_2;
	PAL_I2C_Init(&hi2c1);
	
	// App_I2C_Init();
	
	PAL_I2C_MasterTransmit(&hi2c1, 0x78, oled_init_command, sizeof(oled_init_command)/sizeof(uint8_t));
	//App_I2C_MasterTransmit(0x78, oled_init_command, sizeof(oled_init_command)/sizeof(uint8_t));
	
	// 熄灭显示器
	PAL_I2C_MasterTransmit(&hi2c1, 0x78, oled_off_command, sizeof(oled_off_command)/sizeof(uint8_t));
	PAL_I2C_MasterReceive(&hi2c1, 0x78, buffer, 1);
	PAL_I2C_MasterReceive(&hi2c1, 0x78, buffer, 2);
	PAL_I2C_MasterReceive(&hi2c1, 0x78, buffer, 8);
	
	// 点亮显示器
	PAL_I2C_MasterTransmit(&hi2c1, 0x78, oled_on_command, sizeof(oled_off_command)/sizeof(uint8_t));
	PAL_I2C_MasterReceive(&hi2c1, 0x78, buffer, 1);
	PAL_I2C_MasterReceive(&hi2c1, 0x78, buffer, 2);
	PAL_I2C_MasterReceive(&hi2c1, 0x78, buffer, 8);
	
	while(1)
	{
	}
}

static void App_I2C_Init(void)
{
	// #1. 初始化SCL和SDA引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// #2. 开启I2C的时钟并复位
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
	
	// #3. 设置I2C的参数
	I2C_InitTypeDef I2C_InitStruct;
	
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_ClockSpeed = 400000;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	
	I2C_Init(I2C1, &I2C_InitStruct);
	
	// #4. 使能I2C
	I2C_Cmd(I2C1, ENABLE);
}

static ErrorStatus App_I2C_MasterTransmit(uint8_t SlaveAddr, const uint8_t *pData, uint16_t Size)
{
	ErrorStatus ret = SUCCESS;
	
	// #1. 等待总线空闲
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);
	// #2. 发送起始位
	I2C_GenerateSTART(I2C1, ENABLE);
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_SB) == RESET);
	// #3. 发送地址
	I2C_ClearFlag(I2C1, I2C_FLAG_AF);
	I2C_SendData(I2C1, SlaveAddr & 0xfe);
	
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) == RESET)
	{
		if(I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET)
		{
			ret = ERROR;
			goto STOP;
		}
	}
	
	// #4. 发送数据
	I2C_ReadRegister(I2C1, I2C_Register_SR1); // 清除ADDR
	I2C_ReadRegister(I2C1, I2C_Register_SR2);
	
	uint32_t i;
	
	for(i=0;i<Size;i++)
	{
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE) == RESET)
		{
			if(I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET)
			{
				ret = ERROR;
				goto STOP;
			}
		}
		I2C_SendData(I2C1, pData[i]);
	}
	
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == RESET)
	{
		if(I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET)
		{
			ret = ERROR;
			goto STOP;
		}
	}
	
	// #5. 发送停止位
STOP:
	I2C_GenerateSTOP(I2C1, ENABLE);
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);
	
	return ret;
}

static ErrorStatus App_I2C_MasterReceive(uint8_t SlaveAddr, uint8_t *pDataOut, uint16_t Size)
{
	// #1. 等待总线空闲
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);
	
	// #2. 发送起始位
	I2C_GenerateSTART(I2C1, ENABLE);
	
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_SB) == RESET);
	
	// #3. 发送从机地址
	I2C_ClearFlag(I2C1, I2C_FLAG_AF);
	
	I2C_SendData(I2C1, SlaveAddr | 0x01);
	
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) == RESET)
	{
		if(I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET)
		{
			I2C_GenerateSTOP(I2C1, ENABLE);
			while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);
			
			return ERROR;
		}
	}
	
	// #4. 接收数据
	if(Size == 1)
	{
		// 4.1. 清除ADDR标志位
		I2C_ReadRegister(I2C1, I2C_Register_SR1);
		I2C_ReadRegister(I2C1, I2C_Register_SR2);
		
		// 4.2. ACK=0, STOP=1
		I2C_AcknowledgeConfig(I2C1, DISABLE);
		I2C_GenerateSTOP(I2C1, ENABLE);
		
		// 4.3. 等待RXNE，然后把数据读出来
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);
		
		pDataOut[0] = I2C_ReceiveData(I2C1);
		
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);
		
		return SUCCESS;
	}
	else if(Size == 2)
	{
		I2C_AcknowledgeConfig(I2C1, ENABLE);
		
		// 4.1. 清除ADDR标志位
		I2C_ReadRegister(I2C1, I2C_Register_SR1);
		I2C_ReadRegister(I2C1, I2C_Register_SR2);
		
		// 4.2. 等待RXNE=1
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);
		I2C_AcknowledgeConfig(I2C1, DISABLE);
		I2C_GenerateSTOP(I2C1, ENABLE);
		pDataOut[0] = I2C_ReceiveData(I2C1);
		
		// 4.3. 接收第2个字节
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);
		pDataOut[1] = I2C_ReceiveData(I2C1);
		
		// 4.4. 等待总线空闲
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);
		
		return SUCCESS;
	}
	else if(Size > 2)
	{
		// ACK = 1 N-1 ACK NAK
		I2C_AcknowledgeConfig(I2C1, ENABLE);
		
		// 4.1. 清除ADDR标志位
		I2C_ReadRegister(I2C1, I2C_Register_SR1);
		I2C_ReadRegister(I2C1, I2C_Register_SR2);
		
		// 4.2. 
		uint32_t i;
		for(i=0;i<Size-3;i++)
		{
			while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);
			
			pDataOut[i] = I2C_ReceiveData(I2C1);
		}
		
		// 4.3. 等待BTF标志位置位
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == RESET);
		
		pDataOut[Size-3] = I2C_ReceiveData(I2C1);
		pDataOut[Size-2] = I2C_ReceiveData(I2C1);
		
		// 4.4. ACK=0,STOP=1
		I2C_AcknowledgeConfig(I2C1, DISABLE);
		I2C_GenerateSTOP(I2C1, ENABLE);
		
		// 4.5. 等待RXNE标志位置位
		pDataOut[Size-1] = I2C_ReceiveData(I2C1);
		
		// 4.6. 等待总线空闲
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);
		
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}
