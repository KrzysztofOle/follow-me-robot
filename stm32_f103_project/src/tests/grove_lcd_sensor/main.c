#include "stm32f1xx_hal.h"

I2C_HandleTypeDef hi2c1;

static void MX_I2C1_Init(void);
static void Error_Handler(void);
static void LCD_WriteCommand(uint8_t cmd);
static void LCD_WriteData(uint8_t data);
static void LCD_Init(void);
static void LCD_Clear(void);
static void LCD_SetCursor(uint8_t row, uint8_t col);
static void LCD_Print(const char *text);
static HAL_StatusTypeDef LCD_WriteRaw(uint8_t control, const uint8_t *data, uint16_t len);
static void LCD_PrintPadded(const char *text, uint8_t width);
static uint8_t Sensor_Present(void);

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

  while (1)
  {
    uint8_t sensor_ok = Sensor_Present();

    LCD_SetCursor(0U, 0U);
    LCD_Print("Grove LCD OK");
    LCD_PrintPadded("", 16U - 12U);
    LCD_SetCursor(1U, 0U);
    LCD_PrintPadded(sensor_ok != 0U ? "Sensor 0x11 OK" : "Sensor 0x11 ERR", 16U);

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

static void LCD_Print(const char *text)
{
  while (*text != '\0')
  {
    LCD_WriteData((uint8_t)*text++);
  }
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

static uint8_t Sensor_Present(void)
{
  return (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(0x11U << 1), 2U, 20U) == HAL_OK) ? 1U : 0U;
}

static void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
