# ARCHITECTURE.md

Dokument opisuje **architekturę systemową** robota mobilnego rozwijanego etapowo: od sterowania RC w Etapie 1 do późniejszego śledzenia człowieka. Uzupełnia README o **konkretne decyzje inżynierskie**, podział odpowiedzialności oraz interfejsy między komponentami.

---

## 1. Przegląd systemu

System składa się z dwóch głównych domen:

- **Low-level (real-time)** – sterowanie, sensoryka, bezpieczeństwo, interpretacja RC
- **High-level (AI / decyzyjna)** – percepcja, śledzenie, planowanie ruchu

```
[ Aparatura RC ]
     |
     v
[ Odbiornik RC ] --> [ STM32 (RT) ] --> [ VESC / AESC ]
                         |  \
                         |   \---- Ethernet ----> [ Jetson Nano ] <-- Kamera
                         |
                         +---- [ Czujniki ]
```

---

## 2. Podział odpowiedzialności

### 2.1 STM32 – warstwa niskopoziomowa

**Rola:** deterministyczna kontrola robota

Zakres odpowiedzialności:

- odbiór i interpretacja sygnału RC (PWM per-channel)
- obsługa czujników:
  - URM09 (I2C)
  - VL53L8 (SPI)
  - IMU
- agregacja i filtracja surowych danych
- generowanie komend prędkości (twist)
- obsługa napędu (VESC / AESC)
- obsługa stanów awaryjnych:
  - watchdog
  - zatrzymanie bezpieczeństwa
  - override sygnału RC przez warstwę safety

**Pętla sterowania Etapu 1:**

1. odczyt sygnału RC (kanały)
2. mapowanie sygnału (throttle / steering)
3. odczyt czujników przeszkód
4. decyzja bezpieczeństwa (override RC)
5. wysłanie komendy do ESC

**Cechy:**

- brak alokacji dynamicznej (docelowo)
- stałe częstotliwości pętli
- możliwość pracy bez Jetsona (tryb awaryjny)

---

### 2.2 Jetson Nano – warstwa wysokopoziomowa

**Rola:** percepcja i decyzje w późniejszych etapach

Zakres odpowiedzialności:

- przetwarzanie obrazu (OpenCV)
- detekcja człowieka (YOLO / MediaPipe)
- estymacja położenia celu
- logika śledzenia człowieka
- (docelowo) ROS i fuzja sensorów

**Cechy:**

- brak hard real-time
- możliwość restartu bez utraty kontroli napędu
- wysoki pobór mocy, niski priorytet bezpieczeństwa

---

## 3. Podział na rdzenie (STM32H755 – docelowo)

### 3.1 Cortex-M7

- główna pętla sterowania ruchem
- komunikacja Ethernet z Jetsonem
- obliczenia filtrów (np. Kalman / LPF)

### 3.2 Cortex-M4

- obsługa czujników
- zbieranie danych I2C / SPI
- obsługa IMU

Komunikacja M7 ↔ M4:

- message queue / shared memory

---

## 4. Architektura komunikacji – finalna

### 4.1 Debug (ST-LINK)

| Interfejs | Piny               | Połączenie          | Uwagi                                 |
| --------- | ------------------ | ------------------- | ------------------------------------- |
| USART3    | PD8 → TX, PD9 → RX | ST-LINK Virtual COM | Nie używać tych pinów do innych celów |

### 4.2 ESC (2x sterownik silnika - UART)

#### ESC #1

| Interfejs | Piny               | Poziomy logiczne | Uwagi                 |
| --------- | ------------------ | ---------------- | --------------------- |
| USART1    | PB6 → TX, PB7 → RX | 3.3V             | Sterowanie niezależne |

#### ESC #2

| Interfejs | Piny               | Poziomy logiczne | Uwagi                 |
| --------- | ------------------ | ---------------- | --------------------- |
| USART2    | PD5 → TX, PD6 → RX | 3.3V             | Sterowanie niezależne |

- oba ESC sterowane niezależnie
- wymagane wspólne GND
- poziomy logiczne: 3.3V

### 4.3 Jetson

| Interfejs | Przeznaczenie              | Uwagi                                    |
| --------- | -------------------------- | ---------------------------------------- |
| Ethernet  | komunikacja STM32 ↔ Jetson | Brak użycia UART do komunikacji z Jetson |

### 4.4 RC receiver (Etap 1)

| Interfejs         | Przeznaczenie     | Uwagi                                      |
| ----------------- | ----------------- | ------------------------------------------ |
| PWM (per-channel) | odbiór kanałów RC | domyślny tryb dla FlySky FS-BS6            |
| PPM               | alternatywnie     | wspierane, jeśli użyty jest inny odbiornik |

- CH1 – skręt
- CH2 – prędkość
- CH3 – tryb pracy
- CH4 – E-STOP
- brak autonomii w Etapie 1

### RC Channel Mapping

| Channel | Function  | Range        | Notes      |
|---------|-----------|--------------|------------|
| CH1     | Steering  | 1000–2000 µs | skręt      |
| CH2     | Throttle  | 1000–2000 µs | przód/tył  |
| CH3     | Mode      | discrete     | tryb pracy |
| CH4     | E-STOP    | binary       | opcjonalne |

### Control Priority

1. E-STOP
2. STOP_OBSTACLE
3. LIMIT_SPEED
4. MANUAL_RC

Każdy wyższy poziom nadpisuje niższy.

---

## 5. Wymagania elektryczne UART

- wspólna masa dla STM32 i wszystkich urządzeń UART
- poziomy logiczne 3.3V
- zalecane krótkie przewody
- dla stabilności połączeń zalecana skrętka dla par TX/RX tam, gdzie to możliwe

---

## 6. Ograniczenia sprzętowe NUCLEO

- USART3 (PD8/PD9) domyślnie zajęty przez ST-LINK
- zwolnienie wymaga przecięcia SB16/SB17, opcjonalnie, ale niezalecane
- SPI1 współdzielony z Arduino header
- SDMMC nieużywany, bo brak pełnego wyprowadzenia na złącza
- zamiast SDMMC zalecany SPI

---

## 7. Decyzje projektowe

- UART dla ESC zamiast CAN na etapie 1
- RC receiver jako główne źródło komend w Etapie 1
- Ethernet zamiast UART dla Jetson
- zachowanie USART3 dla debugowania

---

## 8. Częstotliwości pracy (docelowe)

| Moduł                      | Częstotliwość |
|----------------------------|---------------|
| I2C sensory                | 50–100 Hz     |
| Pętla sterowania STM32     | 200–500 Hz    |
| Kamera / AI                | 15–30 FPS     |
| Komunikacja STM32 ↔ Jetson | 50–100 Hz     |

---

## 9. Tryby pracy systemu

1. **Boot standalone** – STM32 działa sam
2. **MANUAL_RC** – sterowanie z aparatury RC
3. **LIMIT_SPEED** – ograniczenie prędkości przez przeszkodę
4. **STOP_OBSTACLE** – zatrzymanie przez przeszkodę
5. **LOST_SIGNAL** – utrata sygnału RC
6. **FOLLOW_VISION** – śledzenie człowieka w późniejszych etapach
7. **SAFE_STOP** – wykryta anomalia
8. **Debug mode** – logowanie pełne

---

## 10. Założenia projektowe

- bezpieczeństwo > funkcjonalność
- STM32 nigdy nie ufa Jetsonowi w 100%
- obstacle avoidance ma wyższy priorytet niż komendy RC
- RC failsafe przechodzi w SAFE_STOP
- komunikacja odporna na utratę pakietów
- możliwość stopniowej migracji do ROS

## 10.1 Ograniczenia Etapu 1

- brak autonomii
- zależność od operatora
- brak podążania za użytkownikiem
- możliwe niewielkie opóźnienia RC
- konieczny failsafe

---

## 11. Plany rozwoju architektury

- ROS 2 jako warstwa logiczna
- EKF dla fuzji sensorów
- oddzielny Safety MCU (opcjonalnie)

---

*Ten dokument jest żywy i będzie rozwijany wraz z projektem.*
