#ifndef __TXS0102_H
#define __TXS0102_H

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

void TXS0102_IoInit(void);
void TXS0102_DeInit(void);
void TXS0102_OUT_State(uint8_t pin_state);

#ifdef __cplusplus
}
#endif

#endif

