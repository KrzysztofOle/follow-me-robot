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
- komunikacja z Jetson Nano przez Ethernet
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
├─ boards/
├─ include/
├─ src/
└─ test/
```

---

## 4. Komunikacja z Jetson Nano

Komunikacja realizowana jest zgodnie z dokumentami:

- `../docs/ARCHITECTURE.md`
- `../docs/PROTOCOL.md`

Zasady:

- model publish / subscribe
- brak ważnej komendy → STOP
- STM32 ma zawsze priorytet bezpieczeństwa
- interfejs fizyczny: Ethernet

---

## 5. Dokumentacja powiązana

- `../docs/ARCHITECTURE.md`
- `../docs/PROTOCOL.md`
- `../docs/power_architecture.md`

---

## 6. Bring-up na NUCLEO-H755ZI-Q

Ten katalog zawiera gotowy projekt PlatformIO dla testu uruchomienia płytki:

- board: `nucleo_h755zi_q_cm7`
- board: `nucleo_h755zi_q_cm4`
- framework: `stm32cube`
- debug / upload: `ST-LINK`
- tryb testowy: standardowe miganie LED przez HAL
- zegar systemowy: `SystemClock_Config()` z HSI
- test LED: `PB0` (`LD1`) dla CM7
- test LED: `PE1` (`LD2`) dla CM4
- tempo migania: CM7 szybciej, CM4 wolniej

W VS Code użyj:

- `H755 CM7: Build + Upload + Monitor`
- `H755 CM4: Build + Upload + Monitor`
- `H755 Dual-Core: Build + Upload CM7 + CM4`
- `PIO Debug H755 CM7`
- `PIO Debug H755 CM4`

Wymagane narzędzia:

- rozszerzenie `PlatformIO IDE`
- dostęp do `~/.platformio/penv/bin/pio`

Uwagi:

- CM7 i CM4 mają osobne obrazy i osobne uploady.
- miganie realizowane jest klasycznie przez `HAL_GPIO_TogglePin()` i `HAL_Delay()`
- CM7 używa krótszego okresu niż CM4, żeby łatwo odróżnić rdzenie po LED
- Nie uruchamiaj uploadu obu rdzeni równolegle na tym samym ST-LINK.
- Jeśli nie widzisz migania, najpierw sprawdź `LD1` i `LD2`.

Na tej maszynie płytka jest widoczna jako:

- `/dev/cu.usbmodem24203`
- `/dev/tty.usbmodem24203`

---

## 7. Status

Firmware STM32H755 znajduje się w fazie **projektowania architektury i przygotowania implementacji produkcyjnej**.
