# NUCLEO-H745ZI-Q and NUCLEO-H755ZI-Q pin assignments

Źródło:

- UM2408 Rev 6, `STM32H7 Nucleo-144 boards (MB1363) - User manual`
- zakres: `CN8`, `CN9`, `CN7`, `CN10`

Kolumny:

- `Pin`
- `Pin name`
- `STM32H7 pin`
- `Project function`

## CN8

| Pin | Pin name | STM32H7 pin | Project function |
| --- | --- | --- | --- |
| 1 | NC | - | - |
| 2 | D43 | PC8 | SDMMC1 D0 |
| 3 | IOREF | - | 3.3 V reference |
| 4 | D44 | PC9 | SDMMC1 D1 |
| 5 | NRST | NRST | Reset |
| 6 | D45 | PC10 | SDMMC1 D2 |
| 7 | 3V3 | - | 3.3 V power |
| 8 | D46 | PC11 | SDMMC1 D3 |
| 9 | 5V | - | 5 V power |
| 10 | D47 | PC12 | SDMMC1 CK |
| 11 | GND | - | GND |
| 12 | D48 | PD2 | SDMMC1 CMD |
| 13 | GND | - | GND |
| 14 | D49 | PG10 | Board power control / I/O |
| 15 | VIN | - | VIN input |
| 16 | D50 | PG8 | Board power control / I/O |

## CN9

| Pin | Pin name | STM32H7 pin | Project function |
| --- | --- | --- | --- |
| 1 | A0 | PA3 | Unused / analog |
| 2 | D51 | PD7 | USART2 SCLK / unused |
| 3 | A1 | PC0 | Unused / analog |
| 4 | D52 | PD6 | ESC_2 UART RX |
| 5 | A2 | PC3_C | Unused / analog |
| 6 | D53 | PD5 | ESC_2 UART TX |
| 7 | A3 | PB1 | Unused / analog |
| 8 | D54 | PD4 | USART2 RTS / unused |
| 9 | A4 | PB9 | PB9 = standard: I2C1 SDA; PC2_C = ADC alternative |
| 10 | D55 | PD3 | USART2 CTS / unused |
| 11 | A5 | PB8 | PB8 = standard: I2C1 SCL; PF11 = ADC alternative |
| 12 | GND | - | GND |
| 13 | D72 | PB2 | Unused / comparator input |
| 14 | D56 | PE2 | Unused / SAI |
| 15 | D71 | PE9 | Unused / comparator input |
| 16 | D57 | PE4 | Unused / SAI |
| 17 | D70 | PB5 | SPI_B MOSI |
| 18 | D58 | PE5 | Unused / SAI |
| 19 | D69 | PF14 | I2C2 SCL |
| 20 | D59 | PE6 | Unused / SAI |
| 21 | D68 | PF15 | I2C2 SDA |
| 22 | D60 | PE3 | Unused / SAI |
| 23 | GND | - | GND |
| 24 | D61 | PF8 | Unused / SAI |
| 25 | D67 | PD0 | CAN1 RX |
| 26 | D62 | PF7 | Unused / SAI |
| 27 | D66 | PD1 | CAN1 TX |
| 28 | D63 | PF9 | Unused / SAI |
| 29 | D65 | PB14 | SPI_B MISO |
| 30 | D64 | PD10 | Board IO / USB_FS_PWR_EN |

## CN7

| Pin | Pin name | STM32H7 pin | Project function |
| --- | --- | --- | --- |
| 1 | D16 | PC6 | Unused / audio |
| 2 | D15 | PB8 | I2C1 SCL |
| 3 | D17 | PB15 | Unused / audio |
| 4 | D14 | PB9 | I2C1 SDA |
| 5 | D18 | PB13 | Unused / audio |
| 6 | AREF | - | Analog reference |
| 7 | D19 | PB12 | Unused / audio |
| 8 | GND | - | GND |
| 9 | D20 | PA15 | Unused / audio |
| 10 | D13 | PA5 | SPI1 SCK |
| 11 | D21 | PC7 | Unused / audio |
| 12 | D12 | PA6 | SPI1 MISO |
| 13 | D22 | PB5 | SPI_B MOSI |
| 14 | D11 | PA7 | SPI1 MOSI |
| 15 | D23 | PB3 | SPI_B SCK |
| 16 | D10 | PD14 | SPI1 NSS / CS |
| 17 | D24 | PA4 | SPI_B NSS |
| 18 | D9 | PD15 | VL53L8CX INT_A |
| 19 | D25 | PB4 | SPI_B MISO |
| 20 | D8 | PG9 | VL53L8CX LPn_A |

## CN10

| Pin | Pin name | STM32H7 pin | Project function |
| --- | --- | --- | --- |
| 1 | VDDA | - | Analog supply |
| 2 | D7 | PG12 | VL53L8CX SPI_I2C_N_A |
| 3 | AGND | - | Analog ground |
| 4 | D6 | PA8 | RC_CH1 / PWM |
| 5 | GND | - | GND |
| 6 | D5 | PE11 | RC_CH2 / PWM |
| 7 | GND | - | GND |
| 8 | D4 | PE14 | VL53L8CX INT_B |
| 9 | PF6 | A6 | Unused / analog |
| 10 | D3 | PE13 | RC_CH3 / PWM |
| 11 | PF10 | A7 | Unused / analog |
| 12 | D2 | PG14 | VL53L8CX SPI_I2C_N_B |
| 13 | PA2 | A8 | Unused / analog |
| 14 | D1 | PB6 | ESC_1 UART TX |
| 15 | PG6 | D26 | Unused / board IO |
| 16 | D0 | PB7 | ESC_1 UART RX |
| 17 | PB2 | D27 | Unused / board IO |
| 18 | D42 | PE8 | Unused |
| 19 | GND | - | GND |
| 20 | D41 | PE7 | Unused |
| 21 | PD13 | D28 | Unused |
| 22 | D40 | PE10 | Unused |
| 23 | PD12 | D29 | Unused |
| 24 | D39 | PE12 | Unused |
| 25 | PD11 | D30 | Unused |
| 26 | D38 | PE6 | VL53L8CX INT_A2 |
| 27 | PE2 | D31 | Unused |
| 28 | D37 | PE15 | Unused |
| 29 | PA0 | D32 | RC_CH4 / PWM |
| 30 | D36 | PB10 | SPI_B SCK |
| 31 | PB0 | D33 | Unused |
| 32 | D35 | PB11 | Unused |
| 33 | PE0 | D34 | Unused |
| 34 | GND | - | GND |
