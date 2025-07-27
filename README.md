
# 🔍 BASS ROOT CHECKER

![Root Detection](https://img.shields.io/badge/Root%20Detection-Aktif-blueviolet?style=flat-square)
![Magisk & Zygisk](https://img.shields.io/badge/Magisk%20&%20Zygisk-TERDETEKSI-critical?style=flat-square)
![Cheat Cloak](https://img.shields.io/badge/Cheat%20Cloak%20Scanner-Aman-informational?style=flat-square)
![Non Root](https://img.shields.io/badge/No%20Root-Diperlukan-green?style=flat-square)

> **BASS ROOT CHECKER** adalah alat pendeteksi root tersembunyi tingkat lanjut, dirancang untuk mendeteksi berbagai bentuk modifikasi sistem Android yang disembunyikan menggunakan teknik modern seperti Zygisk, Shamiko, KernelSU, dan modul-modul seperti LSPosed, SUSFS, Game Guardian, dll.

---

## ✨ Fitur Utama

- ✅ Deteksi **root tersembunyi** (termasuk Zygisk, Shamiko, KernelSU)
- ✅ Deteksi **Magisk**, **Xposed**, **LSPosed**, **SUSFS**
- ✅ Deteksi keberadaan **Game Guardian**, Cheat Engine, dan tools sejenis
- ✅ Hasil scan disimpan otomatis dalam file `.txt`
- ✅ Tidak membutuhkan akses root
- ✅ Bisa dijalankan di **Termux** (versi ELF)
- ✅ Versi Android (APK) dengan tampilan ringan dan mudah dipahami

---

## 📦 Struktur File

```
📁 BASS_ROOT_CHECKER/
├── BASS_ROOTCHECK_FINAL.elf        # Versi ELF untuk Termux
├── bass-root-checker.apk           # Versi APK Android
├── /output/
│   └── scan_result.txt             # Hasil scan otomatis
├── README.md                       # Dokumentasi ini
```

---

## 🧪 Cara Jalankan (Versi Termux)

```bash
chmod +x BASS_ROOTCHECK_FINAL.elf
./BASS_ROOTCHECK_FINAL.elf
```

Hasil akan disimpan otomatis ke: `output/scan_result.txt`

---

## 📋 Penjelasan Deteksi

| Komponen               | Status Deteksi                 |
|------------------------|-------------------------------|
| Root (umum)            | ✅ Bisa terdeteksi             |
| Magisk / Zygisk        | ✅ Termasuk modul tersembunyi  |
| Shamiko / SUSFS        | ✅ Teridentifikasi otomatis    |
| KernelSU               | ✅ Termasuk versi terbaru      |
| Game Guardian / CE     | ✅ Dicari lewat jejak & proses |
| Modul Xposed / LSPosed | ✅ Dicari lewat mount & file   |

---

## 💡 Kegunaan

- Deteksi manipulasi sistem pada HP pengguna
- Cek keamanan untuk aplikasi sensitif (bank, game, dll)
- Menemukan jejak root atau cheat meskipun disembunyikan
- Alat bantu debugging dan audit sistem

---

## 🛠 Teknologi

- Bahasa C/C++ (versi ELF)
- Java Android (versi APK)
- Deep scan file system & process
- Tanpa akses root

---

## 👤 Developer

**BASS TEAM**  
🛠️ Sejak 2025  
📍 Indonesia

---

## 📄 Lisensi

Proyek ini menggunakan lisensi **MIT**.  
Bebas digunakan dan dimodifikasi, dengan tetap mencantumkan kredit.

---
