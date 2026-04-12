#include "stm32f1xx_hal.h"
#include <stdio.h>

I2C_HandleTypeDef hi2c1;

static void MX_I2C1_Init(void);
static void Error_Handler(void);
static void LCD_WriteCommand(uint8_t cmd);
static void LCD_WriteData(uint8_t data);
static void LCD_Init(void);
static void LCD_Clear(void);
static void LCD_SetCursor(uint8_t row, uint8_t col);
static HAL_StatusTypeDef LCD_WriteRaw(uint8_t control, const uint8_t *data, uint16_t len);
static void LCD_PrintPadded(const char *text, uint8_t width);
static HAL_StatusTypeDef URM09_WriteRegister(uint8_t reg, uint8_t value);
static HAL_StatusTypeDef URM09_ReadRegisters(uint8_t reg, uint8_t *data, uint16_t len);
static HAL_StatusTypeDef URM09_SetModeRange(uint8_t range, uint8_t mode);
static HAL_StatusTypeDef URM09_Measurement(void);
static HAL_StatusTypeDef URM09_ReadDistanceTemp(int16_t *distance_cm, int16_t *temperature_x10);

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
  MX_I2C1_Init();

  LCD_Init();
  LCD_Clear();
  if (URM09_SetModeRange(0x20U, 0x00U) != HAL_OK)
  {
    Error_Handler();
  }

  while (1)
  {
    int16_t distance_cm = 0;
    int16_t temperature_x10 = 0;

    if (URM09_Measurement() != HAL_OK)
    {
      LCD_SetCursor(0U, 0U);
      LCD_PrintPadded("Grove LCD OK", 16U);
      LCD_SetCursor(1U, 0U);
      LCD_PrintPadded("Measure ERR", 16U);
      HAL_Delay(500U);
      continue;
    }

    HAL_Delay(120U);

    if (URM09_ReadDistanceTemp(&distance_cm, &temperature_x10) != HAL_OK)
    {
      LCD_SetCursor(0U, 0U);
      LCD_PrintPadded("Grove LCD OK", 16U);
      LCD_SetCursor(1U, 0U);
      LCD_PrintPadded("Sensor read ERR", 16U);
      HAL_Delay(500U);
      continue;
    }

    char line1[17];
    char line2[17];
    int16_t temperature_whole = (int16_t)(temperature_x10 / 10);
    int16_t temperature_frac = (int16_t)(temperature_x10 < 0 ? -temperature_x10 : temperature_x10);

    snprintf(line1, sizeof(line1), "Dist:%4d cm", (int)distance_cm);
    snprintf(line2, sizeof(line2), "Temp:%2d.%1d C", (int)temperature_whole, (int)(temperature_frac % 10));

    LCD_SetCursor(0U, 0U);
    LCD_PrintPadded(line1, 16U);
    LCD_SetCursor(1U, 0U);
    LCD_PrintPadded(line2, 16U);

    HAL_Delay(500U);
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

static HAL_StatusTypeDef LCD_WriteRaw(uint8_t control, const uint8_t *data, uint16_t len)
{
  uint8_t buffer[17];

  if (len > 16U)
  {
    return HAL_ERROR;
  }

  buffer[0] = control;
  for (uint16_t i = 0U; i < len; ++i)
  {
    buffer[i + 1U] = data[i];
  }

  return HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)(0x3EU << 1), buffer, (uint16_t)(len + 1U), 100U);
}

static void LCD_WriteCommand(uint8_t cmd)
{
  uint8_t data = cmd;

  if (LCD_WriteRaw(0x00U, &data, 1U) != HAL_OK)
  {
    Error_Handler();
  }
}

static void LCD_WriteData(uint8_t data)
{
  if (LCD_WriteRaw(0x40U, &data, 1U) != HAL_OK)
  {
    Error_Handler();
  }
}

static void LCD_Init(void)
{
  HAL_Delay(50U);

  LCD_WriteCommand(0x38U);
  LCD_WriteCommand(0x39U);
  LCD_WriteCommand(0x14U);
  LCD_WriteCommand(0x70U);
  LCD_WriteCommand(0x56U);
  LCD_WriteCommand(0x6CU);
  HAL_Delay(200U);
  LCD_WriteCommand(0x38U);
  LCD_WriteCommand(0x0CU);
  LCD_WriteCommand(0x01U);
  HAL_Delay(2U);
}

static void LCD_Clear(void)
{
  LCD_WriteCommand(0x01U);
  HAL_Delay(2U);
}

static void LCD_SetCursor(uint8_t row, uint8_t col)
{
  uint8_t address = (row == 0U) ? (uint8_t)(0x80U + col) : (uint8_t)(0xC0U + col);
  LCD_WriteCommand(address);
}

static void LCD_PrintPadded(const char *text, uint8_t width)
{
  uint8_t i = 0U;

  while (*text != '\0' && i < width)
  {
    LCD_WriteData((uint8_t)*text++);
    ++i;
  }

  while (i < width)
  {
    LCD_WriteData(' ');
    ++i;
  }
}

static HAL_StatusTypeDef URM09_WriteRegister(uint8_t reg, uint8_t value)
{
  return HAL_I2C_Mem_Write(&hi2c1, (uint16_t)(0x11U << 1), reg, I2C_MEMADD_SIZE_8BIT, &value, 1U, 100U);
}

static HAL_StatusTypeDef URM09_ReadRegisters(uint8_t reg, uint8_t *data, uint16_t len)
{
  return HAL_I2C_Mem_Read(&hi2c1, (uint16_t)(0x11U << 1), reg, I2C_MEMADD_SIZE_8BIT, data, len, 100U);
}

static HAL_StatusTypeDef URM09_SetModeRange(uint8_t range, uint8_t mode)
{
  return URM09_WriteRegister(0x07U, (uint8_t)(range | mode));
}

static HAL_StatusTypeDef URM09_Measurement(void)
{
  return URM09_WriteRegister(0x08U, 0x01U);
}

static HAL_StatusTypeDef URM09_ReadDistanceTemp(int16_t *distance_cm, int16_t *temperature_x10)
{
  uint8_t distance_buf[2] = {0};
  uint8_t temperature_buf[2] = {0};

  if (URM09_ReadRegisters(0x03U, distance_buf, 2U) != HAL_OK)
  {
    return HAL_ERROR;
  }

  if (URM09_ReadRegisters(0x05U, temperature_buf, 2U) != HAL_OK)
  {
    return HAL_ERROR;
  }

  *distance_cm = (int16_t)((distance_buf[0] << 8) | distance_buf[1]);
  *temperature_x10 = (int16_t)((temperature_buf[0] << 8) | temperature_buf[1]);
  return HAL_OK;
}

static void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
