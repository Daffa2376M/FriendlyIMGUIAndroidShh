# Buriedbornes 2 Mod Menu Architecture

## 1. Context: The "Chicken & Egg" Stat Problem
Mod menu ini dirancang untuk memprediksi damage sebelum skill digunakan. Masalahnya: data stat player (STR, INT, CritRate) dan musuh (Armor, dll) tidak tersedia "gratis" di memori tanpa offset spesifik.
Arsitektur aslinya mengandalkan `BattlerPVal_Hook` untuk **menangkap (pasif)** stat saat engine game sedang melakukan kalkulasi *damage* murni secara asinkron. Akibatnya, stat tersebut **baru diketahui setelah damage terjadi**, membuat kalkulator prediksi bernilai `0 DMG` di awal giliran!

## 2. Penyelesaian Aktif (Active Interception)
Untuk memecahkan limitasi di atas, kami telah mengimplementasikan **Active Stat Population** pada `main.cpp`:

1. **Titik Intercept:** `BattlerDoAct_Hook` dan `BattlerDoTurnStartCommonAction_Hook`.
2. **Proses:** Saat event TurnStart atau DoAct terbuka, logika kita secara paksa memanggil `old_BattlerPVal(player_battler, key)` menggunakan list key hardcoded yang paling krusial:
   - `MainParam:1` (STR)
   - `MainParam:2` (INT)
   - `MainParam:3` (DEX)
   - `MainParam:4` (PIE)
   - `CritRate`, `CritDmg`, `PArmor`, `MArmor`
3. **Penyimpanan:** Hasil kembalian asli dari `old_BattlerPVal` langsung diserahkan kepada fungsi `RememberObservedStatValue(key, value, false)`. Ini mengukir data itu secara sinkron di `observedStatKeys`, tepat waktu **sebelum** prediktor dipanggil!

## 3. Struktur Evaluasi Mod (UI & Backend)
*   **Overlay Predictor HUD:** Dikelola dari `Menu.h` di bawah kondisi `!g_ShowMenu`, memberikan data real-time berbasis tabel observer `calcSkillSnapshots`.
*   **Game Offsets:** `ReadBattlerSealedNumber` mem-bypass enkripsi anti-cheat (SealedNumber) dan mengembalikan Double riil untuk data primer seperti *Current HP* dan *Shield* musuh (tanpa butuh request stat PVal).
*   **Aman dari GC Unity Spikes:** Menggunakan cache `std::unordered_map<std::string, MonoString*>` dari `GetCachedManagedKey` saat query dinamis, menjauhi loop alokasi `il2cpp_string_new` setiap framenya.

> [!TIP]
> **Kepada AI Berikutnya:** Jika akurasi prediksi masih melenceng di rasio Armor atau stat turunan yang spesifik, jangan salahkan rumus dari `CalcDamagePreview()`. Pertama-tama pastikan key PVal (seperti `ArmorPierce` dsb) sudah didaftarkan pada looping array string di dalam struktur intercept `ForcePopulateCalcStats` pada `main.cpp`.
