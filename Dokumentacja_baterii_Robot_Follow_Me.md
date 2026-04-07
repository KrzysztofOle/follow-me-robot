# 🔋 Dokumentacja techniczna baterii – Robot Follow‑Me

## 📌 1. Identyfikacja baterii

- **Typ:** Li‑ion (litowo‑jonowa)  
- **Pochodzenie:** hoverboard  
- **Konfiguracja:** 10S2P  
- **BMS:** wbudowany (zabezpieczenia: OVP / UVP / OCP)

---

## ⚡ 2. Dane znamionowe

| Parametr | Wartość |
|---------|--------|
| Napięcie nominalne | 36 V |
| Napięcie maksymalne | 42 V |
| Napięcie minimalne | ~30 V |
| Pojemność | 4,4 Ah |
| Energia | 158,4 Wh |

---

## 🔌 3. Architektura elektryczna

Bateria posiada **jedno wspólne złącze** wykorzystywane do:
- zasilania układu napędowego (VESC),
- ładowania pakietu.

### Schemat połączeń

```text
          ┌── VESC
BATERIA ──┤
          └── ŁADOWARKA
```

---

## ⚠️ 4. Ograniczenia i ryzyka

### ❗ Ładowanie podczas jazdy
**Zabronione** — może prowadzić do:
- uszkodzenia ładowarki,
- niestabilności napięcia DC,
- zadziałania zabezpieczeń BMS (odcięcie zasilania).

---

### ❗ Rekuperacja (hamowanie odzyskowe)

- generuje prąd zwrotny do baterii,
- przy pełnym naładowaniu (≈42 V) istnieje ryzyko:
  - przeładowania,
  - uszkodzenia BMS lub VESC.

---

## ⚙️ 5. Zalecane ustawienia VESC

| Parametr | Wartość |
|---------|--------|
| Battery current max | 10–12 A |
| Battery current regen | −4 do −6 A |
| Motor current | 20–30 A |
| Cutoff start | 33 V |
| Cutoff end | 30 V |

---

## 🔧 6. Zasilanie elektroniki sterującej

```text
36 V → przetwornica DC/DC → 5 V → ESP32 → czujniki (3,3 V)
```

---

## 🔌 7. Ładowanie

- **Ładowarka:** 42 V (hoverboard)
- **Prąd ładowania:** ~2 A
- **Podłączenie:** bezpośrednio do baterii (przez BMS)

---

## 🧠 8. Rekomendacje projektowe

- dodać bezpiecznik **15–20 A** po stronie baterii,
- stosować **wspólną masę** dla całego systemu,
- używać markowych przetwornic 5 V,
- unikać tanich, niestabilnych modułów buck.

---

## 📌 9. Podsumowanie

- ✅ kompatybilna z VESC  
- ✅ wystarczająca do testów  
- ⚠️ ograniczona pojemność  
- ⚠️ wymaga poprawnej konfiguracji
