#ifndef __SOIL_H
#define __SOIL_H

#include "common.h"

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
 * @brief  
 *
 * @note
 * @retval None
 */

void SOIL_Rest(void);
uint16_t SOIL_Data_processing(uint8_t opt);
uint8_t SOIL_Connection_Test(UART_HandleTypeDef *UartHandle,void (*GPIO_OUT_Set)(uint8_t state));
static uint16_t GetCRC16(uint8_t *ptr,  uint8_t len);
uint8_t SOIL_Crc_Check(void);
void SOIL_getData(UART_HandleTypeDef *UartHandle,void (*GPIO_OUT_Set)(uint8_t state));
void SOIL_Data_Read(char* data);
void SOIL_Init(void  (*uart_init)(void),void (*uart_485_enable)(void));
void SOIL_DeInit(void(*uart_disable)(void),void(*uart_485_disable)(void));

#ifdef __cplusplus
}
#endif

#endif

