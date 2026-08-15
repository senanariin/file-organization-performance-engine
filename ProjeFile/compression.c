#include "common.h"
#include <zlib.h>

int compress_file(const char* source_path, const char* dest_path) {
    FILE* src = fopen(source_path, "rb");
    FILE* dst = fopen(dest_path, "wb");
    if (!src || !dst) return 0;

    // 1. Orijinal boyutu al ve dosyanın en başına yaz (Header)
    // Sıkıştırılmış dosyanın en başına geri açma (Decompression) aşamasında kullanılacak orijinal boyutu gömüyoruz (Custom Header)

    // 1. Get the original size and write it to the very beginning of the file (Header)
    // Embedding the original size at the very beginning of the compressed file to be used during the decompression stage (Custom Header)
    fseek(src, 0, SEEK_END);
    fseek(src, 0, SEEK_END);
    long original_size = ftell(src);
    fseek(src, 0, SEEK_SET);
    fwrite(&original_size, sizeof(long), 1, dst);

    char in[16384], out[16384];
    z_stream strm = { 0 };
    // Zlib kütüphanesini maksimum sıkıştırma (Z_BEST_COMPRESSION) moduyla başlatıyoruz
    // Initializing the zlib library with maximum compression (Z_BEST_COMPRESSION) mode
    if (deflateInit(&strm, Z_BEST_COMPRESSION) != Z_OK) return 0;

    int flush;
    // Bloklar halinde (Chunk-based) ham veriyi okuyup Zlib motoruna besleyen ana döngü
    // Main loop that reads raw data in blocks (Chunk-based) and feeds it into the Zlib engine
    do {
        strm.avail_in = (uInt)fread(in, 1, sizeof(in), src);
        flush = feof(src) ? Z_FINISH : Z_NO_FLUSH; // If the file has ended, we set the Z_FINISH flag to terminate the stream (Dosya bittiyse akışı sonlandırmak için Z_FINISH bayrağını set ediyoruz)
        strm.next_in = (Bytef*)in;
        // Zlib tampon belleği (Buffer) dolana kadar sıkıştırılmış veriyi parça parça çıktı dosyasına yazan iç döngü
        // Inner loop that writes the compressed data piece by piece to the output file until the Zlib buffer is full
        do {
            strm.avail_out = sizeof(out);
            strm.next_out = (Bytef*)out;
            deflate(&strm, flush); // Compressing the data in binary format using the Deflate algorithm (Deflate algoritması ile veriyi ikili (binary) formatta sıkıştırıyoruz)
            fwrite(out, 1, sizeof(out) - strm.avail_out, dst);
        } while (strm.avail_out == 0);
    } while (flush != Z_FINISH);

    deflateEnd(&strm); // Cleaning up the dynamic memory areas used by the Zlib engine (Resource Cleanup) (Zlib motorunun kullandığı dinamik bellek alanlarını temizliyoruz (Resource Cleanup))
    fclose(src); fclose(dst);
    return 1;
}

unsigned char* veriyiAc(const char* kaynakYol, long* orijinalBoyut) {
    FILE* f = fopen(kaynakYol, "rb");
    if (!f) return NULL;

    // 1. Dosyanın başındaki orijinal boyutu oku
    // Kodun başındaki Custom Header alanından, verinin sıkıştırılmadan önceki net boyutunu okuyoruz

    // 1. Read the original size at the beginning of the file
    // Reading the net size of the data before compression from the Custom Header field at the beginning of the code
    long stored_original_size;
    if (fread(&stored_original_size, sizeof(long), 1, f) != 1) {
        fclose(f);
        return NULL;
    }

    // 2. Sıkıştırılmış verinin gerçek boyutunu hesapla (Toplam - Header Boyutu)
    // Toplam dosya boyutundan başlık (Header) boyutunu çıkararak diske yazılmış net sıkıştırılmış veri boyutunu buluyoruz

    // 2. Calculate the actual size of the compressed data (Total - Header Size)
    // Finding the net compressed data size written to disk by subtracting the header size from the total file size
    long header_size = ftell(f);
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    long compressed_data_size = file_size - header_size;
    fseek(f, header_size, SEEK_SET); // Positioning the pointer to where the header ends and the compressed binary data begins (İşaretçiyi başlığın bittiği, sıkıştırılmış binary verinin başladığı yere konumlandırıyoruz)

    // 3. Sıkıştırılmış veriyi oku
    // Sıkıştırılmış ham binary veriyi RAM'e yüklemek için dinamik bellekten yer ayırıyoruz

    // 3. Read the compressed data
    // Allocating space from dynamic memory to load the compressed raw binary data into RAM
    unsigned char* compressed_buffer = (unsigned char*)malloc(compressed_data_size);
    fread(compressed_buffer, 1, compressed_data_size, f);
    fclose(f);

    // 4. Header'dan aldığımız kesin boyuta göre yer ayır
    // Başlıktan okuduğumuz boyuta göre, verinin açılmış hali için tam gerektiği kadar dinamik bellek (RAM) ayırıyoruz

    // 4. Allocate space according to the exact size retrieved from the Header
    // Allocating exactly as much dynamic memory (RAM) as needed for the decompressed state of the data, based on the size read from the header
    unsigned char* decompressed_buffer = (unsigned char*)malloc(stored_original_size + 1);
    unsigned long dest_size = (unsigned long)stored_original_size;

    // Zlib uncompress fonksiyonu ile RAM üzerindeki sıkışmış veriyi tek seferde çözüyoruz (In-memory Decompression)
    // Decompressing the compressed data on RAM in a single pass using the Zlib uncompress function (In-memory Decompression)
    if (uncompress(decompressed_buffer, &dest_size, compressed_buffer, compressed_data_size) != Z_OK) {
        free(compressed_buffer);
        free(decompressed_buffer);
        return NULL;
    }

    decompressed_buffer[stored_original_size] = '\0'; // Appending a terminator to prevent buffer overflow in C string functions (C string fonksiyonlarında bellek taşması (Buffer Overflow) olmaması için sonlandırıcı ekleme)
    free(compressed_buffer); // Freeing the compressed data buffer from memory that has completed its function (İşlevini tamamlayan sıkıştırılmış veri tamponunu bellekten siliyoruz)

    *orijinalBoyut = stored_original_size; // Passing the original size information outside the function via a pointer (Fonksiyon dışına orijinal boyut bilgisini pointer ile aktarıyoruz)
    return decompressed_buffer;
}