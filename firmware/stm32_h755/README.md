# Firmware – STM32H755 (Follow‑Me Robot)

Firmware warstwy **low‑level (real‑time)** robota mobilnego *Follow‑Me Robot*,
przeznaczone dla **STM32H755 (Cortex‑M7 + Cortex‑M4)**.

Ten firmware stanowi **docelową wersję produkcyjną**, rozwijaną na bazie
doświadczeń z projektu `stm32_f103_project`.

---

## 1. Rola firmware (STM32H755)

Firmware dla H755 realizuje wszystkie funkcje:

- krytyczne czasowo,
- związane z bezpieczeństwem,
- wymagające deterministycznych pętli sterowania.

STM32H755 działa jako:
> **master sterowania i bezpieczeństwa robota**

Jetson Nano pełni wyłącznie rolę:

- percepcji,
- planowania,
- generowania celów ruchu.

---

## 2. Architektura dual‑core (M7 / M4)

### 2.1 Cortex‑M7 (High‑performance / Control core)

- główna pętla sterowania ruchem
- komunikacja z Jetson Nano (UART / CAN)
- generowanie komend dla VESC / AESC
- nadzór bezpieczeństwa

### 2.2 Cortex‑M4 (Sensor / IO core)

- obsługa czujników (I2C / SPI)
- akwizycja danych
- wstępna filtracja danych

### 2.3 Komunikacja M7 ↔ M4

- shared memory / message queue
- brak alokacji dynamicznej
- deterministyczny dostęp do danych

---

## 3. Struktura projektu

```
stm32_h755_project/
├─ README.md
├─ platformio.ini
├─ include/
│  ├─ common/
│  ├─ protocol/
│  └─ board/
├─ src/
│  ├─ m7/
│  ├─ m4/
│  ├─ comm/
│  └─ safety/
└─ test/
```

---

## 4. Komunikacja z Jetson Nano

Komunikacja realizowana jest zgodnie z dokumentem:

- `../docs/PROTOCOL.md`

Zasady:

- model publish / subscribe
- brak ważnej komendy → STOP
- STM32 ma zawsze priorytet bezpieczeństwa

---

## 5. Dokumentacja powiązana

- `../docs/ARCHITECTURE.md`
- `../docs/PROTOCOL.md`
- `../docs/power_architecture.md`

---

## 6. Status

Firmware STM32H755 znajduje się w fazie **projektowania architektury i przygotowania implementacji produkcyjnej**.
