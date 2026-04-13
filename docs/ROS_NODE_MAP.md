# ROS_NODE_MAP.md

Dokument opisuje **mapowanie funkcjonalne systemu robota *Follow‑Me Robot* na węzły ROS (ROS 2)**.

Celem dokumentu jest:

- jednoznaczne określenie **jakie funkcje realizuje ROS**,
- jasno odseparować **ROS (high‑level)** od **firmware STM32 (real‑time, safety)**,
- przygotować projekt na skalowanie i dalszy rozwój.

Dokument dotyczy **warstwy Jetson Nano (Linux + ROS 2)**.

---

## 1. Zasady nadrzędne

1. ROS **nie realizuje funkcji safety‑critical**.
2. ROS **nigdy nie steruje bezpośrednio napędem**.
3. STM32 jest masterem sterowania i bezpieczeństwa.
4. ROS publikuje **intencję ruchu**, nie komendy niskopoziomowe.
5. Każdy węzeł ROS ma **jednoznaczną odpowiedzialność**.

---

## 2. Warstwy logiczne ROS

ROS w projekcie dzieli się na cztery warstwy:

1. **Perception** – widzenie i sensory high‑level
2. **Tracking** – śledzenie człowieka
3. **Control / Decision** – decyzje ruchu
4. **Interface** – komunikacja z STM32

---

## 3. Lista węzłów ROS (Node Map)

### 3.1 perception_camera_node

**Rola:**

- obsługa kamery RGB / depth

**Publikowane topic‑y:**

- `/camera/image_raw`
- `/camera/camera_info`

**Zależności:**

- sterowniki kamery
- OpenCV

---

### 3.2 perception_human_detector_node

**Rola:**

- detekcja sylwetki człowieka (YOLO / MediaPipe)

**Subskrybuje:**

- `/camera/image_raw`

**Publikuje:**

- `/human/detections`

---

### 3.3 tracking_node

**Rola:**

- wybór celu (człowiek do śledzenia)
- estymacja pozycji celu względem robota

**Subskrybuje:**

- `/human/detections`

**Publikuje:**

- `/target/pose`

---

### 3.4 motion_decision_node

**Rola:**

- generowanie intencji ruchu robota
- logika follow‑me

**Subskrybuje:**

- `/target/pose`

**Publikuje:**

- `/motion/intent`

> Uwaga: ten node **nie zna napędu ani PWM / RPM**.

---

### 3.5 ros_protocol_bridge_node

**Rola:**

- most ROS ↔ STM32
- implementacja `PROTOCOL.md`

**Subskrybuje:**

- `/motion/intent`

**Publikuje:**

- `/robot/state`

**Komunikacja fizyczna:**

- UART (MVP)
- CAN (docelowo)

> Jest to **jedyny węzeł**, który komunikuje się ze STM32.

---

## 4. Węzły diagnostyczne i debug

### 4.1 robot_state_monitor_node

**Rola:**

- monitoring stanu robota
- wizualizacja w RViz

**Subskrybuje:**

- `/robot/state`

---

### 4.2 safety_monitor_node (opcjonalny)

**Rola:**

- obserwacja flag safety
- wymuszanie SAFE_STOP po stronie ROS (bez hard‑cut)

**Uwaga:**

- stanowi wyłącznie warstwę informacyjną

---

## 5. Mapowanie ROS ↔ firmware STM32

| Funkcja | Gdzie realizowana |
|-------|-------------------|
| Safety / E‑STOP | STM32 |
| Watchdog | STM32 |
| Sterowanie napędem | STM32 |
| Detekcja człowieka | ROS |
| Decyzje follow‑me | ROS |
| Ostateczna zgoda na ruch | STM32 |

---

## 6. Stany systemu a ROS

Mapowanie zgodne z `STATE_MACHINE.md`:

| Stan | Zachowanie ROS |
|-----|----------------|
| BOOT | ROS offline lub ignorowany |
| IDLE | Brak publikacji ruchu |
| ACTIVE | Publikacja `/motion/intent` |
| SAFE_STOP | ROS publikuje zero‑motion |
| EMERGENCY_STOP | ROS ignorowany |

---

## 7. Minimalny zestaw ROS (MVP)

Dla pierwszej wersji robota wystarczą:

- `perception_camera_node`
- `perception_human_detector_node`
- `tracking_node`
- `motion_decision_node`
- `ros_protocol_bridge_node`

---

## 8. Zasada końcowa

> **ROS decyduje *co* robot chce zrobić.**  
> **STM32 decyduje *czy* i *jak* robot może to zrobić.**

---

*Dokument ROS_NODE_MAP – mapa węzłów ROS w systemie.*
