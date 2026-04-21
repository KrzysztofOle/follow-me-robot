# Firmware – Follow‑Me Robot

Firmware warstwy **low‑level (real‑time)** robota mobilnego *Follow‑Me Robot*.

Odpowiada za:

- deterministyczną obsługę czujników,
- sterowanie napędem,
- logikę bezpieczeństwa,
- komunikację z jednostką wysokopoziomową (Jetson Nano).

Firmware jest projektowany zgodnie z zasadą:
> **bezpieczeństwo i real‑time mają pierwszeństwo przed funkcjonalnością AI**

---

## 1. Rola firmware w systemie

Firmware działa na mikrokontrolerze **STM32** i realizuje funkcje, które:

- **muszą działać zawsze**, nawet bez Jetsona,
- wymagają **ściśle określonych czasów reakcji**,
- nie mogą zależeć od systemu Linux ani AI.

Główne zadania:

- obsługa magistral **I2C / SPI / UART**,
- zbieranie i wstępne przetwarzanie danych z czujników,
- generowanie komend ruchu dla napędu,
- realizacja mechanizmów bezpieczeństwa,
- cykliczna komunikacja z Jetson Nano.

> Uwaga: ten dokument dotyczy projektu bring-up dla **NUCLEO-F103RB**. Nie opisuje finalnego mapowania UART dla **NUCLEO-H755ZI-Q**.

---

## 2. Wspierany hardware

### 2.1 Aktualnie (bring‑up / testy)

- **STM32 NUCLEO‑F103RB**
  - Cortex‑M3 @ 72 MHz
  - 20 KB RAM / 128 KB Flash

### 2.2 Docelowo

- **STM32 NUCLEO‑H755ZI‑Q**
  - Cortex‑M7 + Cortex‑M4
  - wyraźny podział zadań czasowych

---

## 3. Struktura katalogów

```
firmware/
├─ stm32_f103_project/
│  ├─ platformio.ini
│  ├─ include/     # nagłówki, definicje interfejsów
│  ├─ src/         # logika firmware
│  ├─ test/        # testy peryferiów i bring‑up
│  └─ .vscode/
```

---

## 4. Środowisko deweloperskie

- PlatformIO
- VS Code
- kompilator: arm‑none‑eabi‑gcc

---

## 5. Komunikacja z Jetson Nano

Komunikacja odbywa się zgodnie z dokumentem:

- `../docs/PROTOCOL.md`

Zasady:

- model cykliczny (publish / subscribe)
- brak valid command → STOP
- STM32 zawsze ma priorytet bezpieczeństwa

---

## 6. Architektura

Szczegółowa architektura opisana jest w:

- `../docs/ARCHITECTURE.md`

---

## 7. Status

Firmware znajduje się w fazie **aktywnych testów sprzętowych i budowy architektury RT**.
