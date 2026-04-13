# SAFETY.md

Dokument opisuje **mechanizmy bezpieczeństwa (functional safety)** robota mobilnego *Follow‑Me Robot*.

Celem warstwy SAFETY jest zapewnienie, że robot:
- **nigdy nie wykona niekontrolowanego ruchu**,
- **zatrzyma się w przypadku błędu**,
- pozostanie bezpieczny nawet przy awarii oprogramowania AI lub komunikacji.

Dokument obowiązuje **wszystkie warstwy systemu**, ze szczególnym naciskiem na firmware STM32.

---

## 1. Zasady nadrzędne bezpieczeństwa

1. **Bezpieczeństwo > funkcjonalność**
2. **STM32 jest autorytetem bezpieczeństwa**
3. Brak danych ≡ zagrożenie
4. System musi przejść do stanu bezpiecznego samodzielnie
5. Jetson Nano nigdy nie ma bezpośredniej kontroli nad napędem

---

## 2. Stany bezpieczeństwa systemu

| Stan | Opis |
|-----|------|
| BOOT | start systemu, brak ruchu |
| IDLE | system gotowy, napęd nieaktywny |
| ACTIVE | ruch robota dozwolony |
| SAFE_STOP | kontrolowane zatrzymanie |
| EMERGENCY_STOP | natychmiastowe zatrzymanie |

Przejście do **SAFE_STOP** lub **EMERGENCY_STOP** jest możliwe z każdego stanu.

---

## 3. Emergency Stop (E‑STOP)

### 3.1 Źródła E‑STOP

E‑STOP może zostać wyzwolony przez:
- wejście sprzętowe (przycisk E‑STOP)
- watchdog firmware
- timeout komunikacji z Jetson Nano
- krytyczny błąd czujnika
- błąd konfiguracji systemu

### 3.2 Zachowanie po E‑STOP

Po aktywacji:
- natychmiastowe wyzerowanie komend prędkości
- napęd przechodzi w tryb hamowania / freewheel (wg konfiguracji)
- ignorowanie wszystkich komend z Jetsona
- wymagana ręczna interwencja do wyjścia z trybu E‑STOP

---

## Sprzętowe odcięcie zasilania napędu (Power Cut / Contactor)

### 4.1 Cel

Samo software'owe `STOP` po stronie VESC nie jest wystarczające do zapewnienia pełnego bezpieczeństwa układu napędowego.

Powody:
- software'owy `STOP` nadal zakłada poprawne działanie elektroniki sterującej i toru komunikacji,
- awaria firmware, zawieszenie MCU lub utrata kontroli nad logiką sterowania może pozostawić napęd w stanie aktywnym,
- błąd po stronie Jetson Nano nie może być jedynym mechanizmem ochrony przed ruchem silników.

Z tego względu wymagane jest sprzętowe odcięcie zasilania silników, które usuwa energię z toru mocy niezależnie od stanu oprogramowania.

### 4.2 Założenia bezpieczeństwa

- stosowany jest przekaźnik lub stycznik typu **NC** (normalnie zamknięty),
- brak sterowania ≡ brak zasilania silników,
- tor odcięcia zasilania musi być niezależny od Jetson Nano,
- stan bezpieczny ma być osiągany również przy resetach, watchdogach i błędach oprogramowania.

### 4.3 Architektura toru zasilania

Podstawowy tor mocy powinien być zaprojektowany jako:

`bateria → E‑STOP (NC) → przekaźnik/stycznik (NC) → VESC/AESC`

Rola mechanicznego E‑STOP:
- zapewnia bezpośrednie, fizyczne przerwanie toru mocy,
- nie zależy od logiki systemu operacyjnego ani od komunikacji z komputerem pokładowym,
- stanowi pierwszy, lokalny punkt wymuszenia bezpieczeństwa w obwodzie napędu.

### 4.4 Logika działania

- `SAFE_STOP` → software'owe zatrzymanie: VESC = 0,
- `EMERGENCY_STOP` → software'owe zatrzymanie + sprzętowe odcięcie zasilania,
- reset MCU lub zadziałanie watchdog'a → automatyczne odcięcie zasilania silników.

W praktyce oznacza to, że zatrzymanie kontrolowane i odcięcie energii są odrębnymi krokami, a sprzętowe rozłączenie toru mocy jest rezerwowym mechanizmem wymuszanym przy stanach krytycznych.

### 4.5 Ochrona przed łukiem elektrycznym

Nie wolno odcinać dużych prądów bez wcześniejszego soft-stopu, ponieważ zwiększa to ryzyko:
- łuku elektrycznego na stykach,
- degradacji przekaźnika lub stycznika,
- zakłóceń w torze zasilania i uszkodzenia elementów mocy.

Zalecana sekwencja:
1. soft stop,
2. krótka zwłoka umożliwiająca spadek prądu,
3. hard cut.

### 4.6 Odpowiedzialność rdzeni (STM32H755)

- **Cortex-M7**: podejmuje decyzję o odcięciu zasilania i steruje logiką toru bezpieczeństwa,
- **Cortex-M4**: nie ma bezpośredniego dostępu do toru mocy ani prawa do jego aktywacji.

Podział ten ogranicza powierzchnię zaufania i utrzymuje decyzje dotyczące mocy w rdzeniu pełniącym rolę nadrzędną dla SAFETY.

### 4.7 Zasady projektowe

- Jetson nigdy nie steruje przekaźnikiem mocy,
- przekaźnik musi powodować stan **FAIL-SAFE**,
- sprzętowe odcięcie jest ostatnią linią obrony, a nie mechanizmem podstawowym.

Sprzętowe odcięcie zasilania napędu jest elementem krytycznym warstwy SAFETY
i nie może być wyłączone ani obejście programowe.

---

## 4. Watchdogi

### 4.1 Watchdog sprzętowy (IWDG)
- aktywny zawsze w trybie produkcyjnym
- reset MCU przy zawieszeniu firmware

### 4.2 Watchdog programowy
- monitoruje pętle sterowania, komunikację i sensory
- wykrycie anomalii → SAFE_STOP

---

## 5. Bezpieczeństwo komunikacji

Zgodnie z `PROTOCOL.md`:
- brak komendy → STOP
- CRC error → ramka odrzucona
- timeout Jetsona (>200 ms) → SAFE_STOP

---

## 6. Bezpieczeństwo czujników

- walidacja zakresów
- liczniki błędów
- degradacja funkcjonalna zamiast natychmiastowego STOP (jeśli bezpieczne)

---

## 7. Podział odpowiedzialności (H755)

- **M7**: decyzje bezpieczeństwa, napęd
- **M4**: detekcja błędów czujników

---

## 8. Tryb standalone

Przy braku Jetsona:
- robot zatrzymany
- system w stanie bezpiecznym

---

## 9. Testy bezpieczeństwa

Każda zmiana MUSI być przetestowana pod kątem:
- E‑STOP
- watchdog
- timeoutów

---

## 10. Zasada końcowa

> **Robot, który się zatrzymał, jest bezpieczny.**

---

*Dokument bezpieczeństwa – wersja żywa.*
