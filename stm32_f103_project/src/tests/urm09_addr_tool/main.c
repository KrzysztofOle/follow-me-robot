#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <stdlib.h>

UART_HandleTypeDef huart2;
I2C_HandleTypeDef hi2c1;

static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void Error_Handler(void);
static void UART_SendChar(char ch);
static void UART_SendString(const char *text);
static void UART_SendHexByte(uint8_t value);
static char UART_GetChar(void);
static void UART_ReadLine(char *buffer, uint16_t buffer_size);
static void UART_PromptAddress(const char *prompt, uint8_t default_addr, uint8_t *addr_out);
static void I2C_ScanBus(void);
static HAL_StatusTypeDef URM09_ReadReg(uint8_t addr, uint8_t reg, uint8_t *value);
static HAL_StatusTypeDef URM09_WriteReg(uint8_t addr, uint8_t reg, uint8_t value);
static void URM09_ReportIdentity(uint8_t addr);
static HAL_StatusTypeDef URM09_GetAddress(uint8_t addr, uint8_t *value);
static HAL_StatusTypeDef URM09_GetPid(uint8_t addr, uint8_t *value);
static HAL_StatusTypeDef URM09_GetVersion(uint8_t addr, uint8_t *value);
static HAL_StatusTypeDef URM09_ModifyAddress(uint8_t current_addr, uint8_t new_addr);
static uint8_t ParseAddress(const char *text, uint8_t *addr_out);

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
  uint8_t current_addr = 0x11U;
  uint8_t new_addr = 0U;

  HAL_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();

  UART_SendString("\r\nURM09 address tool\r\n");
  UART_SendString("Bus scan:\r\n");
  I2C_ScanBus();

  UART_PromptAddress("Current URM09 address [0x11]: ", 0x11U, &current_addr);

  UART_SendString("\r\nReading identity registers...\r\n");
  URM09_ReportIdentity(current_addr);

  UART_PromptAddress("New URM09 address: ", 0U, &new_addr);

  if (new_addr == current_addr)
  {
    UART_SendString("New address is the same as the current address. No change made.\r\n");
    while (1)
    {
      HAL_Delay(1000U);
    }
  }

  if (URM09_ModifyAddress(current_addr, new_addr) != HAL_OK)
  {
    UART_SendString("Failed to write new address.\r\n");
    Error_Handler();
  }

  HAL_Delay(100U);

  UART_SendString("Verifying new address...\r\n");
  URM09_ReportIdentity(new_addr);

  while (1)
  {
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
  huart2.Init.Mode = UART_MODE_TX_RX;
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

static char UART_GetChar(void)
{
  while ((USART2->SR & USART_SR_RXNE) == 0U)
  {
  }

  return (char)(USART2->DR & 0xFFU);
}

static void UART_ReadLine(char *buffer, uint16_t buffer_size)
{
  uint16_t index = 0U;

  if (buffer_size == 0U)
  {
    return;
  }

  while (1)
  {
    char ch = UART_GetChar();

    if (ch == '\r' || ch == '\n')
    {
      UART_SendString("\r\n");
      break;
    }

    if ((ch == '\b' || ch == 127) && index > 0U)
    {
      index--;
      UART_SendString("\b \b");
      continue;
    }

    if ((index + 1U) < buffer_size)
    {
      buffer[index++] = ch;
      UART_SendChar(ch);
    }
  }

  buffer[index] = '\0';
}

static uint8_t ParseAddress(const char *text, uint8_t *addr_out)
{
  char *end = NULL;
  unsigned long value = strtoul(text, &end, 0);

  if (end == text)
  {
    return 0U;
  }

  while (*end == ' ' || *end == '\t')
  {
    ++end;
  }

  if (*end != '\0')
  {
    return 0U;
  }

  if (value < 1UL || value > 127UL)
  {
    return 0U;
  }

  *addr_out = (uint8_t)value;
  return 1U;
}

static void UART_PromptAddress(const char *prompt, uint8_t default_addr, uint8_t *addr_out)
{
  char line[16];

  while (1)
  {
    UART_SendString(prompt);
    UART_ReadLine(line, sizeof(line));

    if (line[0] == '\0' && default_addr != 0U)
    {
      *addr_out = default_addr;
      UART_SendString("Using default address 0x");
      UART_SendHexByte(default_addr);
      UART_SendString("\r\n");
      return;
    }

    if (ParseAddress(line, addr_out) != 0U)
    {
      UART_SendString("Selected address 0x");
      UART_SendHexByte(*addr_out);
      UART_SendString("\r\n");
      return;
    }

    UART_SendString("Invalid address. Use decimal or hex, for example 18 or 0x12.\r\n");
  }
}

static void I2C_ScanBus(void)
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

static HAL_StatusTypeDef URM09_ReadReg(uint8_t addr, uint8_t reg, uint8_t *value)
{
  return HAL_I2C_Mem_Read(&hi2c1, (uint16_t)(addr << 1), reg, I2C_MEMADD_SIZE_8BIT, value, 1U, 100U);
}

static HAL_StatusTypeDef URM09_WriteReg(uint8_t addr, uint8_t reg, uint8_t value)
{
  return HAL_I2C_Mem_Write(&hi2c1, (uint16_t)(addr << 1), reg, I2C_MEMADD_SIZE_8BIT, &value, 1U, 100U);
}

static HAL_StatusTypeDef URM09_GetAddress(uint8_t addr, uint8_t *value)
{
  return URM09_ReadReg(addr, 0x00U, value);
}

static HAL_StatusTypeDef URM09_GetPid(uint8_t addr, uint8_t *value)
{
  return URM09_ReadReg(addr, 0x01U, value);
}

static HAL_StatusTypeDef URM09_GetVersion(uint8_t addr, uint8_t *value)
{
  return URM09_ReadReg(addr, 0x02U, value);
}

static void URM09_ReportIdentity(uint8_t addr)
{
  uint8_t slave_addr = 0U;
  uint8_t pid = 0U;
  uint8_t version = 0U;

  if (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(addr << 1), 2U, 20U) != HAL_OK)
  {
    UART_SendString("No device responds at 0x");
    UART_SendHexByte(addr);
    UART_SendString("\r\n");
    return;
  }

  if (URM09_GetAddress(addr, &slave_addr) != HAL_OK ||
      URM09_GetPid(addr, &pid) != HAL_OK ||
      URM09_GetVersion(addr, &version) != HAL_OK)
  {
    UART_SendString("Failed to read identity registers from 0x");
    UART_SendHexByte(addr);
    UART_SendString("\r\n");
    return;
  }

  UART_SendString("Candidate at 0x");
  UART_SendHexByte(addr);
  UART_SendString(": slave=0x");
  UART_SendHexByte(slave_addr);
  UART_SendString(" pid=0x");
  UART_SendHexByte(pid);
  UART_SendString(" ver=0x");
  UART_SendHexByte(version);
  UART_SendString("\r\n");
}

static HAL_StatusTypeDef URM09_ModifyAddress(uint8_t current_addr, uint8_t new_addr)
{
  return URM09_WriteReg(current_addr, 0x00U, new_addr);
}

static void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
