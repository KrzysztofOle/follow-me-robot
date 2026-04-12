#include "stm32f1xx_hal.h"
#include <stdint.h>

UART_HandleTypeDef huart2;
I2C_HandleTypeDef hi2c1;

#define URM09_RANGE_150 0x00U
#define URM09_MODE_AUTOMATIC 0x80U

static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void Error_Handler(void);
static void UART_SendChar(char ch);
static void UART_SendString(const char *text);
static void UART_SendHexByte(uint8_t value);
static void UART_SendU16(uint16_t value);
static void UART_SendScan(void);
static HAL_StatusTypeDef URM09_FindAddress(uint8_t *addr_out);
static HAL_StatusTypeDef URM09_WriteRegister(uint8_t addr, uint8_t reg, uint8_t value);
static HAL_StatusTypeDef URM09_ReadRegisters(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len);
static HAL_StatusTypeDef URM09_SetModeRange(uint8_t addr, uint8_t range, uint8_t mode);
static HAL_StatusTypeDef URM09_InitFast(uint8_t addr);
static HAL_StatusTypeDef URM09_ReadDistance(uint8_t addr, uint16_t *distance_cm);

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
  uint16_t distance_cm = 0U;
  uint8_t sample_index = 0U;
  uint8_t sensor_addr = 0U;

  HAL_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();

  UART_SendString("\r\n================================\r\n");
  UART_SendString("URM09 fast distance log\r\n");
  UART_SendString("I2C scan: ");
  UART_SendScan();
  if (URM09_FindAddress(&sensor_addr) != HAL_OK)
  {
    UART_SendString("sensor not found\r\n");
    Error_Handler();
  }
  UART_SendString("addr=0x");
  UART_SendHexByte(sensor_addr);
  UART_SendString("\r\n");
  UART_SendString("log every 6th read\r\n");
  UART_SendString("================================\r\n");

  if (URM09_InitFast(sensor_addr) != HAL_OK)
  {
    UART_SendString("URM09 init warning\r\n");
  }

  while (1)
  {
    sample_index = (uint8_t)((sample_index + 1U) % 6U);

    if (URM09_ReadDistance(sensor_addr, &distance_cm) == HAL_OK)
    {
      if (sample_index == 0U)
      {
        UART_SendU16(distance_cm);
        UART_SendString(" cm\r\n");
      }
    }
    else if (sample_index == 0U)
    {
      UART_SendString("read err\r\n");
    }
  }
}

static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200U;
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

static void UART_SendChar(char ch)
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

static void UART_SendString(const char *text)
{
  while (*text != '\0')
  {
    UART_SendChar(*text++);
  }
}

static void UART_SendHexByte(uint8_t value)
{
  static const char hex[] = "0123456789ABCDEF";

  UART_SendChar(hex[(value >> 4) & 0x0FU]);
  UART_SendChar(hex[value & 0x0FU]);
}

static void UART_SendU16(uint16_t value)
{
  char buf[6];
  uint8_t idx = 0U;

  if (value == 0U)
  {
    UART_SendChar('0');
    return;
  }

  while (value > 0U && idx < sizeof(buf))
  {
    buf[idx++] = (char)('0' + (value % 10U));
    value /= 10U;
  }

  while (idx > 0U)
  {
    UART_SendChar(buf[--idx]);
  }
}

static void UART_SendScan(void)
{
  uint8_t any = 0U;

  for (uint8_t addr = 0x08U; addr <= 0x77U; ++addr)
  {
    if (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(addr << 1), 2U, 20U) == HAL_OK)
    {
      UART_SendString("0x");
      UART_SendHexByte(addr);
      UART_SendString(" ");
      any = 1U;
    }
  }

  if (any == 0U)
  {
    UART_SendString("none");
  }

  UART_SendString("\r\n");
}

static HAL_StatusTypeDef URM09_WriteRegister(uint8_t addr, uint8_t reg, uint8_t value)
{
  return HAL_I2C_Mem_Write(&hi2c1, (uint16_t)(addr << 1), reg, I2C_MEMADD_SIZE_8BIT, &value, 1U, 50U);
}

static HAL_StatusTypeDef URM09_ReadRegisters(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len)
{
  return HAL_I2C_Mem_Read(&hi2c1, (uint16_t)(addr << 1), reg, I2C_MEMADD_SIZE_8BIT, data, len, 50U);
}

static HAL_StatusTypeDef URM09_FindAddress(uint8_t *addr_out)
{
  for (uint8_t addr = 0x08U; addr <= 0x77U; ++addr)
  {
    if (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(addr << 1), 2U, 20U) == HAL_OK)
    {
      if (addr == 0x3EU)
      {
        continue;
      }

      *addr_out = addr;
      return HAL_OK;
    }
  }

  return HAL_ERROR;
}

static HAL_StatusTypeDef URM09_SetModeRange(uint8_t addr, uint8_t range, uint8_t mode)
{
  return URM09_WriteRegister(addr, 0x07U, (uint8_t)(range | mode));
}

static HAL_StatusTypeDef URM09_InitFast(uint8_t addr)
{
  for (uint8_t attempt = 0U; attempt < 5U; ++attempt)
  {
    if (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(addr << 1), 2U, 20U) == HAL_OK &&
        URM09_SetModeRange(addr, URM09_RANGE_150, URM09_MODE_AUTOMATIC) == HAL_OK)
    {
      HAL_Delay(20U);
      return HAL_OK;
    }

    HAL_Delay(40U);
  }

  return HAL_ERROR;
}

static HAL_StatusTypeDef URM09_ReadDistance(uint8_t addr, uint16_t *distance_cm)
{
  uint8_t data[4] = {0};

  if (URM09_ReadRegisters(addr, 0x03U, data, 4U) != HAL_OK)
  {
    return HAL_ERROR;
  }

  *distance_cm = (uint16_t)((data[0] << 8) | data[1]);
  return HAL_OK;
}

static void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
