#include "stm32h7xx_hal.h"

#if !defined(CORE_CM7) && !defined(CORE_CM4)
#error "This firmware expects CORE_CM7 or CORE_CM4 to be defined by the board."
#endif

#if defined(CORE_CM7)
#define LED_GPIO_PORT GPIOB
#define LED_GPIO_PIN GPIO_PIN_0
#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#elif defined(CORE_CM4)
#define LED_GPIO_PORT GPIOE
#define LED_GPIO_PIN GPIO_PIN_1
#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()
#endif

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void Error_Handler(void);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  while (1) {
    HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_GPIO_PIN);
    HAL_Delay(250U);
  }
}

static void SystemClock_Config(void) {
  RCC_OscInitTypeDef osc_init = {0};
  RCC_ClkInitTypeDef clk_init = {0};

  __HAL_RCC_HSEM_CLK_ENABLE();

  osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  osc_init.HSIState = RCC_HSI_ON;
  osc_init.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  osc_init.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&osc_init) != HAL_OK) {
    Error_Handler();
  }

  clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                       RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 |
                       RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1;
  clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  clk_init.SYSCLKDivider = RCC_SYSCLK_DIV1;
  clk_init.AHBCLKDivider = RCC_HCLK_DIV1;
  clk_init.APB3CLKDivider = RCC_APB3_DIV1;
  clk_init.APB1CLKDivider = RCC_APB1_DIV1;
  clk_init.APB2CLKDivider = RCC_APB2_DIV1;
  clk_init.APB4CLKDivider = RCC_APB4_DIV1;
  if (HAL_RCC_ClockConfig(&clk_init, FLASH_LATENCY_0) != HAL_OK) {
    Error_Handler();
  }
}

static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef gpio_init = {0};

  LED_GPIO_CLK_ENABLE();

  gpio_init.Pin = LED_GPIO_PIN;
  gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init.Pull = GPIO_NOPULL;
  gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_PORT, &gpio_init);

  HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_RESET);
}

static void Error_Handler(void) {
  __disable_irq();
  while (1) {
  }
}
