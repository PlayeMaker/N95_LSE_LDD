#include "common.h"
#include "nbInit.h"

static uint8_t sys_pwd[10]={0};
static char sensor_data[500]={0};

SYSTEM sys    = {.pwd=sys_pwd};
SENSOR sensor ={.data=sensor_data};
USER user={0};

void product_information_print(void)
{
#if NSE01
  printf("NSE01 NB-IOT Soil Moisture & EC Sensor\r\n");
	printf("Image Version: "AT_VERSION_STRING"\r\n");
#elif NDDS75
  printf("NDDS75 NB-IOT Distance Sensor\n\r");
	printf("Image Version: "AT_VERSION_STRING"\n\r");
#elif NDDS20
  printf("NDDS20 NB-IOT  Non Contact Liquid Level Sensor\n\r");
	printf("Image Version: "AT_VERSION_STRING"\n\r");			
#else
    #error "Please define a product."
#endif
	printf("NB-IoT Stack : "stack	 "\r\n"
				 "Protocol in Used: ");
	
	if(sys.protocol == COAP_PRO)
		printf("COAP\r\n");
	else if(sys.protocol == UDP_PRO)
		printf("UDP\r\n");
	else if(sys.protocol == MQTT_PRO)
		printf("MQTT\r\n");
	else if(sys.protocol == TCP_PRO)
		printf("TCP\r\n");
}

void reboot_information_print(void)
{
	user_main_debug("reboot flag :0x%04X",RCC->CSR);					//Print register
//__HAL_RCC_GET_FLAG(RCC->CSR);
	
	/*Determine and print the reason for restart*/
	if(RCC->CSR & ((uint32_t)1<<31))
		user_main_printf("reboot error:Low-power!");						//Low power management
	else if(RCC->CSR & ((uint32_t)1<<30))
		user_main_printf("reboot error:Window watchdog!");			//Window watchdog
	else if(RCC->CSR & ((uint32_t)1<<29))
		user_main_printf("reboot error:Independent watchdog!");	//Independent watchdog
	else if(RCC->CSR & ((uint32_t)1<<28))
		user_main_printf("reboot error:Software!");							//Software reset
	else if(RCC->CSR & ((uint32_t)1<<27))
		user_main_printf("reboot error:POR/PDR!");							//por/pdr reset
	else if(RCC->CSR & ((uint32_t)1<<26))
		user_main_printf("reboot error:NRST!");									//NRST pin reset
	else if(RCC->CSR & ((uint32_t)1<<25))
		user_main_printf("reboot error:BOR!");									//Software writes to clear the RMVF bit
	
	__HAL_RCC_CLEAR_RESET_FLAGS(); 														//Clear flag
}

void EX_GPIO_Init(uint8_t state)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	if(state == 0)
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();
		GPIO_InitStruct.Pin = GPIO_PIN_12;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
	else if(state == 1)
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();
		GPIO_InitStruct.Pin = GPIO_PIN_12;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	}
	else if(state == 2)
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();
		GPIO_InitStruct.Pin = GPIO_PIN_12;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	}
	else if(state == 3)
  {
		__HAL_RCC_GPIOB_CLK_ENABLE();
		GPIO_InitStruct.Pin = GPIO_PIN_12;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);		
	}
	
	if(state!=0)
	{
		HAL_NVIC_SetPriority(EXTI4_15_IRQn, 2, 0);
		HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
	}
}

void led_on(uint16_t time)
{
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);
	HAL_Delay(time);

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);
	HAL_Delay(100);
}

void txPayLoadDeal(SENSOR* Sensor)
{	
	memset(Sensor->data,0,sizeof((char*)Sensor->data));
	Sensor->data_len = 0;
	Sensor->singal = nb.singal;
	Sensor->batteryLevel_mV = getVoltage();
	HAL_GPIO_WritePin(Power_5v_GPIO_Port, Power_5v_Pin, GPIO_PIN_SET);
	HAL_Delay(5000+sys.power_time);
	
#if NSE01
		SOIL_Init(uart1_Init,TXS0102_IoInit);
		SOIL_getData(&huart1,TXS0102_OUT_State);		
		SOIL_DeInit(uart1_IoDeInit,TXS0102_DeInit);
		Sensor->soil_water=SOIL_Data_processing(0);
		Sensor->soil_tem=SOIL_Data_processing(1);
		Sensor->soil_ecl=SOIL_Data_processing(2);

#elif (NDDS75 || NDDS20)	
		ULT_Init(uart1_Init,TXS0102_IoInit);
		ULT_getData(&huart1,TXS0102_OUT_State);
		ULT_DeInit(uart1_IoDeInit,TXS0102_DeInit);
		Sensor->distance = ULT_Data_processing();	
		
#endif

	
	HAL_GPIO_WritePin(Power_5v_GPIO_Port, Power_5v_Pin, GPIO_PIN_RESET);
	
	for(int i=0;i<strlen((char*)user.deui);i++)
		sprintf(Sensor->data+strlen(Sensor->data), "%c",  user.deui[i]);
	
	sprintf(Sensor->data+strlen(Sensor->data), "%.4x",  string_touint());
	
	sprintf(Sensor->data+strlen(Sensor->data), "%.4x", Sensor->batteryLevel_mV);
	sprintf(Sensor->data+strlen(Sensor->data), "%.2x", Sensor->singal);
	sprintf(Sensor->data+strlen(Sensor->data), "%.2x", sys.mod-0x30);
#if NSE01
	sprintf(Sensor->data+strlen(Sensor->data), "%.4x", Sensor->soil_water);
	sprintf(Sensor->data+strlen(Sensor->data), "%.4x", Sensor->soil_tem);
	sprintf(Sensor->data+strlen(Sensor->data), "%.4x", Sensor->soil_ecl);
#elif (NDDS75 || NDDS20)	
	sprintf(Sensor->data+strlen(Sensor->data), "%.4x", Sensor->distance);
#endif
	sprintf(Sensor->data+strlen(Sensor->data), "%.2x", Sensor->exit_flag);
	
	sprintf(Sensor->data+strlen(Sensor->data), "%.8x", sensor.time_stamp);

	Sensor->data_len = strlen(Sensor->data)/2;
	if(sys.protocol == MQTT_PRO || sys.protocol == COAP_PRO)
	{
		Sensor->data[strlen(Sensor->data)]=0x1A;
		Sensor->data_len = Sensor->data_len*2+1;
	}
	
	user_main_debug("Sensor->data:%s",Sensor->data);
	user_main_debug("Sensor->data_len:%d",Sensor->data_len);
	Sensor->exit_flag = 0;
	sys.exit_flag = 0;
	
}



/**
	* @brief  Processing received data 
  * @param  Instruction parameter
  * @retval None
  */
void rxPayLoadDeal(char* payload)
{
	uint8_t dataCom[10]={0};
	rtrim((char*)payload);
	StrToHex((char*)dataCom,payload,strlen(payload));
	uint8_t dataCom_len = strlen(payload)/2;
	user_main_debug("dataCom_len:%d",dataCom_len);
	
	switch(dataCom[0])
	{
		case 0x01:
			if(dataCom_len == 4)
			{
				int tdc=( dataCom[1]<<16 | dataCom[2]<<8 | dataCom[3] );
				if(tdc >= 60 && tdc <= 0xFFFFFF)
				{
					sys.tdc = tdc;
					config_Set();
				}
			}
			break;
		case 0x04:
			if(dataCom_len == 2 && dataCom[1] == 0xFF)
				NVIC_SystemReset();
			break;
		case 0x06:
			if(dataCom_len == 4 && (dataCom[3]<=3))
			{
				sys.inmod = dataCom[3]+0x30;
				EX_GPIO_Init(sys.inmod-0x30);
				config_Set();
			}			
			break;
		default:
			printf("Downstream parameter error\n");
			break;
	}
}

void sensor_connect_detection(void)
{
	HAL_IWDG_Refresh(&hiwdg);
	HAL_GPIO_WritePin(Power_5v_GPIO_Port,Power_5v_Pin,GPIO_PIN_SET);//Enable 5v power supply
	HAL_Delay(2000+sys.power_time);
#if NSE01
	SOIL_Init(uart1_Init,TXS0102_IoInit);
	if(SOIL_Connection_Test(&huart1,TXS0102_OUT_State) == 0)
	{
		user_main_printf("\r\nNO FOUND Sensor\r\n");
	}
	else
	{
		for(uint8_t i = 0;i<8;i++)
		{
			HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_8);
			HAL_Delay(100);
		}
	}
	SOIL_DeInit(uart1_IoDeInit,TXS0102_DeInit);
#elif (NDDS75 || NDDS20)
	 uint8_t ldd_flag = 0;
	 ULT_Init(uart1_Init,TXS0102_IoInit);
	 if(ULT_Connection_Test(&huart1,TXS0102_OUT_State) == 0)
	 {
			user_main_printf("\r\nNO FOUND Sensor\r\n");
	 }
	 else
	 {
		for(uint8_t i = 0;i<8;i++)
		{
			HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_8);
			HAL_Delay(100);
		}
		ldd_flag=1;
	 }
	ULT_DeInit(uart1_IoDeInit,TXS0102_DeInit);
		
#if NDDS20
	if(ldd_flag==1)
	{
		for(uint8_t i=0;i<3;i++)
		{
			HAL_Delay(10000);
		}
	}
#endif

#endif
	HAL_GPIO_WritePin(Power_5v_GPIO_Port,Power_5v_Pin,GPIO_PIN_RESET);//Disable 5v power supply
}
/**
	* @brief  Processing version number 
  * @param  NULL
  * @retval None
  */
uint16_t string_touint(void)
{
	char *p;	
	uint8_t chanum=0;	
	uint16_t versi;
	char version[8]="";
	p=AT_VERSION_STRING;
	
	while(*p++!='\0')
	{
		if(*p>='0'&&*p<='9')
		{
			version[chanum++]=*p;
		}		 
	}
	versi=atoi(version);
	return versi;
}

void new_firmware_update(void)
{
	uint16_t update_flags,r_config;
	r_config=*(__IO uint16_t *)EEPROM_USER_START_ADD;	
  if(r_config!=string_touint())
	{
		update_flags = string_touint();
		HAL_FLASHEx_DATAEEPROM_Unlock();
		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_HALFWORD,EEPROM_USER_START_ADD,update_flags);//store hardversion
		HAL_FLASHEx_DATAEEPROM_Lock();
		at_fdr_run(NULL);
	}
}

void StrToHex(char *pbDest, char *pszSrc, int nLen)
{
	char h1, h2;
	char s1, s2;
	for (int i = 0; i < nLen; i++)
	{
		h1 = pszSrc[2 * i];
		h2 = pszSrc[2 * i + 1];
 
		s1 = toupper(h1) - 0x30;
		if (s1 > 9)
			s1 -= 7;
 
		s2 = toupper(h2) - 0x30;
		if (s2 > 9)
			s2 -= 7;
 
		pbDest[i] = s1 * 16 + s2;
	}
}

int hexToint(char *str)
{  
	int i;  
	int n = 0;  
	if (str[0] == '0' && (str[1]=='x' || str[1]=='X'))   
		i = 2;   
	else  
		i = 0;  
	for (; (str[i] >= '0' && str[i] <= '9') || (str[i] >= 'a' && str[i] <= 'z') || (str[i] >='A' && str[i] <= 'Z');++i)  
	{  
		if (tolower(str[i]) > '9')
			n = 16 * n + (10 + tolower(str[i]) - 'a');  
		else  
			n = 16 * n + (tolower(str[i]) - '0');  
	}
	return n;  
}

int countchar(char *str,char a)
{
    int n=0;
    int i = 0;
    while(*(str+i)!='\0')
    {
        if(*(str+i) == a)
            n++;
        i++;
    }
    return n;
}
