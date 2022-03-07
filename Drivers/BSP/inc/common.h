/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMON_H__
#define __COMMON_H__

#include "stm32l0xx_hal.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "usart.h"
#include "adc.h"
#include "iwdg.h"

#include "at.h"
#include "time_server.h"
#include "battery_read.h"
#include "ds18b20.h"
#include "soil.h"
#include "ult.h"
#include "txs0102.h"

#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 
/**
 * @brief  initialises the 
 *
 * @note
 * @retval None
 */

#define AT_VERSION_STRING 	"v1.2.0"
#define stack 							"D-BC95-003"

#define COAP_PRO  0x01
#define UDP_PRO   0x02
#define MQTT_PRO  0x03
#define TCP_PRO   0x04

typedef enum
{
 model1 ='1',
}model;


typedef struct
{
	uint16_t len;
	uint8_t* data;
}USART;

typedef struct
{
	uint8_t*		pwd;		   	 		//System password
	uint8_t			pwd_flag;		 		//Password correct flag
	uint8_t			fdr_flag;		 		//FDR flag
	uint8_t  		exit_flag;		 	//System external interrupt 
	uint8_t  		mod;				   	//mode
	uint8_t  		inmod;			   	//Interrupt mode
	uint32_t 		tdc;				   	//Send cycle
	uint16_t 		power_time;	 		//Power on time 
	uint16_t 		uplink_count; 	//Number of postings
	uint8_t  		protocol;		 		//protocol
	uint8_t  		cfm;					 	//Confirm mode flag
	uint16_t 		rxdl;				 		//Receiving time

	USART				usart;
}SYSTEM;

typedef struct
{
	uint8_t  deui[15];
	uint8_t  add[25];
	uint8_t  uri[128];
	uint8_t  client[41];
	uint8_t  uname[41];
	uint8_t  pwd[41];
	uint8_t  pubtopic[65];
	uint8_t  subtopic[65];
}USER;

typedef struct
{
	uint8_t  exit_flag;
	uint8_t  singal;
	uint32_t exit_count;
	uint32_t time_stamp;
	uint16_t temDs18b20_1;
	uint16_t batteryLevel_mV;
	uint16_t distance;
	uint16_t soil_tem;
	uint16_t soil_water;
	uint16_t soil_ecl;
	char*  		data;	
	uint16_t  data_len;
}SENSOR;


#ifdef __cplusplus
}
#endif

extern SYSTEM sys;
extern SENSOR sensor;
extern USER user;

void product_information_print(void);
void reboot_information_print(void);
void EX_GPIO_Init(uint8_t state);
void led_on(uint16_t time);

void txPayLoadDeal(SENSOR* Sensor);
void rxPayLoadDeal(char* payload);
int hexToint(char *str);
uint16_t string_touint(void);
void StrToHex(char *pbDest, char *pszSrc, int nLen);
void new_firmware_update(void);
void sensor_connect_detection(void);
int countchar(char *str,char a);
#endif 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
