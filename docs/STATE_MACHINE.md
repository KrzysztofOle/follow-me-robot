# STATE_MACHINE.md

Dokument opisuje **maszynę stanów (state machine)** robota mobilnego *Follow‑Me Robot*.

Celem dokumentu jest:

- jednoznaczne zdefiniowanie dozwolonych stanów systemu,
- określenie przejść między stanami,
- powiązanie stanów z dokumentami **SAFETY.md**, **FAULTS.md** i **PROTOCOL.md**,
- zapobieganie niekontrolowanym zachowaniom robota.

---

## 1. Zasady ogólne maszyny stanów

1. Robot **zawsze** znajduje się w dokładnie jednym stanie.
2. Każde przejście musi mieć jasno określony warunek.
3. Stany bezpieczeństwa mają **pierwszeństwo nad wszystkimi innymi**.
4. Brak zdarzenia = brak przejścia.
5. Przejście do **EMERGENCY_STOP** jest możliwe z każdego stanu.

---

## 2. Lista stanów systemu

| Stan           | Opis                       | Ruch dozwolony |
|----------------|----------------------------|----------------|
| BOOT           | Uruchamianie systemu       |     ❌         |
| IDLE           | System gotowy, brak ruchu  |     ❌         |
| ACTIVE         | Normalna praca robota      |     ✅         |
| SAFE_STOP      | Kontrolowane zatrzymanie   |     ❌         |
| EMERGENCY_STOP | Natychmiastowe zatrzymanie |     ❌         |

---

## 3. Opis stanów

### 3.1 BOOT

**Opis:**

- inicjalizacja MCU
- testy podstawowe (self‑test)
- sprawdzenie toru safety

**Wejście:**

- włączenie zasilania
- reset STM32

**Wyjście:**

- BOOT → IDLE, jeśli brak błędów
- BOOT → EMERGENCY_STOP, jeśli błąd krytyczny

---

### 3.2 IDLE

**Opis:**

- system gotowy
- napęd zablokowany
- brak ruchu robota

**Dozwolone akcje:**

- kalibracje
- testy czujników

**Wyjście:**

- IDLE → ACTIVE (komenda start + warunki safety OK)
- IDLE → EMERGENCY_STOP

---

### 3.3 ACTIVE

**Opis:**

- robot porusza się
- napęd aktywny
- odbierane komendy ruchu z RC lub wyższej warstwy sterowania

**Warunki utrzymania:**

- poprawna komunikacja
- brak błędów F2/F3
- odpowiednie dane czujników

**Wyjście:**

- ACTIVE → SAFE_STOP (błąd F2)
- ACTIVE → EMERGENCY_STOP (błąd F3 / E‑STOP)
- ACTIVE → IDLE (komenda stop)
- ACTIVE → SAFE_STOP (LOST_SIGNAL)

### 3.3.1 MANUAL_RC

**Opis:**

- tryb ręcznego sterowania z aparatury RC
- komendy RC są bazą dla ruchu robota
- wyższe priorytety mogą nadpisać komendy RC

### 3.3.2 LIMIT_SPEED

**Opis:**

- aktywowany przy średniej odległości przeszkody
- prędkość wyjściowa jest skalowana względem komendy RC
- wzór: `speed_out = RC_speed * factor`
- `factor` konfigurowalne w zakresie `0.2 - 0.5`

### 3.3.3 STOP_OBSTACLE

**Opis:**

- aktywowany przy krytycznie małej odległości przeszkody
- nadpisuje każdą komendę RC
- wymusza zatrzymanie robota

### 3.3.4 LOST_SIGNAL

**Opis:**

- brak aktualizacji PWM przez ponad `100 ms`
- lub aktywacja failsafe w odbiorniku RC

**Reakcja:**

- przejście do `SAFE_STOP`

---

### 3.4 SAFE_STOP

**Opis:**

- kontrolowane zatrzymanie
- prędkość zadana = 0
- zasilanie napędu pozostaje włączone

**Wyjście:**

- SAFE_STOP → IDLE (warunki OK + potwierdzenie)
- SAFE_STOP → EMERGENCY_STOP

---

### 3.5 EMERGENCY_STOP

**Opis:**

- natychmiastowe zatrzymanie
- sprzętowe odcięcie zasilania napędu
- ignorowanie komend z RC i wyższych warstw sterowania

**Wejście:**

- E‑STOP
- watchdog
- błąd F3

**Wyjście:**

- tylko manualny reset systemu

---

## 4. Przejścia nadrzędne (asynchroniczne)

Z każdego stanu:

- E‑STOP → EMERGENCY_STOP
- watchdog trigger → EMERGENCY_STOP
- krytyczny fault (F3) → EMERGENCY_STOP

---

## 5. Mapowanie stanów na dokumenty safety

| Stan           | SAFETY | FAULTS | E_STOP_TEST |
|----------------|--------|--------|-------------|
| BOOT           |   ✅   |  ✅    |     ⛔      |
| IDLE           |   ✅   |  ✅    |     ⛔      |
| ACTIVE         |   ✅   |  ✅    |     ✅      |
| SAFE_STOP      |   ✅   |  ✅    |     ✅      |
| EMERGENCY_STOP |   ✅   |  ✅    |     ✅      |

---

## 6. Implementacja firmware (wytyczne)

- maszyna stanów realizowana w **jednym module**
- brak przejść warunkowych w wielu miejscach
- przejścia rejestrowane (log / trace)
- brak bezpośredniego sterowania napędem poza ACTIVE

---

## 7. Zasada końcowa

> **Jeśli stan jest nieznany → EMERGENCY_STOP.**

---

*Dokument STATE_MACHINE – definicja zachowania systemu.*
