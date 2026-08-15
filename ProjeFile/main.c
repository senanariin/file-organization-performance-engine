#define _CRT_SECURE_NO_WARNINGS 
#include "common.h"
#include <stdio.h>
int main() {
    printf("---Performance Analysis---\n\n");

    // 1. Veriyi Temizle
    // 1. AŞAMA: Veri Setinin Temizlenmesi ve Ön İşleme (Preprocessing)
    // SecLists deposundan alınan 8.3 milyonluk ham parola listesi okunur,
    // boşluklar/hatalı karakterler ayıklanarak standart formata getirilir.
    
    // 1. Clean the Data
    // STAGE 1: Data Set Cleaning and Preprocessing
    // The raw password list of 8.3 million entries obtained from the SecLists repository is read,
    // and spaces/corrupted characters are filtered out to bring it into a standard format.
    preprocess_and_clean();

    // 2. Depolama Varyantlarini Olustur
    // 2. AŞAMA: Depolama Varyantlarının (RAW, HASHED, COMPRESSED) Oluşturulması
    // Temizlenen veriden ardışıl RAW, OpenSSL tabanlı sıralı HASHED (MD5) 
    // ve Zlib kütüphanesi kullanılarak ikili (binary) COMPRESSED dosyaları inşa edilir.

    // 2. Create Storage Variants
    // STAGE 2: Creation of Storage Variants (RAW, HASHED, COMPRESSED)
    // Consecutive RAW files, OpenSSL-based sequential HASHED (MD5) files, 
    // and binary COMPRESSED files using the Zlib library are constructed from the cleaned data.
    create_all_storage_variants();

    // 3. Performans Testlerini Calistir
    // 3. AŞAMA: Deneysel Performans Testleri ve Zaman Ölçümü
    // Windows API (QueryPerformanceCounter) kullanılarak donanımsal düzeyde
    // arama hızları, disk alan verimlilikleri ve CPU/RAM yükleri analiz edilir.

    // 3. Run Performance Tests
    // STAGE 3: Experimental Performance Tests and Time Measurement
    // Search speeds, disk space efficiencies, and CPU/RAM loads are analyzed at the hardware level
    // using the Windows API (QueryPerformanceCounter).
    detayliAnalizVeHizTesti();

    printf("\nProcess completed. Press Enter to exit...");
    int dummy = getchar();
    return 0;
}