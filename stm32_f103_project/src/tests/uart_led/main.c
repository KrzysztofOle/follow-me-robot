#include "stm32f1xx_hal.h"

UART_HandleTypeDef huart2;

typedef struct
{
  GPIO_TypeDef *port;
  uint16_t pin;
  const char *name;
} LedOutput;

static void MX_USART2_UART_Init(void);
static void MX_LEDs_Init(void);
static void LEDs_AllOff(void);
static void Error_Handler(void);
static void USART2_SendChar(char ch);
static void USART2_SendString(const char *text);

static const LedOutput leds[] = {
  {GPIOA, GPIO_PIN_5, "PA5"},
  {GPIOC, GPIO_PIN_9, "PC9"},
  {GPIOC, GPIO_PIN_8, "PC8"},
  {GPIOC, GPIO_PIN_5, "PC5"},
};

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (huart->Instance == USART2)
  {
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}

static void MX_LEDs_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);
}

static void LEDs_AllOff(void)
{
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

int main(void)
{
  HAL_Init();
  MX_USART2_UART_Init();
  MX_LEDs_Init();

  USART2_SendString("!\r\n");
  USART2_SendString("BOOT\r\n");
  USART2_SendString("Helo Word\r\n");

  uint8_t led_index = 0U;
  const uint8_t led_count = (uint8_t)(sizeof(leds) / sizeof(leds[0]));

  while (1)
  {
    LEDs_AllOff();
    HAL_GPIO_WritePin(leds[led_index].port, leds[led_index].pin, GPIO_PIN_SET);
    USART2_SendString(leds[led_index].name);
    USART2_SendString("\r\n");

    led_index = (uint8_t)((led_index + 1U) % led_count);
    HAL_Delay(3000U);
  }
}

static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

static void USART2_SendChar(char ch)
{
  uint32_t timeout = HAL_GetTick() + 1000U;

  while ((USART2->SR & USART_SR_TXE) == 0U)
  {
    if ((int32_t)(HAL_GetTick() - timeout) >= 0)
    {
      Error_Handler();
    }
  }

  USART2->DR = (uint8_t)ch;
}

static void USART2_SendString(const char *text)
{
  while (*text != '\0')
  {
    USART2_SendChar(*text++);
  }
}

static void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
