# E_STOP_CHECKLIST.md

# ✅ CHECKLISTA TESTÓW E‑STOP  
*(Emergency Stop – Follow‑Me Robot)*

Checklistę należy wykonać **przed pierwszym ruchem robota**, po **każdej zmianie firmware**, oraz po **modyfikacjach hardware (napęd, zasilanie, safety)**.

---

## 0. Warunki wstępne (PRE‑CONDITIONS)

- ☐ Robot podniesiony (koła w powietrzu) **LUB** w wolnej, bezpiecznej przestrzeni
- ☐ Pakiet baterii naładowany (>30%)
- ☐ Brak osób w zasięgu robota
- ☐ Dostępny mechaniczny przycisk **E‑STOP**
- ☐ Sprawny przekaźnik / stycznik odcinający zasilanie napędu

**Jeśli którykolwiek punkt nie jest spełniony → TESTÓW NIE WYKONYWAĆ.**

---

## 1. Test E‑STOP mechanicznego (hardware)

### 1.1 E‑STOP przy postoju
- ☐ Robot w stanie IDLE (napęd nieaktywny)
- ☐ Wciśnięcie E‑STOP powoduje:
  - ☐ natychmiastowe odcięcie zasilania napędu
  - ☐ brak reakcji na komendy ruchu
  - ☐ stan systemu = EMERGENCY_STOP

✅ Oczekiwany wynik: robot pozostaje nieruchomy, VESC/AESC bez zasilania.

---

### 1.2 E‑STOP podczas jazdy
- ☐ Robot jedzie z niską prędkością
- ☐ Wciśnięcie E‑STOP powoduje:
  - ☐ wyzerowanie prędkości w < 1 cyklu sterowania
  - ☐ sprzętowe odcięcie zasilania napędu
  - ☐ brak ponownego załączenia napędu

✅ Oczekiwany wynik: natychmiastowe zatrzymanie, brak niekontrolowanego ruchu.

---

### 1.3 Reset bez zwolnienia E‑STOP
- ☐ E‑STOP pozostaje wciśnięty
- ☐ Reset STM32
- ☐ Napęd pozostaje bez zasilania

✅ Oczekiwany wynik: brak możliwości ruchu po resecie.

---

## 2. SAFE_STOP vs EMERGENCY_STOP

### 2.1 SAFE_STOP (software)
- ☐ Wymuszenie SAFE_STOP (brak komendy, soft error)
- ☐ Reakcja:
  - ☐ prędkość zadana = 0
  - ☐ zasilanie napędu pozostaje włączone

✅ Oczekiwany wynik: kontrolowane zatrzymanie.

---

### 2.2 EMERGENCY_STOP
- ☐ Aktywacja E‑STOP lub krytycznego błędu
- ☐ Reakcja:
  - ☐ soft stop
  - ☐ hard power cut
  - ☐ ignorowanie komend z Jetsona

✅ Oczekiwany wynik: całkowity brak napędu.

---

## 3. Testy watchdogów

### 3.1 Watchdog firmware
- ☐ Zatrzymanie głównej pętli STM32
- ☐ Watchdog resetuje MCU
- ☐ Napęd pozostaje odłączony

---

### 3.2 Reset MCU podczas jazdy
- ☐ Robot jedzie
- ☐ Reset STM32
- ☐ Brak ruchu po resecie

✅ Oczekiwany wynik: reset ≡ STOP.

---

## 4. Test komunikacji Jetson ↔ STM32

### 4.1 Timeout Jetsona
- ☐ Przerwanie komunikacji
- ☐ Po ~200 ms:
  - ☐ SAFE_STOP lub EMERGENCY_STOP
  - ☐ brak ruchu robota

---

### 4.2 Niepoprawne ramki
- ☐ Wysłanie ramek z błędnym CRC
- ☐ Ramki ignorowane

✅ Oczekiwany wynik: brak reakcji napędu.

---

## 5. Testy czujników

- ☐ Odłączenie czujnika odległości
- ☐ Wartości spoza zakresu
- ☐ Opóźnione dane

✅ Oczekiwany wynik: SAFE_STOP lub ograniczenie funkcjonalne.

---

## 6. Testy zasilania

### 6.1 Spadek napięcia
- ☐ Symulacja brown‑out
- ☐ Brak niekontrolowanego ruchu

### 6.2 Utrata zasilania Jetsona
- ☐ Jetson OFF
- ☐ STM32 ON
- ☐ Robot pozostaje zatrzymany

---

## 7. Powrót z E‑STOP

- ☐ Zwolnienie E‑STOP
- ☐ Wymagany reset systemu
- ☐ Brak automatycznego startu napędu

✅ Oczekiwany wynik: świadomy i bezpieczny powrót.

---

## 8. Kryteria zaliczenia

System zalicza testy E‑STOP jeżeli:
- ✔ żaden test nie powoduje niekontrolowanego ruchu
- ✔ każda awaria kończy się STOP lub EMERGENCY_STOP
- ✔ napęd nigdy nie działa bez zgody warstwy SAFETY

---

## 9. Zasada końcowa

> **Jeśli masz wątpliwość – test NIE ZALICZONY.**  
> **Robot, który się zatrzymał, jest bezpieczny.**

---

*Dokument bezpieczeństwa – checklist E‑STOP.*
