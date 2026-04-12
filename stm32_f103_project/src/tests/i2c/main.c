#include "stm32f1xx_hal.h"

UART_HandleTypeDef huart2;
I2C_HandleTypeDef hi2c1;

static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void Error_Handler(void);
static void USART2_SendChar(char ch);
static void USART2_SendString(const char *text);
static void USART2_SendHexByte(uint8_t value);

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

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (hi2c->Instance == I2C1)
  {
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_AFIO_REMAP_I2C1_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

int main(void)
{
  HAL_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();

  USART2_SendString("I2C scan start\r\n");

  while (1)
  {
    uint8_t found = 0U;

    USART2_SendString("Scan: ");

    for (uint8_t addr = 0x08U; addr <= 0x77U; ++addr)
    {
      if (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(addr << 1), 2U, 20U) == HAL_OK)
      {
        USART2_SendString("0x");
        USART2_SendHexByte(addr);
        USART2_SendString(" ");
        found = 1U;
      }
    }

    if (found == 0U)
    {
      USART2_SendString("none");
    }

    USART2_SendString("\r\n");
    HAL_Delay(1000U);
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

static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000U;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0U;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0U;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
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

static void USART2_SendHexByte(uint8_t value)
{
  static const char hex[] = "0123456789ABCDEF";

  USART2_SendChar(hex[(value >> 4) & 0x0FU]);
  USART2_SendChar(hex[value & 0x0FU]);
}

static void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
