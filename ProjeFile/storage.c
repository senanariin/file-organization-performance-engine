#include "common.h"

void create_all_storage_variants() {
    // 3 Kat yukarıdaki klasör yolları
    // Folder paths 3 levels up
    FILE* processed = fopen("../../../Processed/cleaned_data.txt", "r");
    FILE* raw_out = fopen("../../../Storage/Raw/raw_data.txt", "w");
    FILE* hashed_out = fopen("../../../Storage/Hashed/hashed_data.txt", "w");

    // Dosya açma kontrolü
    // File opening control
    if (!processed || !raw_out || !hashed_out) {
        printf("Error: Failed to create storage files! (Check the paths)\n");
        if (processed) fclose(processed);
        if (raw_out) fclose(raw_out);
        if (hashed_out) fclose(hashed_out);
        return;
    }

    // MD5 sonucu her zaman 32 karakter + \0 olduğu için 33 byte yeterlidir
    // Since the MD5 result is always 32 characters + \0, 33 bytes is sufficient
    char line[256], hash_result[33];
    printf("[Storage] Creating Raw and Hashed files...\n");

    while (fgets(line, sizeof(line), processed)) {
        // 1. Raw dosyasına orijinal satırı direkt yaz (Olduğu gibi saklama)
        // 1. Write the original line directly to the raw file (Storing as-is)
        fprintf(raw_out, "%s", line);

        // 2. Hash hesaplamak için satır sonu karakterlerini (\n, \r) temizle
        // 2. Clean trailing characters (\n, \r) to calculate the hash
        char temp[256];
        strcpy(temp, line);
        temp[strcspn(temp, "\r\n")] = 0;

        // Eğer satır boş değilse MD5 hesapla ve kaydet
        // [HASHED MODELI]: Kelimenin MD5 özetini hesaplayıp sabit 33 bayt (32 karakter + \n) genişlikle yazma

        // If the line is not empty, calculate and save the MD5
        // [HASHED MODEL]: Calculating the MD5 digest of the word and writing it with a fixed width of 33 bytes (32 characters + \n)
        if (strlen(temp) > 0) {
            // Calls the OpenSSL function inside md5.c (md5.c içindeki OpenSSL fonksiyonunu çağırır)
            compute_md5(temp, hash_result); // OpenSSL-based hash generation (OpenSSL tabanlı hash üretimi)
            fprintf(hashed_out, "%-32s\n", hash_result); // Fixed-length record design (Sabit uzunluklu kayıt (Fixed-length record) tasarımı)
        }
    }

    fclose(processed);
    fclose(raw_out);
    fclose(hashed_out);

    printf("[Storage] Sorting Hash files (for Binary Search)... \n");
    char sort_hash_cmd[512];
    // /L C: Yerel sıralama düzenini kullanır, /REC 33: Her satırın 33 karakter (32 hex + \n) olduğunu belirtir
    // /L C: Uses the local sort order, /REC 33: Specifies that each line is 33 characters (32 hex + \n)
    sprintf(sort_hash_cmd, "sort /L C /REC 33 \"../../../Storage/Hashed/hashed_data.txt\" /O \"../../../Storage/Hashed/hashed_data.txt\"");
    system(sort_hash_cmd);

    // 3. Sıkıştırma (Zlib) işlemini başlat
    // [COMPRESSED MODELI]: Zlib kütüphanesinin deflate algoritmasını çağırarak ikili sıkıştırma adımını tetikleme

    // 3. Initialize the compression (Zlib) process
    // [COMPRESSED MODEL]: Invoking the deflate algorithm of the zlib library to trigger the binary compression step
    printf("[Storage] Initiating Zlib compression...\n");
    if (compress_file("../../../Processed/cleaned_data.txt", "../../../Storage/Compressed/compressed.dat")) {
        printf("[Success] All storage variants successfully created.\n");
    }
    else {
        printf("[Error] Compression process failed!\n");
    }
}