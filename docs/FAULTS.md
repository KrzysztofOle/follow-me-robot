# FAULTS.md

Dokument opisuje **klasyfikację błędów (faults)** w systemie robota mobilnego *Follow‑Me Robot* oraz **jednoznaczne reakcje warstwy SAFETY**.

Celem dokumentu jest:

- zapewnienie przewidywalnej reakcji systemu na każdy wykryty błąd,
- powiązanie błędów z **SAFE_STOP** lub **EMERGENCY_STOP**,
- ułatwienie testowania, debugowania i audytu bezpieczeństwa.

Dokument jest ściśle powiązany z:

- `ARCHITECTURE.md`
- `PROTOCOL.md`
- `SAFETY.md`
- `E_STOP_CHECKLIST.md`

---

## 1. Zasady ogólne obsługi błędów

1. Każdy błąd musi być:
   - wykryty,
   - sklasyfikowany,
   - obsłużony deterministycznie.
2. Błąd nigdy nie może powodować niekontrolowanego ruchu.
3. Brak danych traktowany jest jak **błąd krytyczny**.
4. Reset systemu **nie usuwa skutków błędu krytycznego** bez decyzji warstwy SAFETY.

---

## 2. Poziomy krytyczności błędów

| Poziom | Nazwa         | Reakcja                   |
|--------|---------------|---------------------------|
|  F0    | Informacyjny  | Log, brak wpływu na ruch  |
|  F1    | Ostrzeżenie   | Ograniczenie funkcjonalne |
|  F2    | Krytyczny     | SAFE_STOP                 |
|  F3    | Katastrofalny | EMERGENCY_STOP            |

---

## 3. Błędy komunikacji

### F2‑COM‑01: Timeout sygnału sterowania RC

- **Opis:** brak aktualizacji kanałów RC w zadanym czasie
- **Wykrycie:** licznik czasu w STM32
- **Reakcja:** SAFE_STOP

---

### F3‑COM‑02: Uszkodzone ramki sterujące

- **Opis:** powtarzające się błędy CRC / struktury ramki
- **Wykrycie:** parser PROTOCOL
- **Reakcja:** EMERGENCY_STOP

---

## 4. Błędy firmware

### F3‑FW‑01: Watchdog hardware reset

- **Opis:** zawieszenie firmware STM32
- **Wykrycie:** IWDG
- **Reakcja:** EMERGENCY_STOP

---

### F2‑FW‑02: Błąd pętli sterowania

- **Opis:** przekroczenie czasu cyklu sterowania
- **Wykrycie:** pomiar czasu cyklu
- **Reakcja:** SAFE_STOP

---

## 5. Błędy czujników

### F2‑SEN‑01: Brak danych z czujnika krytycznego

- **Opis:** brak aktualizacji danych
- **Reakcja:** SAFE_STOP

---

### F2‑SEN‑02: Dane poza zakresem fizycznym

- **Opis:** wartości nierealistyczne
- **Reakcja:** SAFE_STOP

---

### F3‑SEN‑03: Sprzeczne dane czujników

- **Opis:** dane wzajemnie się wykluczają
- **Reakcja:** EMERGENCY_STOP

---

## 6. Błędy napędu (VESC / AESC)

### F2‑DRV‑01: Błąd statusu napędu

- **Opis:** błąd zgłoszony przez VESC
- **Reakcja:** SAFE_STOP

---

### F3‑DRV‑02: Brak potwierdzenia zatrzymania

- **Opis:** napęd nie reaguje na STOP
- **Reakcja:** EMERGENCY_STOP (power cut)

---

## 7. Błędy zasilania

### F2‑PWR‑01: Spadek napięcia (brown‑out)

- **Opis:** napięcie poniżej progu bezpiecznej pracy
- **Reakcja:** SAFE_STOP

---

### F3‑PWR‑02: Niestabilne zasilanie napędu

- **Opis:** fluktuacje zasilania silników
- **Reakcja:** EMERGENCY_STOP

---

## 8. Błędy safety

### F3‑SAFE‑01: Aktywacja E‑STOP

- **Opis:** mechaniczny lub logiczny E‑STOP
- **Reakcja:** EMERGENCY_STOP

---

### F3‑SAFE‑02: Uszkodzenie toru safety

- **Opis:** brak możliwości sterowania przekaźnikiem safety
- **Reakcja:** EMERGENCY_STOP

---

## 9. Mapowanie błędów na testy

Każdy błąd klasy **F2** i **F3** musi posiadać odpowiadający test w:
- `E_STOP_CHECKLIST.md`

Błędy bez testu = **niezatwierdzony system**.

---

## 10. Zasada końcowa

> **Nieobsłużony błąd jest błędem krytycznym.**

---

*Dokument FAULTS – klasyfikacja i reakcje systemowe.*
