#define _CRT_SECURE_NO_WARNINGS 
#ifndef COMMON_H
#define COMMON_H
// --- Standart C ve İşletim Sistemi Kütüphaneleri ---
// --- Standard C and Operating System Libraries ---
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> // For hardware-level QueryPerformanceCounter timers (Donanımsal QueryPerformanceCounter zamanlayıcıları için)
#include <time.h>
#include <math.h>
#include <psapi.h>  // To capture RAM/Process resource consumption metrics (RAM/Proses kaynak tüketim metriklerini yakalamak için)
#include <openssl/md5.h>

// Üçüncü parti ve sistem kütüphanelerini Visual Studio'ya otomatik bağlar.
// Automatically links third-party and system libraries to Visual Studio.
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "psapi.lib")


// --- Fonksiyon Prototipleri ---
/**
 * @brief SecLists veri setini okur, temizler ve standart referans haline getirir.
 * 1. AŞAMA: Preprocessing (Ön İşleme) adımıdır.
 */

 // --- Function Prototypes ---
 /**
 * @brief Reads, cleans, and converts the SecLists data set into a standard reference. 
 * STAGE 1: It is the Preprocessing step. 
 */
void preprocess_and_clean();
/**
 * @brief Temizlenen veriden RAW, HASHED ve COMPRESSED dosyalarını diske yazar.
 * 2. AŞAMA: Depolama modellerinin disk üzerinde fiziksel olarak inşa edilmesini sağlar.
 */

 /** 
 * @brief Writes RAW, HASHED, and COMPRESSED files from the cleaned data to the disk. 
 * STAGE 2: Enables the physical construction of storage models on the disk. 
 */
void create_all_storage_variants();
/**
 * @brief Bir dosyayı Zlib kütüphanesinin deflate() motoru ile sıkıştırır.
 * @param source_path Kaynak ham dosya yolu
 * @param dest_path   Üretilecek .zlib uzantılı ikili (binary) dosya yolu
 * @return int        Başarı durum kodu (0: Başarılı, -1: Hata)
 */

 /** 
 * @brief Compresses a file using the deflate() engine of the Zlib library. 
 * @param source_path Source raw file path 
 * @param dest_path   The path of the binary file with .zlib extension to be produced 
 * @return int        Success status code (0: Success, -1: Error) 
 */
int compress_file(const char* source_path, const char* dest_path);
/**
 * @brief Girdi olarak alınan parolanın 32 karakterlik MD5 hex özetini üretir.
 * HASHED arama modelinde sorgulanan kelimeyi maskelemek için kullanılır.
 */

 /** 
 * @brief Generates the 32-character MD5 hex hash of the password received as input. 
 * Used to mask the queried word in the HASHED search model. 
 */
void compute_md5(const char* metin, char* sonuc);
/**
 * @brief 3 farklı modeli (Raw, Hashed, Compressed) deneysel olarak test eder.
 * 3. AŞAMA: Arama hızlarını, CPU/RAM yüklerini ölçer ve sonuç matrisini basar.
 */

 /** 
 * @brief Experimentally tests 3 different models (Raw, Hashed, Compressed). 
 * STAGE 3: Measures search speeds, CPU/RAM loads, and prints the result matrix. 
 */
void detayliAnalizVeHizTesti();
/**
 * @brief Sıkıştırılmış verinin bellek üzerinde geri açılma maliyetlerini test eder.
 */

 /** 
 * @brief Tests the memory decompression costs of the compressed data. 
 */
void decompress_performance_test();

// --- Utils (Yardimci Fonksiyonlar) ---
/**
 * @brief Belirtilen dosyanın disk üzerindeki boyutunu döndürür.
 * Raporlama aşamasındaki Alan (Space) metriklerinin doğruluğunu sağlar.
 */

 // --- Utils (Utility Functions) ---
 /**
  * @brief Returns the size of the specified file on the disk. 
  * Ensures the accuracy of Space metrics during the reporting stage. 
  */
long dosyaBoyutuAl(const char* yol);
/**
 * @brief Programın o anki anlık RAM (çalışma kümesi) kullanımını MB olarak ölçer.
 */

 /** 
 * @brief Measures the current instantaneous RAM (working set) usage of the program in MB. 
 */
double ramKullanimiAl();
/**
 * @brief İşlemcinin bu proses için harcadığı saf kernel ve user CPU süresini alır.
 */

 /** 
 * @brief Retrieves the pure kernel and user CPU time spent by the processor for this process. 
 */
double get_cpu_time();
/**
 * @brief COMPRESSED modelde arama yapabilmek için Zlib verisini RAM'e inflate eder.
 * @param kaynakYol     Sıkıştırılmış dosyanın yolu
 * @param orijinalBoyut Açılan verinin boyutunu döndüren çıktı işaretçisi
 * @return unsigned char* Bellekte açılan ham verinin adres işaretçisi
 */

 /** 
 * @brief Inflates Zlib data into RAM in order to perform a search in the COMPRESSED model. 
 * @param kaynakYol     The path of the compressed file 
 * @param orijinalBoyut Output pointer that returns the size of the decompressed data 
 * @return unsigned char* Address pointer of the raw data decompressed in memory 
 */
unsigned char* veriyiAc(const char* kaynakYol, long* orijinalBoyut);

#endif