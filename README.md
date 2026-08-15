# File Organization & Performance Engine

C diliyle geliştirilmiş; ham, hash'lenmiş ve sıkıştırılmış dosya depolama yapılarının disk kullanımı, arama süreleri ve sıkıştırma maliyetlerini karşılaştıran veri performans analiz motoru.

---

## 📂 Proje Yapısı (Project Structure)

Projenin dosya organizasyonu ve modüler mimarisi aşağıda gösterilmiştir:

<img width="625" height="350" alt="photo" src="https://github.com/user-attachments/assets/3cb8a1a9-2e9f-4b00-af0a-6c789471f26f" />


* **`main.c`**: Ana çalıştırma döngüsü ve performans test senaryoları.
* **`storage.c` & `search.c`**: Disk depolama mantığı ve arama algoritmaları.
* **`compression.c` & `md5.c`**: Sıkıştırma (Zlib) ve hash (OpenSSL MD5) işlevleri.
* **`preprocess.c` & `utils.c`**: Veri ön işleme ve yardımcı fonksiyonlar.

---

## 📌 Proje Hakkında (Overview)

Bu proje, büyük boyuttaki veri kümelerinin farklı depolama formatlarında nasıl performans gösterdiğini deneysel olarak ölçmek amacıyla tasarlanmıştır. Sistem üç ana depolama mimarisini benchmark eder:

* **Raw (Ham):** Düz metin veri formatı.
* **Hashed (MD5):** Arama ve doğrulama süreçlerini hızlandırmak için indekslenmiş/hash'lenmiş veri formatı.
* **Compressed (Sıkıştırılmış - Zlib):** Disk alanından tasarruf sağlayan sıkıştırılmış veri formatı.

---

## 📊 Veri Seti ve Atıf (Dataset & Credits)

Bu projede kullanılan parola veri kümesi, **Daniel Miessler** tarafından yönetilen açık kaynaklı **SecLists** deposundan temin edilmiştir:

* **Veri Seti Kaynağı:** [SecLists / Passwords / Common-Credentials](https://github.com/danielmiessler/SecLists/tree/master/Passwords/Common-Credentials)
* **Derleyen / Yazar:** Daniel Miessler ve SecLists Katkıda Bulunanları

---

## 🛠️ Teknolojiler ve Gereksinimler

* **Dil:** C (C11 / C17)
* **IDE / Derleyici:** Visual Studio 2022 (MSVC x64)
* **Bağımlılıklar:** 
  * OpenSSL (MD5 / Cryptographic Operations)
  * Zlib (Data Compression)

---

## 🚀 Kurulum ve Çalıştırma

1. Repoyu klonlayın:
   ```bash
   git clone [https://github.com/senanariin/file-organization-performance-engine.git](https://github.com/senanariin/file-organization-performance-engine.git)
