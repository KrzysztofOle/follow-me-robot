# 🔌 STM32H755ZIT6 – Wykorzystanie pinów (NUCLEO-ZIO)

Obudowa układu: LQFP144

## 🧠 Podsumowanie

Projekt wykorzystuje kilka kluczowych peryferiów STM32:

- 2 × UART
- 2 × SPI
- 2 × I2C
- SDMMC
- CAN
- wejścia RC (PWM)
- GPIO (INT)

---

## 📡 UART

| Interfejs | TX         | RX         | Status  |
|-----------|------------|------------|---------|
| USART_A   | PB6 (CN10) | PB7 (CN10) | używany |
| USART_B   | PD5  (CN9) | PD6 (CN9)  | używany |

---

## 🔌 SPI

| SPI   | SCK       | MISO      | MOSI       | CS         | Status  |
|-------|-----------|-----------|------------|------------|---------|
| SPI_A | PA5 (CN7) | PA6 (CN7) | PB5  (CN7) | PD14 (CN7) | używany |
| SPI_B | PB3 (CN7) | PB4 (CN7) | PB15 (CN7) | PA4  (NN7) | używany |

---

## 🔗 I2C

| Magistrala | SCL  | SDA  | Status  |
|------------|------|------|---------|
| I2C_A      | PB8  | PB9  | używany |
| I2C_B      | PF14 | PF15 | używany |

---

## 💾 SDMMC

| Sygnał | Pin  |
|--------|------|
| D0     | PC8  |
| D1     | PC9  |
| D2     | PC10 |
| D3     | PC11 |
| CK     | PC12 |
| CMD    | PD2  |

---

## 🚗 CAN

| Sygnał | Pin  |
|--------|------|
| CAN_TX | PB13 |
| CAN_RX | PB12 |

---

## 🎮 RC (PWM)

| Kanał  | Pin         |
|--------|-------------|
| RC_CH1 | PA8  (CN10) |
| RC_CH2 | PE11 (CN10) |
| RC_CH3 | PE13 (CN10) |
| RC_CH4 | PA0  (CN10) |

---

## ⚠️ GPIO / INT

| Sygnał | Pin.       |
|--------|------------|
| INT_A  | PD15 (CN7) |
| INT_B  | PG9  (CN7) |
| INT_A2 | PC6  (CN7) |
| INT_B2 | PB5  (CN9) |

---

## 📊 Wykorzystanie portów

| Port | Status |
|------|--------|
| PA | 🟠 częściowo użyty |
| PB | 🔴 mocno użyty |
| PC | 🔴 SDMMC |
| PD | 🟠 częściowo |
| PE | 🟠 RC |
| PF | 🟢 częściowo użyty |
| PG | 🟢 głównie wolny |

---

## 🟢 Wolne piny (istotne)

### 🔌 CN7

- PB12
- PB13
- PA5
- PA6
- PA7
- PB6
- PB7

### 🔌 CN10

- PA8
- PC6
- PC7
- PG6
- PB6
- PB7
- PE7–PE15 (częściowo)

### 🔌 CN9

- PC0
- PC3
- PB1
- PC2
- PF11
- PE2
- PE4
- PE5
- PE6
- PE3
- PF8
- PF7
- PF9
- PD10

---

## 📡 Dostępne dodatkowe UART

### ✅ Rekomendowane dla DFPlayer Mini

| UART   | TX  | RX  | Lokalizacja | Uwagi              |
|--------|-----|-----|-------------|--------------------|
| USART1 | PB6 | PB7 | CN7 / CN10  | ⭐ najlepszy wybór |
| USART6 | PC6 | PC7 | CN10        | dobra alternatywa  |
| UART4  | PA0 | PA1 | CN10        | opcjonalny         |

---

## 🎯 Rekomendacja (DFPlayer)

👉 Użyj:

```
USART1:
TX → PB6
RX → PB7
```

### Powody:
- wolne piny
- brak konfliktów
- dobre rozmieszczenie
- pełny sprzętowy UART

---

## ⚡ Uwagi projektowe

- Port **PB** jest mocno obciążony → unikać dalszego dokładania
- Porty **PF / PG** mają duży zapas → preferowane do nowych funkcji
- SDMMC używa dedykowanych pinów → poprawnie

---

## 🚀 Wnioski

✔ System ma duży zapas pinów  
✔ Dostępne min. 3 dodatkowe UART  
✔ Możliwość dalszej rozbudowy bez zmian architektury  

---