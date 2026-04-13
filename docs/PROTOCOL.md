# PROTOCOL.md

Protokół komunikacyjny pomiędzy **STM32 (low-level, real-time)** a **Jetson Nano (high-level, AI)**.

Celem protokołu jest:
- deterministyczna i odporna komunikacja
- minimalna złożoność (MVP)
- łatwa migracja UART → CAN → DDS

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
- interfejs: **UART**
- prędkość: `921600 bps` (docelowo), startowo `115200`
- format: `8N1`

### Docelowo
- **CAN 2.0B**
- bitrate: `500 kbps`

---

## 3. Model komunikacji

- model: **request-less publish / subscribe**
- STM32 wysyła stan cyklicznie
- Jetson wysyła komendy cyklicznie

```
STM32  ---- state ---->  Jetson
STM32  <--- command ---  Jetson
```

Brak blokujących zapytań.

---

## 4. Format ramki (UART)

### 4.1 Struktura ramki

```
+--------+------+-----+---------+------+--------+
|  SOF   | LEN  | ID  | PAYLOAD | CRC  |  EOF   |
+--------+------+-----+---------+------+--------+
| 0xAA55 | 1 B  | 1 B |  N B    | 2 B  | 0x55AA |
```

- **SOF** – start of frame
- **LEN** – długość PAYLOAD
- **ID** – typ wiadomości
- **CRC** – CRC-16 (X25)

---

## 5. Typy wiadomości

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
- 1 – follow
- 2 – manual
- 3 – emergency_stop

---

## 6. Częstotliwości

| Wiadomość | Częstotliwość |
|----------|---------------|
| RobotState | 50–100 Hz |
| Sensors | 20–50 Hz |
| VelocityCommand | 50 Hz |

---

## 7. Bezpieczeństwo

- brak valid command → STOP
- CRC fail → discard frame
- Jetson silence > 200 ms → STOP
- tryb emergency_stop nadpisuje wszystko

---

## 8. Wersjonowanie

```
uint8_t protocol_version_major
uint8_t protocol_version_minor
```

Zmiana MAJOR = breaking change

---

## 9. Migracja (plan)

1. UART + custom protocol (MVP)
2. CAN + te same ID
3. ROS 2 + DDS

---

*Dokument obowiązuje firmware STM32 i software Jetson.*
