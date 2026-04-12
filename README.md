

Sprzęt ktury aktualnie mam:

Tymczasowo:
        Plytka oznaczona: Xnucleo F103RB (mbed ARM Cortex-M3)
        Procesor: STM32F103RBT6
                Cecha           Wartość
                🧠 CPU          Cortex-M3
                ⏱️ Clock         72 MHz
                💾 Flash        128 KB
                🧮 RAM          20 KB
                🔌 GPIO         ~51
                📡 I2C          2x
                📡 SPI          2x
        📡 UART         3x
        🔌 USB          FS (device)

Test komunikacji UART dla STM32F103RB:
- firmware w `stm32_f103_project/src/tests/uart_led/main.c`
- env: `uart_led`
- używany interfejs: `USART2`
- linie: `PA2` (TX), `PA3` (RX, niewykorzystywany)
- TX działa przez polling na rejestrach `USART2`
- parametry: `115200 8N1`
- monitor: `~/.platformio/penv/bin/pio device monitor -b 115200 -p /dev/tty.usbserial-0001`

LED na płytce testowej:
- `LED4` -> `PC5`
- `LED3` -> `PC8`
- `LED2` -> `PC9`
- `LED1` -> `PA5`

Test I2C:
- firmware w `stm32_f103_project/src/tests/i2c/main.c`
- env: `i2c_scan`
- magistrala: `I2C1`
- linie: `PB6` (SCL), `PB7` (SDA)

Docelowo: STM32 NUCLEO-H755ZI-Q - STM32H755ZIT6 ARM Cortex M7/M4


Jednosta centralna AI: Nvidia Jetson Nano Dev Kit


Czujniki:
2 x VL53L8
6 x URM09 (I2C)

AESC / VESC
Autoro Dual V6 (AESC / VESC 6.7)


2️⃣ Jetson Nano Dev Kit B01 (najczęściej spotykana) - chyba ta ? ? TODO: Sprawdzic
🔹 Rok: 2020
🔹 ✅ Najbardziej „standardowa” wersja
Różnice względem A02:

2× CSI (obsługa dwóch kamer)
Poprawiony layout PCB
Pełna zgodność pinów GPIO z przyszłymi wersjami
Nadal 4 GB RAM

Specyfikacja:

Quad‑core ARM Cortex‑A57
GPU Maxwell 128 CUDA cores
HDMI + DisplayPort
Gigabit Ethernet

INNE NOTATKI:

Budowa robota śledzącego człowieka to temat, gdzie **nie ma jednego czujnika – zawsze stosuje się kombinację (tzw. fuzję sensorów)**, żeby uzyskać stabilne śledzenie. ([arXiv][1])

Poniżej masz konkretne, praktyczne opcje – od najprostszych do profesjonalnych.

---

# 🔧 1. Najprostsze rozwiązania (DIY, tanie)

## 📷 Kamera + AI (najlepszy stosunek efekt/koszt)

![Image](https://miro.medium.com/v2/resize%3Afit%3A1200/1%2AFYS2PwA2O9fF3MLMGwWlxA.png)

![Image](https://www.mdpi.com/sensors/sensors-19-04943/article_deploy/html/images/sensors-19-04943-g001.png)

![Image](https://content.instructables.com/F32/8KDD/L6M71TSH/F328KDDL6M71TSH.jpg?auto=webp)

![Image](https://www.electronicsforu.com/wp-contents/uploads/2026/01/Screenshot-2026-01-16-at-3.20.41-PM-Large-1024x490.jpeg)

**Jak działa:**

* kamera wykrywa człowieka (np. YOLO, MediaPipe)
* robot podąża za środkiem sylwetki

**Czego użyć:**

* Raspberry Pi + kamera
* Python + OpenCV / YOLO / MediaPipe

**Zalety:**

* bardzo dokładne
* rozpoznaje konkretną osobę (np. kolor, twarz)

**Wady:**

* zależne od światła
* większe wymagania CPU/GPU

👉 To najczęstsze rozwiązanie w projektach hobbystycznych.

---

## 📡 Czujniki ultradźwiękowe (HC-SR04)

![Image](https://abacasstorageaccnt.blob.core.windows.net/cirkit/eb1701e9-7fd1-47c8-b23f-3447ceabc48d.png)

![Image](https://circuitdigest.com/sites/default/files/projectimage_mic/human-following-robot.jpg)

![Image](https://hackster.imgix.net/uploads/attachments/390375/hacksterio_cover_B8wPMKX5c6.jpg?auto=compress%2Cformat\&fit=min\&h=675\&w=900)

**Jak działa:**

* robot mierzy odległość do obiektu przed nim
* utrzymuje dystans

**Zalety:**

* bardzo tanie
* proste w implementacji

**Wady:**

* nie rozpoznaje człowieka (tylko przeszkodę)
* słaba precyzja kierunku

👉 dobre jako **dodatek**, nie jako główny sensor

---

## 🔴 IR / śledzenie nadajnika (pilot, beacon)

![Image](https://europe1.discourse-cdn.com/arduino/original/4X/3/8/2/382c4a92624c9198c255e8e1e1ff00717128915d.jpeg)

![Image](https://hacksterio.s3.amazonaws.com/uploads/attachments/1429995/schematic_diagram_ctfUjAnFVL.jpg)

![Image](https://projects.arduinocontent.cc/cover-images/6fb3eace-df78-4064-bc34-ecb7f7a4ce35.png)

**Jak działa:**

* człowiek ma nadajnik IR / BLE / UWB
* robot śledzi sygnał

**Zalety:**

* działa w ciemności
* proste sterowanie

**Wady:**

* trzeba nosić nadajnik
* mniej „inteligentne”

---

# ⚙️ 2. Średnio-zaawansowane rozwiązania

## 📡 LiDAR (np. RPLidar)

![Image](https://www.researchgate.net/publication/328883720/figure/fig2/AS%3A1086464792240152%401636044800117/Robot-used-for-implementation-and-test-Equipped-with-a-RPLIDAR-A1-scanner.jpg)

![Image](https://vision.rwth-aachen.de/media/paper_images/212/Screen_Shot_2021-06-22_at_11.56.06_AM.png)

![Image](https://www.researchgate.net/publication/220633576/figure/fig1/AS%3A671529876598789%401537116607128/What-is-Simultaneous-Localization-and-Mapping-SLAM-A-robot-observes-the-environment.png)

**Jak działa:**

* skanuje otoczenie 360°
* wykrywa sylwetkę jako obiekt ruchomy

**Zalety:**

* działa w ciemności
* bardzo stabilny pomiar odległości

**Wady:**

* trudniejsza analiza (algorytmy)
* droższy

👉 często używany w robotach mobilnych

---

## 📷 Kamera głębi (Intel RealSense, Kinect)

![Image](https://www.robotshop.com/cdn/shop/files/unitree-g1-intel-depth-camera-realsense-d435i-img_1200x1200.webp?v=1727676187)

![Image](https://www.researchgate.net/publication/282245620/figure/fig1/AS%3A391612397047808%401470379079294/Example-of-skeleton-tracking-with-the-Kinect.png)

![Image](https://scx2.b-cdn.net/gfx/news/hires/2023/a-new-model-that-allow-1.jpg)

**Jak działa:**

* widzi obraz + głębię (3D)
* śledzi szkielet człowieka

**Zalety:**

* bardzo dokładne śledzenie
* odporne na tło

**Wady:**

* cena
* większe zużycie energii

👉 jedno z najlepszych rozwiązań „plug & play”

---

# 🚀 3. Profesjonalne / zaawansowane

## 🧠 Fuzja sensorów (najlepsze podejście)

👉 kamera + LiDAR + ultradźwięki + IMU

* kamera → kto to jest
* LiDAR → gdzie jest
* ultradźwięki → bezpieczeństwo
* IMU → stabilizacja ruchu

Takie podejście daje największą dokładność i odporność na błędy ([arXiv][2])

---

## 📡 UWB (Ultra Wideband – lokalizacja)

* dokładność do ~10–30 cm
* tracker na człowieku

👉 bardzo dobre w magazynach / przemyśle

---

# 🧠 Jakie rozwiązanie wybrać (praktycznie)

### 🔹 Najlepszy start (Twoje projekty DIY / STM32 / vision):

👉 **kamera + OpenCV + YOLO**

### 🔹 Jeśli chcesz stabilności:

👉 **kamera + ultradźwięki (backup)**

### 🔹 Jeśli chcesz PRO poziom:

👉 **RealSense + ROS + SLAM**

---

# 📊 Przykładowa architektura (sprawdzona)

```
Kamera (YOLO) → wykrycie człowieka
        ↓
PID sterowanie → kierunek robota
        ↓
Ultradźwięki → zabezpieczenie przed kolizją
```

---

# ⚠️ Najczęstsze błędy

* ❌ tylko ultradźwięki → robot „gubi” człowieka
* ❌ tylko kamera → problemy przy słabym świetle
* ❌ brak filtracji → drgania (tu pomaga filtr Kalmana) ([Yadda][3])

---

# Jeśli chcesz — mogę dobrać konkretny zestaw pod Twój projekt

Napisz:

* jaki budżet
* czy robot indoor/outdoor
* czy chcesz STM32 czy Raspberry Pi

👉 Mogę rozpisać Ci gotową architekturę + konkretne modele czujników i kod startowy.

[1]: https://arxiv.org/abs/1703.04877?utm_source=chatgpt.com "Real-time 3D Human Tracking for Mobile Robots with Multisensors"
[2]: https://arxiv.org/abs/1801.04137?utm_source=chatgpt.com "Multisensor Online Transfer Learning for 3D LiDAR-based Human Detection with a Mobile Robot"
[3]: https://yadda.icm.edu.pl/baztech/element/bwmeta1.element.baztech-fbe6dcd9-798c-47a2-a1a4-dd47b119cefc/c/MRR_2018_7_18-25.pdf?utm_source=chatgpt.com "Zastosowanie filtru Kalmana"





✅ Najlepszy wybór do projektów i edukacji (jeśli już masz Nano).
