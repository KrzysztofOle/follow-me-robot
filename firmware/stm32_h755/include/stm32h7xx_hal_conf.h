#ifndef STM32H7XX_HAL_CONF_H
#define STM32H7XX_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* On NUCLEO-H755ZI-Q the ST-LINK MCO feeds HSE at 8 MHz. */
#define HSE_VALUE 8000000UL

#include "stm32h7xx_hal_conf_template.h"

#ifdef __cplusplus
}
#endif

#endif /* STM32H7XX_HAL_CONF_H */

