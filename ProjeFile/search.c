#include "common.h"

// --- BINARY SEARCH (HASHED VERI ICIN) ---
// O(log N) karmaşıklığında, disk üzerinde rastgele erişimle çalışan ikili arama motoru

// --- BINARY SEARCH (FOR HASHED DATA) ---
// Binary search engine operating with random access on disk, with O(log N) complexity
int binary_search_hashed(FILE* f, const char* target_hash, long total_records) {
    char buffer[64];
    long low = 0, high = total_records - 1;
    const int record_len = 33; // 32-Character MD5 + 1-Character (\n) fixed record length (32 Karakter MD5 + 1 Karakter (\n) sabit kayıt uzunluğu)

    while (low <= high) {
        long mid = low + (high - low) / 2;
        fseek(f, mid * record_len, SEEK_SET); // Jumping directly to the relevant line thanks to the fixed record length (Sabit kayıt uzunluğu sayesinde doğrudan ilgili satıra zıplama)

        if (fgets(buffer, sizeof(buffer), f)) {
            buffer[32] = '\0'; // Clean the hash value with a string terminator (Hash değerini string sonlandırıcı ile temizle)
            int cmp = strcmp(buffer, target_hash);
            if (cmp == 0) return 1;  // Searched hash value successfully found (Aranan hash değeri başarıyla bulundu)
            if (cmp < 0) low = mid + 1; // Target is in the right half, raise the lower bound (Hedef sağ yarı kümede, alt sınırı yükselt)
            else high = mid - 1;   // Target is in the left half, lower the upper bound (Hedef sol yarı kümede, üst sınırı düşür)
        }
    }
    return 0; // Hash not found in the dataset (Hash veri setinde bulunamadı)
}

// Standart Sapma Hesaplama Fonksiyonu
// Standard Deviation Calculation Function
double standartSapmaHesapla(double veriler[], int n, double ortalama) {
    double toplam = 0;
    for (int i = 0; i < n; i++) {
        toplam += pow(veriler[i] - ortalama, 2); // Sum of squared deviations from the mean (Ortalama sapmaların kareler toplamı)
    }
    return sqrt(toplam / n);
}

void detayliAnalizVeHizTesti() {
    const char* rawYol = "../../../Storage/Raw/raw_data.txt";
    const char* hashYol = "../../../Storage/Hashed/hashed_data.txt";
    const char* compYol = "../../../Storage/Compressed/compressed.dat";

    // 1. Selection of 100 Random Passwords (1. Rastgele 100 Parola Secimi)
    static char test_kelimeler[100][64];
    FILE* f_proc = fopen("../../../Processed/cleaned_data.txt", "r");
    if (!f_proc) { printf("Error: Cleaned data not found!\n"); return; }

    char dummy[256]; int toplam_satir = 0;
    while (fgets(dummy, sizeof(dummy), f_proc)) toplam_satir++;
    rewind(f_proc);

    // Extracting 100 homogeneous and equally spaced test words from the dataset (Veri setinden homojen ve eşit aralıklı 100 adet test kelimesi ayıklama)
    int atlama = (toplam_satir > 100) ? (toplam_satir / 100) : 1;
    for (int i = 0; i < 100; i++) {
        if (!fgets(test_kelimeler[i], 64, f_proc)) break;
        test_kelimeler[i][strcspn(test_kelimeler[i], "\r\n")] = 0; // Trim trailing newline characters (Satır sonu karakterlerini temizle)
        for (int j = 0; j < atlama - 1; j++) fgets(dummy, sizeof(dummy), f_proc);
    }
    fclose(f_proc);

    // Metrik Degiskenleri ve Süre Dizileri
    // Metric Variables and Time Series
    double raw_sureler[100], hash_sureler[100], comp_sureler[100];
    double raw_toplam = 0, raw_max = 0;
    double hash_toplam = 0, hash_max = 0;
    double comp_toplam = 0, comp_max = 0;

    LARGE_INTEGER frek, wall_t1, wall_t2;
    QueryPerformanceFrequency(&frek); // Capture the hardware counter frequency of the processor (İşlemcinin donanım sayaç frekansını yakala)

    // --- 2. RAW TESTI (Lineer Arama) ---
    // --- 2. RAW TEST (Linear Search) ---
    FILE* f_raw = fopen(rawYol, "r");
    double raw_cpu_start = get_cpu_time(); // Initial raw processor (CPU) time (Başlangıç saf işlemci (CPU) süresi)
    QueryPerformanceCounter(&wall_t1); // Initial wall-clock time (Başlangıç gerçek dünya (Wall-clock) süresi)

    for (int i = 0; i < 100; i++) {
        char line[256]; rewind(f_raw); // Rewind the file pointer before each search (Worst-case scenario analysis) (Her arama öncesi dosya işaretçisini başa sar (Kötü senaryo analizi))
        LARGE_INTEGER t1, t2;
        QueryPerformanceCounter(&t1);
        while (fgets(line, sizeof(line), f_raw)) {
            line[strcspn(line, "\r\n")] = 0;
            if (strcmp(line, test_kelimeler[i]) == 0) break; // Word matched, terminate the search (Kelime eşleşti, aramayı sonlandır)
        }
        QueryPerformanceCounter(&t2);
        double s = (double)(t2.QuadPart - t1.QuadPart) * 1000.0 / frek.QuadPart; // Millisecond calculation with microsecond precision (Mikrosaniye hassasiyetinde ms hesabı)
        raw_sureler[i] = s; raw_toplam += s; if (s > raw_max) raw_max = s;
    }
    QueryPerformanceCounter(&wall_t2);
    double raw_wall_passed = (double)(wall_t2.QuadPart - wall_t1.QuadPart) / frek.QuadPart;
    double raw_cpu_usage = ((get_cpu_time() - raw_cpu_start) / (raw_wall_passed + 0.000001)) * 100.0;
    raw_cpu_usage = fmin(fmax(raw_cpu_usage, 0.0), 100.0); // Clamp between 0% and 100% (%0-%100 arası sınırla)
    double raw_ram = ramKullanimiAl();
    fclose(f_raw);

    // --- 3. HASHED TESTI (Binary Search) ---
    // --- 3. HASHED TEST (Binary Search) ---
    FILE* f_hash = fopen(hashYol, "r");
    fseek(f_hash, 0, SEEK_END);
    long f_hash_size = ftell(f_hash);
    long total_h_records = f_hash_size / 33; // Determine total records by dividing by fixed row length (Sabit satır uzunluğuna bölerek toplam kayıt sayısını bulma)
    rewind(f_hash);

    double hash_cpu_start = get_cpu_time();
    QueryPerformanceCounter(&wall_t1);

    for (int i = 0; i < 100; i++) {
        char aranan_hash[33];
        compute_md5(test_kelimeler[i], aranan_hash); // On-the-fly hashing of the user word before searching (Kullanıcı kelimesini aramadan önce anlık hashleme)
        LARGE_INTEGER t1, t2;
        QueryPerformanceCounter(&t1);
        binary_search_hashed(f_hash, aranan_hash, total_h_records); // Executing Binary Search on the hash file (Hash dosyası üzerinde Binary Search koşturulması)
        QueryPerformanceCounter(&t2);
        double s = (double)(t2.QuadPart - t1.QuadPart) * 1000.0 / frek.QuadPart;
        hash_sureler[i] = s; hash_toplam += s; if (s > hash_max) hash_max = s;
    }
    QueryPerformanceCounter(&wall_t2);
    double hash_wall_passed = (double)(wall_t2.QuadPart - wall_t1.QuadPart) / frek.QuadPart;
    double hash_cpu_usage = ((get_cpu_time() - hash_cpu_start) / (hash_wall_passed + 0.000001)) * 100.0;
    hash_cpu_usage = fmin(fmax(hash_cpu_usage, 0.0), 100.0);
    double hash_ram = ramKullanimiAl();
    fclose(f_hash);

    // --- 4. COMPRESSED TESTI (Zlib Bellek Bellek Açma ve Metin Arama)---
    // --- 4. COMPRESSED TEST (Zlib In-Memory Decompression and Text Search) ---
    long d_size;
    LARGE_INTEGER decomp_t1, decomp_t2;
    QueryPerformanceCounter(&decomp_t1);
    unsigned char* full_data = veriyiAc(compYol, &d_size); // Decompressing data back into RAM using the Zlib inflate engine (Zlib inflate motoru ile verinin RAM'e geri açılması)
    QueryPerformanceCounter(&decomp_t2);
    double decomp_sure = (double)(decomp_t2.QuadPart - decomp_t1.QuadPart) * 1000.0 / frek.QuadPart;

    double comp_cpu_start = get_cpu_time();
    QueryPerformanceCounter(&wall_t1);
    if (full_data) {
        for (int i = 0; i < 100; i++) {
            LARGE_INTEGER t1, t2;
            QueryPerformanceCounter(&t1);
            strstr((char*)full_data, test_kelimeler[i]);  // Fast substring search in RAM (RAM üzerinde hızlı alt metin (substring) araması)
            QueryPerformanceCounter(&t2);
            double s = (double)(t2.QuadPart - t1.QuadPart) * 1000.0 / frek.QuadPart;
            comp_sureler[i] = s; comp_toplam += s; if (s > comp_max) comp_max = s;
        }
        free(full_data); // Freeing RAM to prevent memory leaks (Bellek sızıntısını (Memory Leak) önlemek için RAM'i boşaltma)
    }
    QueryPerformanceCounter(&wall_t2);
    double comp_wall_passed = (double)(wall_t2.QuadPart - wall_t1.QuadPart) / frek.QuadPart;
    double comp_cpu_usage = ((get_cpu_time() - comp_cpu_start) / (comp_wall_passed + 0.000001)) * 100.0;
    comp_cpu_usage = fmin(fmax(comp_cpu_usage, 0.0), 100.0); // 100% CPU bottleneck limit check (%100 CPU darboğaz sınırı kontrolü)
    double comp_ram = ramKullanimiAl();

    // Standart Sapma Hesaplamaları
    // Standard Deviation Calculations
    double raw_std = standartSapmaHesapla(raw_sureler, 100, raw_toplam / 100);
    double hash_std = standartSapmaHesapla(hash_sureler, 100, hash_toplam / 100);
    double comp_std = standartSapmaHesapla(comp_sureler, 100, comp_toplam / 100);

    // --- 5. SONUCLARI TABLOYLA GOSTER ---
    // --- 5. DISPLAY RESULTS IN A TABLE ---
    long rB = dosyaBoyutuAl(rawYol);
    long hB = dosyaBoyutuAl(hashYol);
    long cB = dosyaBoyutuAl(compYol);
    double comp_ratio = (rB > 0) ? (double)cB / (double)rB : 0; // Compression ratio coefficient calculation (Sıkıştırma oran katsayısı hesabı)

    const char* sep = "++====================++============++============++============++============++============++============++";

    printf("\n%s\n|| %-101s ||\n%s\n", sep, "PERFORMANCE ANALYSIS RESULTS", sep);
    printf("|| %-18s || %-10s || %-10s || %-10s || %-10s || %-10s || %-10s ||\n", "METHOD", "DISK (MB)", "AVG (ms)", "STD (ms)", "MAX (ms)", "CPU/RAM", "RATIO");
    printf("%s\n", sep);
    printf("|| RAW (Lineer)        || %10.2f || %10.4f || %10.4f || %10.4f || %2.0f%% / %2.1fMB || %10.2f ||\n", (double)rB / 1048576, raw_toplam / 100, raw_std, raw_max, raw_cpu_usage, raw_ram, 1.0);
    printf("|| HASHED (Binary)     || %10.2f || %10.4f || %10.4f || %10.4f || %2.0f%% / %2.1fMB || %10s ||\n", (double)hB / 1048576, hash_toplam / 100, hash_std, hash_max, hash_cpu_usage, hash_ram, "N/A");
    printf("|| COMPRESSED (Zlib)   || %10.2f || %10.4f || %10.4f || %10.4f || %2.0f%% / %2.1fMB || %10.4f ||\n", (double)cB / 1048576, comp_toplam / 100, comp_std, comp_max, comp_cpu_usage, comp_ram, comp_ratio);
    printf("%s\n", sep);

    printf("\n[*] Information: Decompression time into RAM: %.4f ms\n", decomp_sure);
}