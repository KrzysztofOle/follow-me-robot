# VL53L8CX – Dokument uruchomieniowy

## 📌 Opis modułu

VL53L8CX to sensor odległości typu Time-of-Flight (ToF) firmy STMicroelectronics.
Generuje mapę głębi 8x8 (64 strefy), umożliwiając detekcję wielu obiektów.

- Zasięg: do 4 m
- Kąt widzenia: ~65°
- Technologia: FlightSense (pomiar czasu przelotu światła)

---

## ⚙️ Kluczowe parametry

| Parametr          | Wartość                      |
|-------------------|------------------------------|
| Rozdzielczość     | 4x4 lub 8x8                  |
| Maks. zasięg      | 400 cm                       |
| Częstotliwość     | do 60 Hz                     |
| Interfejs         | I2C (1 MHz), SPI (do 20 MHz) |
| Długość fali      | 940 nm (IR, eye-safe)        |
| Temperatura pracy | -30°C do 85°C                |
| Zasilanie         | 3.3V (AVDD), 1.8V core       |

---

## 🔌 Pinout (typowy breakout jak na zdjęciu)

| Pin         | Funkcja            |
|-------------|--------------------|
| VIN         | Zasilanie          |
| GND         | Masa               |
| SDA / MOSI  | Dane I2C / SPI     |
| SCL / SCK   | Zegar              |
| MISO        | SPI                |
| NCS         | Chip Select        |
| INT         | Przerwanie         |
| LPn (XSHUT) | Reset / wyłączanie |
| SPI_I2C_N   | Wybór trybu        |

---

## 🔄 Tryby pracy

### I2C

- prostszy
- wolniejszy
- adres domyślny: **0x52**

### SPI

- szybszy (zalecany do 8x8)
- wymaga CS

---

## 🧠 Funkcje sensora

- pomiar 64 punktów jednocześnie
- detekcja wielu obiektów
- odporność na światło otoczenia
- histogram + kompensacja zakłóceń
- motion detection per strefa

---

## ⚠️ Ważne uwagi sprzętowe

### Zasilanie

- sensor działa na:
  - 3.3V (analog)
  - 1.8V (core)
- breakout często ma regulator → sprawdź!

### INT

- sygnał gotowości danych
- warto używać zamiast polling

### LPn (XSHUT)

- reset sensora
- wymagany przy wielu sensorach

---

## 🧩 Integracja z STM32

### Rekomendacja

- używać SPI
- używać DMA (dużo danych)
- używać przerwania INT

### Minimalny flow

1. Reset (LPn)
2. Init przez ULD
3. Konfiguracja rozdzielczości
4. Start ranging
5. Odczyt danych

---

## 📦 Dane wyjściowe

Sensor zwraca:

- odległość dla każdej strefy
- status pomiaru
- sygnał (signal rate)
- ambient noise

---

## 🚀 Zastosowanie w robocie

VL53L8CX nadaje się do:

- detekcji przeszkód
- mapy otoczenia (mini depth map)
- wykrywania ruchu
- SLAM (w ograniczonym zakresie)

---

## ❗ Ograniczenia

- brak informacji o kierunku (tylko mapa stref)
- wrażliwy na szkło bez kalibracji
- generuje dużo danych → obciążenie MCU

---

## 🔗 Dokumentacja źródłowa

- Datasheet VL53L8CX (ST)
- UM3109 (ULD API)
- SATEL-VL53L8 (referencyjna płytka)

---

## 💬 Notatki projektowe

- dla Twojego robota:
  - SPI + INT = must-have
  - 2 sensory → sterować LPn
  - filtrować dane (median / EMA)
