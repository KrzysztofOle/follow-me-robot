#include "stm32f1xx_hal.h"
#include <stdio.h>

I2C_HandleTypeDef hi2c1;

#define LCD_ADDR 0x3EU
#define URM09_LEFT_ADDR 0x11U
#define URM09_RIGHT_ADDR 0x13U

static void MX_I2C1_Init(void);
static void Error_Handler(void);
static void LCD_WriteCommand(uint8_t cmd);
static void LCD_WriteData(uint8_t data);
static void LCD_Init(void);
static void LCD_Clear(void);
static void LCD_SetCursor(uint8_t row, uint8_t col);
static HAL_StatusTypeDef LCD_WriteRaw(uint8_t control, const uint8_t *data, uint16_t len);
static void LCD_PrintPadded(const char *text, uint8_t width);
static void LCD_ShowSensorLine(uint8_t row, uint8_t addr, int16_t distance_cm, int16_t temperature_x10);
static void LCD_ShowErrorLine(uint8_t row, uint8_t addr, const char *status);
static HAL_StatusTypeDef URM09_WriteRegister(uint8_t addr, uint8_t reg, uint8_t value);
static HAL_StatusTypeDef URM09_ReadRegisters(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len);
static HAL_StatusTypeDef URM09_SetModeRange(uint8_t addr, uint8_t range, uint8_t mode);
static HAL_StatusTypeDef URM09_Measurement(uint8_t addr);
static HAL_StatusTypeDef URM09_ReadDistanceTemp(uint8_t addr, int16_t *distance_cm, int16_t *temperature_x10);

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
  uint8_t left_ready = 0U;
  uint8_t right_ready = 0U;

  HAL_Init();
  MX_I2C1_Init();

  LCD_Init();
  LCD_Clear();
  LCD_SetCursor(0U, 0U);
  LCD_PrintPadded("Dual URM09 LCD", 16U);
  LCD_SetCursor(1U, 0U);
  LCD_PrintPadded("Init...", 16U);
  HAL_Delay(700U);

  if (URM09_SetModeRange(URM09_LEFT_ADDR, 0x20U, 0x00U) != HAL_OK ||
      URM09_SetModeRange(URM09_RIGHT_ADDR, 0x20U, 0x00U) != HAL_OK)
  {
    Error_Handler();
  }

  LCD_Clear();

  while (1)
  {
    int16_t left_distance = 0;
    int16_t left_temperature = 0;
    int16_t right_distance = 0;
    int16_t right_temperature = 0;

    left_ready = (URM09_Measurement(URM09_LEFT_ADDR) == HAL_OK) ? 1U : 0U;
    right_ready = (URM09_Measurement(URM09_RIGHT_ADDR) == HAL_OK) ? 1U : 0U;

    HAL_Delay(120U);

    if (left_ready != 0U)
    {
      left_ready = (URM09_ReadDistanceTemp(URM09_LEFT_ADDR, &left_distance, &left_temperature) == HAL_OK) ? 1U : 0U;
    }

    if (right_ready != 0U)
    {
      right_ready = (URM09_ReadDistanceTemp(URM09_RIGHT_ADDR, &right_distance, &right_temperature) == HAL_OK) ? 1U : 0U;
    }

    if (left_ready != 0U)
    {
      LCD_ShowSensorLine(0U, URM09_LEFT_ADDR, left_distance, left_temperature);
    }
    else
    {
      LCD_ShowErrorLine(0U, URM09_LEFT_ADDR, "ERR");
    }

    if (right_ready != 0U)
    {
      LCD_ShowSensorLine(1U, URM09_RIGHT_ADDR, right_distance, right_temperature);
    }
    else
    {
      LCD_ShowErrorLine(1U, URM09_RIGHT_ADDR, "ERR");
    }

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

  return HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)(LCD_ADDR << 1), buffer, (uint16_t)(len + 1U), 100U);
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

static void LCD_ShowSensorLine(uint8_t row, uint8_t addr, int16_t distance_cm, int16_t temperature_x10)
{
  char line[17];
  uint16_t distance_display = (distance_cm < 0) ? 0U : (distance_cm > 999) ? 999U : (uint16_t)distance_cm;
  uint16_t temp_abs = (temperature_x10 < 0) ? (uint16_t)(-temperature_x10) : (uint16_t)temperature_x10;
  uint16_t temp_whole = (uint16_t)(temp_abs / 10U);
  uint16_t temp_frac = (uint16_t)(temp_abs % 10U);
  char temp_sign = (temperature_x10 < 0) ? '-' : '+';

  if (temp_whole > 99U)
  {
    temp_whole = 99U;
  }

  (void)snprintf(line, sizeof(line), "%02X D%03u T%c%02u.%1u", (unsigned)addr, (unsigned)distance_display, temp_sign, (unsigned)temp_whole, (unsigned)temp_frac);
  LCD_SetCursor(row, 0U);
  LCD_PrintPadded(line, 16U);
}

static void LCD_ShowErrorLine(uint8_t row, uint8_t addr, const char *status)
{
  char line[17];

  (void)snprintf(line, sizeof(line), "%02X %s", (unsigned)addr, status);
  LCD_SetCursor(row, 0U);
  LCD_PrintPadded(line, 16U);
}

static HAL_StatusTypeDef URM09_WriteRegister(uint8_t addr, uint8_t reg, uint8_t value)
{
  return HAL_I2C_Mem_Write(&hi2c1, (uint16_t)(addr << 1), reg, I2C_MEMADD_SIZE_8BIT, &value, 1U, 100U);
}

static HAL_StatusTypeDef URM09_ReadRegisters(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len)
{
  return HAL_I2C_Mem_Read(&hi2c1, (uint16_t)(addr << 1), reg, I2C_MEMADD_SIZE_8BIT, data, len, 100U);
}

static HAL_StatusTypeDef URM09_SetModeRange(uint8_t addr, uint8_t range, uint8_t mode)
{
  return URM09_WriteRegister(addr, 0x07U, (uint8_t)(range | mode));
}

static HAL_StatusTypeDef URM09_Measurement(uint8_t addr)
{
  return URM09_WriteRegister(addr, 0x08U, 0x01U);
}

static HAL_StatusTypeDef URM09_ReadDistanceTemp(uint8_t addr, int16_t *distance_cm, int16_t *temperature_x10)
{
  uint8_t distance_buf[2] = {0};
  uint8_t temperature_buf[2] = {0};

  if (URM09_ReadRegisters(addr, 0x03U, distance_buf, 2U) != HAL_OK)
  {
    return HAL_ERROR;
  }

  if (URM09_ReadRegisters(addr, 0x05U, temperature_buf, 2U) != HAL_OK)
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
