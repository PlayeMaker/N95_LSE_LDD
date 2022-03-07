#include "ult.h"

extern uint8_t rxbuf_u1;
extern uint8_t rxDATA_u1[100];
extern uint8_t rxlen_u1 ;
static uint16_t receive_data_ult_check[10]={0};

void ULT_Rest(void)
{
	rxlen_u1 = 0 ;
	memset(rxDATA_u1,0,sizeof(rxDATA_u1));
}

void ULT_Init(void  (*uart_init)(void),void (*uart_485_enable)(void))
{
	ULT_Rest();
	uart_init();
	uart_485_enable();
}

void ULT_DeInit(void(*uart_disable)(void),void(*uart_485_disable)(void))
{
	uart_disable();	
	uart_485_disable();
}

uint8_t ULT_Connection_Test(UART_HandleTypeDef *UartHandle,void (*GPIO_OUT_Set)(uint8_t state))
{
	uint8_t state=0;
	HAL_UART_Receive_IT(&huart1,(uint8_t*)&rxbuf_u1,RXSIZE);
	HAL_Delay(100);
	GPIO_OUT_Set(1);			//enable txs0102
	HAL_Delay(3000);
	if(rxDATA_u1[0] == 0xFF || rxDATA_u1[1] == 0xFF || rxDATA_u1[2] == 0xFF || rxDATA_u1[3] == 0xFF)
		state = 1;
	GPIO_OUT_Set(0);		//disable txs0102	
	return state;
}

void ULT_getData(UART_HandleTypeDef *UartHandle,void (*GPIO_OUT_Set)(uint8_t state))
{
	GPIO_OUT_Set(1);		//enable txs0102
#if NDDS75
	HAL_Delay(2000);
#elif (NDDS20)	
	HAL_Delay(4000);
#endif
	GPIO_OUT_Set(0);	 //disable txs0102	
}

int ULT_Data_processing(void)
{
	ULT_Check_Sum();
	ULT_Data_Read();
	uint16_t	distanceSum=0;

	distanceSum = (receive_data_ult_check[0] +receive_data_ult_check[1])/2;
	
	user_main_printf("distance:%d",distanceSum);
	return distanceSum;
}

void ULT_Data_Read(void)
{
	for(uint8_t i=0;i<4;i++)
	{
		user_main_noln("%d ",receive_data_ult_check[i]);
	}
}

void ULT_Check_Sum(void)
{
	uint8_t n=0,m=0;
	for(uint8_t i=0;i<10;i++)
	{
		if(rxDATA_u1[n] == 0xFF)
		{
			if(rxDATA_u1[n+3] == ((rxDATA_u1[n]+rxDATA_u1[n+1]+rxDATA_u1[n+2])&0x00FF))
			{
				receive_data_ult_check[m] = rxDATA_u1[n+1]*256+rxDATA_u1[n+2];
				n=n+4;
				m++;
			}
			else
				n++;
		}
		else
		{
			n++;
		}
		
		if(m>=4)
			break;
	}
}
