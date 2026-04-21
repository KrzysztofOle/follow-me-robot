# PROTOCOL.md

Protokół komunikacyjny pomiędzy **STM32 (low-level, real-time)** a **Jetson Nano (high-level, AI)**.
Warstwa transportowa: **Ethernet**.
W Etapie 1 główne sterowanie pochodzi z RC lokalnie przez STM32, a ten dokument opisuje kontrakt dla etapów wyższych i wspólną semantykę trybów pracy.

Celem protokołu jest:
- deterministyczna i odporna komunikacja
- minimalna złożoność (MVP)
- jednoznaczny kontrakt interfejsowy

Dokument stanowi **kontrakt interfejsowy** między firmware STM32 a software Jetson.

---

## 1. Założenia ogólne

- STM32 jest **masterem bezpieczeństwa**
- Jetson **nie steruje bezpośrednio napędem**
- komunikacja ma charakter **cykliczny**
- brak dynamicznej alokacji po stronie STM32

---

## 2. Warstwa fizyczna

### MVP
- interfejs: **Ethernet**
- transport aplikacyjny definiowany po stronie implementacji

### Uwagi
- dokument nie definiuje ramkowania UART
- dokument nie zakłada użycia CAN do komunikacji z Jetson

---

## 3. Transport Ethernet (STM32 ↔ Jetson)

- komunikacja STM32 ↔ Jetson działa po Ethernet
- protokół na tym etapie opisuje wyłącznie kontrakt wiadomości aplikacyjnych
- szczegóły enkapsulacji i stosu sieciowego są po stronie implementacji firmware i software Jetson

---

## 4. Model komunikacji

- model: **request-less publish / subscribe**
- STM32 wysyła stan cyklicznie
- Jetson wysyła komendy cyklicznie

```
STM32  ---- state ---->  Jetson
STM32  <--- command ---  Jetson
```

Brak blokujących zapytań.

---

## 5. Format wiadomości aplikacyjnych

- dokument definiuje typy wiadomości i ich ładunek
- enkapsulacja transportowa zależy od implementacji Ethernet
- `ID` wskazuje typ wiadomości
- pola poniżej opisują payload

---

## 6. Typy wiadomości

### 5.1 STM32 → Jetson (STATE)

#### ID = `0x10` – RobotState

```
uint32_t timestamp_ms
int16_t  vx_mm_s
int16_t  vy_mm_s
int16_t  omega_mrad_s
uint16_t battery_mv
uint8_t  state_flags
```

`state_flags`:
- bit0 – emergency_stop
- bit1 – obstacle_detected
- bit2 – sensor_error

---

### 5.2 STM32 → Jetson – Sensors

#### ID = `0x11` – UltrasonicArray

```
uint8_t  count
uint16_t distance_mm[count]
```

#### ID = `0x12` – ToFGrid (zagregowane)

```
uint8_t rows
uint8_t cols
uint16_t min_distance_mm
uint16_t avg_distance_mm
```

---

### 5.3 Jetson → STM32 (COMMAND)

#### ID = `0x20` – VelocityCommand

```
int16_t vx_mm_s
int16_t vy_mm_s
int16_t omega_mrad_s
uint8_t validity_ms
```

Jeśli komenda nie zostanie odświeżona w `validity_ms` → **STOP**.

---

#### ID = `0x21` – ModeCommand

```
uint8_t mode
```

`mode`:
- 0 – idle
- 1 – manual_rc
- 2 – limit_speed
- 3 – stop_obstacle
- 4 – lost_signal
- 5 – follow_vision
- 6 – emergency_stop

`manual_rc` jest trybem domyślnym Etapu 1, a `follow_vision` pozostaje zarezerwowany dla etapu śledzenia człowieka.

---

## 7. Częstotliwości

| Wiadomość | Częstotliwość |
|----------|---------------|
| RobotState | 50–100 Hz |
| Sensors | 20–50 Hz |
| VelocityCommand | 50 Hz |

---

## 8. Bezpieczeństwo

- brak valid command → STOP
- integrity check fail → discard message
- Jetson silence > 200 ms → STOP
- tryb emergency_stop nadpisuje wszystko

---

## 9. Wersjonowanie

```
uint8_t protocol_version_major
uint8_t protocol_version_minor
```

Zmiana MAJOR = breaking change

---

## 10. Rozszerzenia (plan)

1. Ethernet + custom protocol (MVP)
2. ROS 2 + DDS

---

*Dokument obowiązuje firmware STM32 i software Jetson.*
