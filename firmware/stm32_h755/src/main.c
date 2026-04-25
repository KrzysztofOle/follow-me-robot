#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <string.h>
#if !defined(CORE_CM7) && !defined(CORE_CM4)
#error "This firmware expects CORE_CM7 or CORE_CM4 to be defined by the board."
#endif

#if defined(CORE_CM7)
#define LED_GPIO_PORT GPIOB
#define LED_GPIO_PIN GPIO_PIN_0
#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define BLINK_DELAY_MS 150U
#define LOG_PERIOD_MS 1000U
#elif defined(CORE_CM4)
#define LED_GPIO_PORT GPIOE
#define LED_GPIO_PIN GPIO_PIN_1
#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()
#define BLINK_DELAY_MS 600U
#endif

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void Error_Handler(void);

#if defined(CORE_CM7)
static UART_HandleTypeDef huart3;
static void LogString(const char *text);
#endif

int main(void) {
#if defined(CORE_CM7)
  uint32_t last_log_tick = 0U;
#endif

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
#if defined(CORE_CM7)
  MX_USART3_UART_Init();
  setvbuf(stdout, NULL, _IONBF, 0);
  LogString("CM7 start\r\n");
  last_log_tick = HAL_GetTick();
#endif

  while (1) {
    HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_GPIO_PIN);
#if defined(CORE_CM7)
    if ((HAL_GetTick() - last_log_tick) >= LOG_PERIOD_MS) {
      last_log_tick = HAL_GetTick();
      char buf[40];
      int len = snprintf(buf, sizeof(buf), "CM7 heartbeat tick=%lu\r\n", (unsigned long)last_log_tick);
      if (len > 0) {
        LogString(buf);
      }
    }
#endif
    HAL_Delay(BLINK_DELAY_MS);
  }
}

static void SystemClock_Config(void) {
  RCC_OscInitTypeDef osc_init = {0};
  RCC_ClkInitTypeDef clk_init = {0};

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

#if defined(CORE_CM7)
static void MX_USART3_UART_Init(void) {
  GPIO_InitTypeDef gpio_init = {0};

  __HAL_RCC_USART3_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  gpio_init.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  gpio_init.Mode = GPIO_MODE_AF_PP;
  gpio_init.Pull = GPIO_PULLUP;
  gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio_init.Alternate = GPIO_AF7_USART3;
  HAL_GPIO_Init(GPIOD, &gpio_init);

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  if (HAL_UART_Init(&huart3) != HAL_OK) {
    Error_Handler();
  }
}

static void LogString(const char *text) {
  size_t len = strlen(text);
  if (HAL_UART_Transmit(&huart3, (uint8_t *)text, (uint16_t)len, HAL_MAX_DELAY) != HAL_OK) {
    Error_Handler();
  }
}
#endif

static void Error_Handler(void) {
  __disable_irq();
  while (1) {
  }
}
