#define _CRT_SECURE_NO_WARNINGS 
#define OPENSSL_API_COMPAT 0x10100000L
#include "common.h"
#include <openssl/md5.h>

// Visual Studio'ya hangi kütüphaneleri kullanacağını kod içinden bildiriyoruz.
// Bu sayede harici OpenSSL bağımlılıkları derleyiciye (Linker) otomatik olarak bağlanır.

// We specify inside the code which libraries Visual Studio should use.
// This way, external OpenSSL dependencies are automatically linked to the compiler (Linker).
#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libssl.lib")

/**
 * @brief Kullanıcıdan alınan düz metin parolanın MD5 kriptografik özetini hesaplar.
 *
 * Bu fonksiyon, HASHED depolama ve arama modelinin temelini oluşturur.
 * Hesaplanan 128-bitlik (16 byte) özet, disk üzerinde sabit uzunluklu (32 karakter + \n)
 * deterministik kayıtlar oluşturmak ve O(log N) İkili Arama (Binary Search)
 * algoritmasını koşturabilmek amacıyla 32 karakterlik hex formatına dönüştürülür.
 *
 * @param metin   Girdi olarak alınan düz metin parola (char dizisi)
 * @param sonuc   Çıktı olarak üretilecek 32 karakterli (+1 null terminator) MD5 hex string arabelleği
 */

 /** 
 * @brief Calculates the MD5 cryptographic hash of the plain text password received from the user. 
 * 
 * This function forms the basis of the HASHED storage and search model. 
 * The calculated 128-bit (16-byte) hash is converted into a 32-character hex format * in order to create fixed-length (32 characters + \n) deterministic records on the disk 
 * and to execute the O(log N) Binary Search algorithm. 
 * 
 * @param metin   The plain text password received as input (char array) 
 * @param sonuc   The 32-character (+1 null terminator) MD5 hex string buffer to be produced as output 
 */

void compute_md5(const char* metin, char* sonuc) {
    unsigned char ozet[MD5_DIGEST_LENGTH];

    // OpenSSL MD5 fonksiyonu
    // 1. ADIM: OpenSSL çekirdek fonksiyonu ile ham MD5 özetini hesapla

    // OpenSSL MD5 function
    // STEP 1: Calculate the raw MD5 hash using the OpenSSL core function
    MD5((const unsigned char*)metin, strlen(metin), ozet);

    // Byte dizisini 32 karakterli hex string'e donustur
    // 2. ADIM: 16 byte'lık ham binary veriyi, ekranda ve diskte okunabilir 
    // 32 karakterlik alt seviye hexadecimal string yapısına dönüştür

    // Convert the byte array into a 32-character hex string
    // STEP 2: Convert the 16-byte raw binary data into a readable 
    // 32-character low-level hexadecimal string structure on screen and disk
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(sonuc + (i * 2), "%02x", ozet[i]);
    }

    // String sonlandirici
    // 3. ADIM: C dili standartlarına uygun olarak string yapısını null karakterle sonlandır

    // String terminator
    // STEP 3: Terminate the string structure with a null character in accordance with C language standards
    sonuc[MD5_DIGEST_LENGTH * 2] = '\0';
}