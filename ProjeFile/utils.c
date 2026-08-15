#include "common.h" 
#include <sys/stat.h>
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")

// 1. Disk Kullanımı Kriteri için: Dosya boyutunu döndürür
// [DISK METRIGI]: POSIX stat yapısını kullanarak hedef dosyanın disk üzerindeki ham boyutunu döndürür

// 1. For Disk Usage Criterion: Returns the file size
// [DISK METRIC]: Returns the raw size of the target file on disk using the POSIX stat structure
long dosyaBoyutuAl(const char* yol) {
    struct stat st;
    if (stat(yol, &st) == 0) return st.st_size;
    return 0;
}

// 2. RAM Kullanımı Kriteri için: O anki bellek kullanımını MB cinsinden ölçer
// [RAM METRIGI]: Win32 API yardımıyla uygulamanın o anki fiziksel bellek tüketimini izole eder

// 2. For RAM Usage Criterion: Measures the current memory usage in MB
// [RAM METRIC]: Isolates the current physical memory consumption of the application using the Win32 API
double ramKullanimiAl() {
    PROCESS_MEMORY_COUNTERS_EX pmc = { 0 }; // The EX extension provides access to process-specific (private) areas (EX uzantısı, prosese özel (private) alanlara erişim sağlar)

    // GetProcessMemoryInfo ile işletim sisteminden güncel bellek sayaçlarını talep ediyoruz
    // Requesting current memory counters from the operating system via GetProcessMemoryInfo
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        // Paylaşılan sayfaları hariç tutup, sadece malloc ile ayrılan net alanı bulmak için PrivateUsage'ı MB birimine çeviriyoruz
        // To exclude shared pages and find only the net area allocated by malloc, we convert PrivateUsage to MB
        return (double)pmc.PrivateUsage / (1024.0 * 1024.0);
    }
    return 0;
}
// 3. CPU Kullanımı Kriteri için: İşlemcinin bu program için harcadığı toplam süreyi ölçer
// [CPU METRIGI]: İşlemcinin (CPU) sadece bu uygulamayı yürütmek için harcadığı saf zamanı ölçer

// 3. For CPU Usage Criterion: Measures the total time spent by the processor for this program
// [CPU METRIC]: Measures the pure time spent by the processor (CPU) solely to execute this application
double get_cpu_time() {
    FILETIME a, b, c, d;
    // İşletim sisteminin zamanlayıcısından kernel (çekirdek) ve user (kullanıcı) modu çalışma sürelerini alıyoruz
    // Retrieving kernel mode and user mode execution times from the operating system scheduler
    if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d)) {
        ULARGE_INTEGER kernel, user;
        kernel.LowPart = c.dwLowDateTime; kernel.HighPart = c.dwHighDateTime;
        user.LowPart = d.dwLowDateTime; user.HighPart = d.dwHighDateTime;
        // 100 nanosaniyelik Windows zaman birimlerini (Ticks), toplam saf işlemci saniyesine dönüştürüyoruz
        // Converting 100-nanosecond Windows time units (Ticks) into total pure processor seconds
        return (double)(kernel.QuadPart + user.QuadPart) / 10000000.0;
    }
    return 0;
}