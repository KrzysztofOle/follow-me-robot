# Mobile Human-Following Robot

Projekt robota mobilnego zdolnego do śledzenia człowieka, z wyraźnym rozdziałem na warstwę niskopoziomową (real-time, sensory, napęd) oraz wysokopoziomową (percepcja, AI, decyzje).

## 1. Cel projektu

- autonomiczny robot kołowy
- śledzenie człowieka w środowisku indoor
- nacisk na deterministykę czasu rzeczywistego
- modularność sprzętu i software'u
- możliwość rozwoju (ROS, fuzja sensorów)

## 2. Architektura systemu

### 2.1 Podział odpowiedzialności

| Warstwa | Element | Odpowiedzialność |
| --- | --- | --- |
| Low-level | STM32 | sensory (I2C), real-time, napęd, bezpieczeństwo |
| High-level | Jetson Nano | wizja, AI, logika śledzenia, fuzja danych |

### 2.2 Komunikacja

- STM32 ↔ Jetson Nano: UART / CAN (planowane)
- STM32 ↔ czujniki: I2C

## 3. Sprzęt

### 3.1 Mikrokontrolery (MCU)

#### Aktualnie (bring-up / testy)

- `NUCLEO-F103RB`
- MCU: `STM32F103RBT6`
- CPU: ARM Cortex-M3 @ 72 MHz
- Flash: 128 KB
- RAM: 20 KB
- UART: 3x
- I2C: 2x
- SPI: 2x

#### Docelowo

- `NUCLEO-H755ZI-Q`
- MCU: `STM32H755ZIT6`
- rdzenie: Cortex-M7 + Cortex-M4

### 3.2 Jednostka AI

- `NVIDIA Jetson Nano Dev Kit B01` (do potwierdzenia)
- CPU: Quad-core ARM Cortex-A57
- GPU: Maxwell, 128 CUDA cores
- RAM: 4 GB
- interfejsy: 2x CSI, HDMI, DP, GbE

### 3.3 Czujniki

- ultradźwiękowe: 6 x URM09 (I2C)
- ToF: 2 x VL53L8
- planowane: kamera RGB / depth
- IMU

### 3.4 Napęd

- VESC / AESC
- Autoro Dual V6 (AESC / VESC 6.7)

## 4. Firmware STM32

### 4.1 Organizacja projektu

- środowisko: PlatformIO + VS Code
- testy podzielone na niezależne aplikacje (`src/tests/...`)

### 4.2 Testy peryferiów

#### UART

- env: `uart_led`
- USART2: PA2 (TX), PA3 (RX - niewykorzystywany)
- 115200 8N1
- test: polling rejestrów + LED

#### LED (NUCLEO-F103RB)

- LED4 -> PC5
- LED3 -> PC8
- LED2 -> PC9
- LED1 -> PA5

#### I2C - skan magistrali

- env: `i2c_scan`
- I2C1: PB8 (SCL), PB9 (SDA)

#### LCD Grove

- env: `grove_lcd`
- adres LCD: `0x3E`
- I2C1: PB8 / PB9

#### LCD + URM09

- env: `grove_lcd_sensor`
- LCD: `0x3E`
- URM09: `0x13`
- pomiar dystansu + temperatury
- I2C 400 kHz

#### LCD + 2x URM09

- env: `grove_lcd_dual_urm09`
- URM09: `0x11`, `0x13`

#### URM09 - szybki log

- env: `urm09_fast_log`
- log dystansu przez USART2
- skan I2C na starcie
- symulacja wielu czujników

#### Zmiana adresu URM09

- env: `urm09_addr_tool`
- automatyczne wykrywanie pojedynczego czujnika
- procedura potwierdzenia po power-cycle

### 4.3 Workflow VS Code

- Taski: Build
- Upload
- Monitor
- Build + Upload + Monitor
- Debug dostępny z poziomu Run and Debug

## 5. Software - Jetson Nano

Planowany stack:

- OS: Ubuntu + JetPack 4.6.x
- robotyka: ROS (docelowo)
- wizja: OpenCV
- YOLO / MediaPipe (light models)

## 6. Algorytmy śledzenia człowieka

Podejście:

- brak pojedynczego czujnika -> fuzja sensorów

Warstwy:

- kamera -> detekcja człowieka (AI)
- LiDAR / ToF / Ultrasonic -> odległość i bezpieczeństwo
- IMU -> stabilizacja ruchu

## 7. Roadmapa

### MVP

- stabilna komunikacja STM32 ↔ Jetson
- śledzenie człowieka kamerą
- podstawowe unikanie kolizji

### Kolejne etapy

- ROS
- fuzja sensorów (EKF / Kalman)
- rozpoznawanie konkretnej osoby

## 8. Notatki badawcze

- fuzja sensorów w robotach mobilnych
- śledzenie człowieka bez markerów
- stabilizacja sterowania (PID, filtracja)

## 9. Status

Projekt w fazie aktywnych testów sprzętowych i architektury systemu.
