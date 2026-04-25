#include "stm32h7xx_hal.h"
#if !defined(CORE_CM7) && !defined(CORE_CM4)
#error "This firmware expects CORE_CM7 or CORE_CM4 to be defined by the board."
#endif

#if defined(CORE_CM7)
#define LED_GPIO_PORT GPIOB
#define LED_GPIO_PIN GPIO_PIN_0
#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define BLINK_DELAY_MS 150U
#define BOOT_PULSES 2U
#define BOOT_ON_MS 80U
#define BOOT_OFF_MS 120U
#define WARMUP_TOGGLES 4U
#define COMM_CORE_COUNTER_FIELD cm7_counter
#define COMM_PEER_COUNTER_FIELD cm4_counter
#define COMM_LAST_SEEN_FIELD cm7_seen_cm4
#define COMM_BOOT_COUNT_FIELD cm7_boot_count
#define COMM_STAGE_FIELD cm7_stage
#define COMM_PEER_STAGE_FIELD cm4_stage
#elif defined(CORE_CM4)
#define LED_GPIO_PORT GPIOE
#define LED_GPIO_PIN GPIO_PIN_1
#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()
#define BLINK_DELAY_MS 600U
#define BOOT_PULSES 1U
#define BOOT_ON_MS 120U
#define BOOT_OFF_MS 180U
#define WARMUP_TOGGLES 2U
#define COMM_CORE_COUNTER_FIELD cm4_counter
#define COMM_PEER_COUNTER_FIELD cm7_counter
#define COMM_LAST_SEEN_FIELD cm4_seen_cm7
#define COMM_BOOT_COUNT_FIELD cm4_boot_count
#define COMM_STAGE_FIELD cm4_stage
#define COMM_PEER_STAGE_FIELD cm7_stage
#endif

typedef struct {
  uint32_t magic;
  uint32_t cm7_boot_count;
  uint32_t cm4_boot_count;
  uint32_t cm7_stage;
  uint32_t cm4_stage;
  uint32_t cm7_counter;
  uint32_t cm4_counter;
  uint32_t cm7_seen_cm4;
  uint32_t cm4_seen_cm7;
} shared_comm_t;

#define COMM_MAGIC 0x434F4D48UL
#define COMM_SHARED ((volatile shared_comm_t *)0x30000000UL)
#define COMM_DIAG_ENABLE 0U

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void WarmupPhase(void);
static void SharedComm_Init(void);
static void SharedComm_Tick(void);
static void BootSignature(void);
static void Error_Handler(void);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  BootSignature();
  WarmupPhase();
  SharedComm_Init();

  while (1) {
    SharedComm_Tick();
    HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_GPIO_PIN);
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

static void SharedComm_Init(void) {
#if COMM_DIAG_ENABLE
  if (COMM_SHARED->magic != COMM_MAGIC) {
    COMM_SHARED->magic = COMM_MAGIC;
    COMM_SHARED->cm7_boot_count = 0U;
    COMM_SHARED->cm4_boot_count = 0U;
    COMM_SHARED->cm7_stage = 0U;
    COMM_SHARED->cm4_stage = 0U;
    COMM_SHARED->cm7_counter = 0U;
    COMM_SHARED->cm4_counter = 0U;
    COMM_SHARED->cm7_seen_cm4 = 0U;
    COMM_SHARED->cm4_seen_cm7 = 0U;
  }
  __DMB();
#endif
}

static void BootSignature(void) {
  uint32_t pulse;

#if COMM_DIAG_ENABLE
  COMM_SHARED->COMM_BOOT_COUNT_FIELD += 1U;
  COMM_SHARED->COMM_STAGE_FIELD = 1U;
  __DMB();
#endif

  for (pulse = 0U; pulse < BOOT_PULSES; ++pulse) {
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_SET);
    HAL_Delay(BOOT_ON_MS);
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_RESET);
    HAL_Delay(BOOT_OFF_MS);
  }

#if COMM_DIAG_ENABLE
  COMM_SHARED->COMM_STAGE_FIELD = 2U;
  __DMB();
#endif
}

static void WarmupPhase(void) {
  uint32_t toggle;

  for (toggle = 0U; toggle < WARMUP_TOGGLES; ++toggle) {
    HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_GPIO_PIN);
    HAL_Delay(200U);
  }

  HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_RESET);
#if COMM_DIAG_ENABLE
  COMM_SHARED->COMM_STAGE_FIELD = 2U;
  __DMB();
#endif
}

static void SharedComm_Tick(void) {
#if COMM_DIAG_ENABLE
  uint32_t local_counter;
  uint32_t peer_counter;

  COMM_SHARED->COMM_STAGE_FIELD = 3U;
  local_counter = COMM_SHARED->COMM_CORE_COUNTER_FIELD + 1U;
  COMM_SHARED->COMM_CORE_COUNTER_FIELD = local_counter;
  __DMB();
  peer_counter = COMM_SHARED->COMM_PEER_COUNTER_FIELD;
  COMM_SHARED->COMM_LAST_SEEN_FIELD = peer_counter;
  COMM_SHARED->COMM_STAGE_FIELD = 4U;
  __DMB();
#endif
}

static void Error_Handler(void) {
  __disable_irq();
  while (1) {
  }
}
