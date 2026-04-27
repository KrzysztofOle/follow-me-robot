# STM32H755ZI / NUCLEO-H755ZI-Q - wykorzystanie pinow na złączach CN7-CN10

Zakres dokumentu:

- dotyczy tylko złącz CN7, CN8, CN9 i CN10 na NUCLEO-H755ZI-Q
- mapowanie pinow MCU oparto na datasheet STM32H755xI
- mapowanie na złącza oparto na UM2408 dla NUCLEO-H745ZI-Q / NUCLEO-H755ZI-Q

Obudowa MCU: LQFP144

## Podsumowanie

Projekt wykorzystuje te klasy interfejsow:

- 3 x UART
- 2 x SPI
- 2 x I2C
- SDMMC1
- CAN1
- 4 kanaly PWM / RC
- kilka linii GPIO / INT

## UART

| Interfejs | Pin MCU   | Złącze NUCLEO | Funkcja w datasheet                            | Status                   |
|-----------|-----------|---------------|------------------------------------ -----------|--------------------------|
| USART_A   | PB6 / PB7 | CN10 D1 / D0  | LPUART1_TX / LPUART1_RX, USART1_TX / USART1_RX | używany  J1   UART_ESC_1 |
| USART_B   | PD5 / PD6 | CN9 D53 / D52 | USART2_TX / USART2_RX                          | używany  J1   UART_ESC_1 |
| USART_C   | PC6 / PC7 | CN7 D16 / D21 | USART6_TX / USART6_RX                          | używany     DFPLAYER     |

## SPI

| Interfejs | SCK  | MISO | MOSI | CS / NSS | Złącze NUCLEO                          | Funkcja w datasheet          | Status  |
|-----------|------|------|------|----------|----------------------------------------|------------------------------|---------|
| SPI_A     | PA5  | PA6  | PB5  | PD14     | CN7 D13 / D12 / D11 / D10              | SPI1_SCK / MISO / MOSI / NSS | używany |
| SPI_B     | PB10 | PB14 | PB15 | PB12     | CN10 D36 / CN9 D65 / CN7 D17 / CN7 D19 | SPI2_SCK / MISO / MOSI / NSS | używany |

Uwagi:

- SPI_B zostal przeniesiony na SPI2, zeby uniknac konfliktu z SPI_A.
- Zestaw PB10 / PB14 / PB15 / PB12 nie dzieli pinow z SPI_A i jest wyprowadzony na CN10, CN9 oraz CN7.

## I2C

| Magistrala | SCL  | SDA  | Złącze NUCLEO | Funkcja w datasheet | Status  |
|------------|------|------|---------------|---------------------|---------|
| I2C_A      | PB8  | PB9  | CN7 D15 / D14 | I2C1_SCL / I2C1_SDA | używany |
| I2C_B      | PF14 | PF15 | CN9 D69 / D68 | I2C2_SCL / I2C2_SDA | używany |

## SDMMC

| Sygnał | Pin MCU | Złącze NUCLEO | Funkcja w datasheet |
|--------|---------|---------------|---------------------|
| D0     | PC8     | CN8 D43       | SDMMC1_D0           |
| D1     | PC9     | CN8 D44       | SDMMC1_D1           |
| D2     | PC10    | CN8 D45       | SDMMC1_D2           |
| D3     | PC11    | CN8 D46       | SDMMC1_D3           |
| CK     | PC12    | CN8 D47       | SDMMC1_CK           |
| CMD    | PD2     | CN8 D48       | SDMMC1_CMD          |
| CD     | PG10    | CN8 D14       | SDMMC1_CD           |

## CAN

| Interfejs | RX  | TX  | Złącze NUCLEO | Funkcja w datasheet   | Status  |
|-----------|-----|-----|---------------|-----------------------|---------|
| CAN_1     | PD0 | PD1 | CN9 D67 / D66 | FDCAN1_RX / FDCAN1_TX | używany |

## RC / PWM

| Kanal  | Pin MCU | Złącze NUCLEO | Funkcja w datasheet |
|--------|---------|---------------|---------------------|
| RC_CH1 | PA8     | CN10 D6       | TIM1_CH1            |
| RC_CH2 | PE11    | CN10 D5       | TIM1_CH2            |
| RC_CH3 | PE13    | CN10 D3       | TIM1_CH3            |
| RC_CH4 | PA0     | CN10 D32      | TIM2_CH1            |

## GPIO / INT

| Sygnał      | Pin MCU | Złącze NUCLEO | Linia EXTI | Funkcja w datasheet                  | Uwagi                                    |
|-------------|---------|---------------|------------|--------------------------------------|------------------------------------------|
| INT_A       | PD15    | CN7 D9        | EXTI15     | Sygnał przerwania z czujnika VL53L8  | Złącze J5 nakładki                       |
| INT_B       | PE14    | CN10 D4       | EXTI14     | Sygnał przerwania z czujnika VL53L8  | Złącze J6 nakładki                       |
| INT_A2      | PE6     | CN10 D38      | EXTI6      | Opcionalny sygnał przerwania         | Złącze J3 nakładki                       |
| INT_B2      | PE12    | CN10 D39      | EXTI12     | Opcionalny sygnał przerwania         | Złącze J4 nakładki                       |
| SPI_I2C_N_A | PG12    | CN10 D2       |            | Tryb komunikacji czujnika VL53L8     | Złącze J5 nakładki                       |
| SPI_I2C_N_B | PA15    | CN7 D9        |            | Tryb komunikacji czujnika VL53L8     | Złącze J6 nakładki                       |
| LPn_A       | PG9     | CN10 D20      |            | Reset / wyłączanie czujnika VL53L8   | Złącze J5 nakładki                       |
| LPn_B       | PB3     | CN7 D15       |            | Reset / wyłączanie czujnika VL53L8   | Złącze J6 nakładki                       |

Uwagi do przerwan:

- EXTI jest mapowane po numerze pinu, nie po porcie.
- Piny z tym samym numerem EXTI nie moga obslugiwac niezaleznych przerwan jednoczesnie.
- Dlatego w tej tabeli kazdy sygnal INT ma osobna linie EXTI.

## Zgodnosc z dokumentacja producentow

- `PB6 / PB7` sa poprawnym wyborem dla `USART1` i rownoczesnie sa wyprowadzone na CN10 jako `D1 / D0`.
- `PD5 / PD6` sa poprawnym wyborem dla `USART2` i sa wyprowadzone na CN9 jako `D53 / D52`.
- `PC6 / PC7` sa poprawnym wyborem dla `USART6` i sa wyprowadzone na CN7 jako `D16 / D21`.
- `PA5 / PA6 / PB5 / PD14` odpowiadaja `SPI1` na CN7.
- `PB10 / PB14 / PB15 / PB12` odpowiadaja `SPI2` na CN10, CN9 i CN7.
- `PB8 / PB9` odpowiadaja `I2C1` na CN7.
- `PF14 / PF15` odpowiadaja `I2C2` na CN9.
- `PC8 / PC9 / PC10 / PC11 / PC12 / PD2` odpowiadaja `SDMMC1` na CN8.
- `PD0 / PD1` odpowiadaja `FDCAN1` na CN9.
- `PE6`, `PE14` i `PE12` sa wolnymi liniami GPIO na CN10 i nadaja sie na dodatkowe INT.

## Dostepne dodatkowe UART

Rekomendowane opcje:

| UART   | TX  | RX  | Złącze NUCLEO | Uwagi                                                            |
|--------|-----|-----|---------------|------------------------------------------------------------------|
| USART6 | PC6 | PC7 | CN7           | Dobra alternatywa, ale pin jest wspoldzielony z innymi funkcjami |

## Uwagi projektowe

- Dokument zaklada tylko złącza CN7-CN10, bez morpha CN11-CN12.
- Nie uzywam tu pinow morpho jako zrodel dla shielda.
- `UART4` zostal usuniety z rekomendacji, bo pelne mapowanie dla `PA1` wychodzi poza CN7-CN10.
- `CAN` przeniesiono na `PD0 / PD1`, bo to odpowiada board-level `CAN_1` na UM2408.
- `PG9` jest pinem wspoldzielonym; przy dalszej rozbudowie trzeba pilnowac konfliktow z funkcjami domyslnymi NUCLEO.
