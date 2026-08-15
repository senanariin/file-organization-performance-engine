#include "common.h"

#define RAW_FOLDER "../../../RawData/Common-Credentials/"
#define PROCESSED_FOLDER "../../../Processed/"

// Satır sonlarındaki görünmeyen boşluk, \r ve \n kaçış karakterlerini temizleyen budama fonksiyonu
// Trimming function to clear invisible trailing spaces, \r, and \n escape characters
void trim_line(char* str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r' || str[len - 1] == ' ')) {
        str[--len] = '\0';
    }
}

void preprocess_and_clean() {
    char list_cmd[512];
    // Windows alt seviye I/O komutu ile hedef klasördeki tüm ham veri dosyalarının isim listesini çıkarıyoruz
    // Extracting the list of all raw data filenames in the target directory using low-level Windows I/O commands
    sprintf(list_cmd, "dir /b \"%s\" > \"%sfile_list.txt\"", RAW_FOLDER, PROCESSED_FOLDER);
    system(list_cmd);

    FILE* list_file = fopen("../../../Processed/file_list.txt", "r");
    FILE* temp_merged = fopen("../../../Processed/temp_merged.txt", "w");

    if (!list_file || !temp_merged) {
        printf("Error: Failed to create list or temporary file! (Check the path)\n");
        return;
    }

    char file_name[256];
    printf("[1/3] Scanning and merging files...\n");

    // [ADIM 1] Tüm harici sözlük dosyalarını tek tek okuyup boşluklardan arındırarak dev bir geçici dosyada birleştirme
    // [STEP 1] Reading all external dictionary files one by one, stripping spaces, and merging them into a giant temporary file
    while (fgets(file_name, sizeof(file_name), list_file)) {
        trim_line(file_name);
        if (strstr(file_name, ".txt") && strcmp(file_name, "file_list.txt") != 0) {
            char full_path[512];
            sprintf(full_path, "%s%s", RAW_FOLDER, file_name);

            FILE* raw = fopen(full_path, "r");
            if (raw) {
                char line[256];
                while (fgets(line, sizeof(line), raw)) {
                    trim_line(line);
                    if (strlen(line) > 0) fprintf(temp_merged, "%s\n", line);
                }
                fclose(raw);
            }
        }
    }
    fclose(list_file); fclose(temp_merged);

    printf("[2/3] Sorting...\n");
    char sort_cmd[512];
    // [ADIM 2] Tekilleştirme (Deduplication) yapabilmek için ham veriyi disk üzerinde alfabetik olarak sıralıyoruz
    // [STEP 2] Sorting raw data alphabetically on disk to enable deduplication
    sprintf(sort_cmd, "sort \"../../../Processed/temp_merged.txt\" /o \"../../../Processed/sorted_data.txt\"");
    system(sort_cmd);

    printf("[3/3] De-duplicating unique data...\n");
    FILE* sorted = fopen("../../../Processed/sorted_data.txt", "r");
    FILE* final = fopen("../../../Processed/cleaned_data.txt", "w");

    if (sorted && final) {
        char current[256], last[256] = "";
        // [ADIM 3] Sıralı dosyada alt alta gelen mükerrer (aynı) kayıtları eleyerek sadece tekil (unique) verileri yazıyoruz
        // [STEP 3] Eliminating consecutive duplicate records in the sorted file and writing only unique data
        while (fgets(current, sizeof(current), sorted)) {
            if (strcmp(current, last) != 0) {
                fprintf(final, "%s", current);
                strcpy(last, current); // Keeping the last written data in memory to compare it with the next line (Bir sonraki satırla kıyaslamak için son yazılan veriyi bellekte tut)
            }
        }
        fclose(sorted); fclose(final);
    }

    // Disk üzerinde gereksiz yer kaplamaması için geçici ara dosyaları temizliyoruz (Garbage Collection)
    // Cleaning up temporary intermediate files to avoid wasting disk space (Garbage Collection)
    remove("../../../Processed/file_list.txt");
    remove("../../../Processed/temp_merged.txt");
    remove("../../../Processed/sorted_data.txt");
}