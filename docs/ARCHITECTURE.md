# ARCHITECTURE.md

Dokument opisuje **architekturę systemową** robota mobilnego śledzącego człowieka. Uzupełnia README o **konkretne decyzje inżynierskie**, podział odpowiedzialności oraz interfejsy między komponentami.

---

## 1. Przegląd systemu

System składa się z dwóch głównych domen:
- **Low-level (real-time)** – sterowanie, sensoryka, bezpieczeństwo
- **High-level (AI / decyzyjna)** – percepcja, śledzenie, planowanie ruchu

```
[ Czujniki ]
     |
     v
[ STM32 (RT) ] <----UART/CAN----> [ Jetson Nano ] <-- Kamera
     |
     v
[ VESC / AESC ]
```

---

## 2. Podział odpowiedzialności

### 2.1 STM32 – warstwa niskopoziomowa

**Rola:** deterministyczna kontrola robota

Zakres odpowiedzialności:
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

**Cechy:**
- brak alokacji dynamicznej (docelowo)
- stałe częstotliwości pętli
- możliwość pracy bez Jetsona (tryb awaryjny)

---

### 2.2 Jetson Nano – warstwa wysokopoziomowa

**Rola:** percepcja i decyzje

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
- komunikacja z Jetsonem
- obliczenia filtrów (np. Kalman / LPF)

### 3.2 Cortex-M4
- obsługa czujników
- zbieranie danych I2C / SPI
- obsługa IMU

Komunikacja M7 ↔ M4:
- message queue / shared memory

---

## 4. Interfejs STM32 ↔ Jetson

### 4.1 Kanał komunikacji

- **UART** – MVP
- **CAN** – docelowo (lepsza odporność)

### 4.2 Kierunek danych

**STM32 → Jetson**
- dystanse z URM09
- macierze ToF (zagregowane)
- prędkość i stan robota

**Jetson → STM32**
- zadany kierunek ruchu
- punkt celu
- tryb pracy robota

---

## 5. Częstotliwości pracy (docelowe)

| Moduł | Częstotliwość |
|-----|---------------|
| I2C sensory | 50–100 Hz |
| Pętla sterowania STM32 | 200–500 Hz |
| Kamera / AI | 15–30 FPS |
| Komunikacja STM32 ↔ Jetson | 50–100 Hz |

---

## 6. Tryby pracy systemu

1. **Boot standalone** – STM32 działa sam
2. **Follow mode** – Jetson steruje celem
3. **Safe stop** – wykryta anomalia
4. **Debug mode** – logowanie pełne

---

## 7. Założenia projektowe

- bezpieczeństwo > funkcjonalność
- STM32 nigdy nie ufa Jetsonowi w 100%
- komunikacja odporna na utratę pakietów
- możliwość stopniowej migracji do ROS

---

## 8. Plany rozwoju architektury

- ROS 2 jako warstwa logiczna
- DDS zamiast UART
- EKF dla fuzji sensorów
- oddzielny Safety MCU (opcjonalnie)

---

*Ten dokument jest żywy i będzie rozwijany wraz z projektem.*
