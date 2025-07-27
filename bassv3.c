#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/system_properties.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <jni.h>

// ================ WARNA & UI ================ //
#define RED "\x1B[1;31m"
#define GREEN "\x1B[1;32m"
#define YELLOW "\x1B[1;33m"
#define BLUE "\x1B[1;34m"
#define PURPLE "\x1B[1;35m"
#define CYAN "\x1B[1;36m"
#define FLASH "\x1B[5;31m"
#define RESET "\x1B[0m"

// ================ ANIMASI LOADING ================ //
void show_loading(const char* text) {
    printf(BLUE "╔════════════════════════════════════════════╗\n");
    printf(BLUE "║ " FLASH "%s ", text);
    fflush(stdout);
    
    for(int i=0; i<15; i++) {
        printf(RED "■ " RESET);
        fflush(stdout);
        usleep(100000);
    }
    
    printf(BLUE "║\n╚════════════════════════════════════════════╝\n" RESET);
}

// ================ DETEKSI UTAMA ================ //

// [1] TRACEPID CHECK (Anti-Debug)
void check_tracepid() {
    show_loading("TRACEPID CHECK");
    FILE* status = fopen("/proc/self/status", "r");
    if(status) {
        char line[128];
        while(fgets(line, sizeof(line), status)) {
            if(strstr(line, "TracerPid:")) {
                int pid = atoi(strchr(line, ':') + 1);
                if(pid != 0) {
                    printf(RED "[✗] Debugger Attached (PID: %d)\n" RESET, pid);
                    fclose(status);
                    return;
                }
            }
        }
        fclose(status);
    }
    printf(GREEN "[✓] No Debugger Detected\n" RESET);
}

// [2] MEMCMP ANTI-HOOKING
void check_memcmp_hooking() {
    show_loading("MEMCMP HOOK CHECK");
    void* memcmp_addr = dlsym(RTLD_NEXT, "memcmp");
    unsigned char* bytes = (unsigned char*)memcmp_addr;
    
    // Deteksi instruksi JMP (0xE9) atau CALL (0xE8)
    if(bytes[0] == 0xE9 || bytes[0] == 0xE8) {
        printf(RED "[✗] memcmp() Hooked (JMP Detected)\n" RESET);
        return;
    }
    printf(GREEN "[✓] memcmp(): Clean\n" RESET);
}

// [3] MEMORY SCANNING
void check_memory_anomalies() {
    show_loading("MEMORY SCANNING");
    FILE* maps = fopen("/proc/self/maps", "r");
    if(!maps) {
        printf(YELLOW "[!] Cannot access memory maps\n" RESET);
        return;
    }

    int found = 0;
    char line[256];
    while(fgets(line, sizeof(line), maps)) {
        if(strstr(line, "rwxp") || 
           strstr(line, "magisk") || 
           strstr(line, "riru")) {
            printf(RED "[✗] Suspicious Memory: %s" RESET, line);
            found = 1;
        }
    }
    fclose(maps);
    
    if(!found) printf(GREEN "[✓] Memory: Clean\n" RESET);
}

// [4] JNI CALL CHECK
void check_jni_abuse() {
    show_loading("JNI CALL CHECK");
    void* get_created_java_vms = dlsym(RTLD_DEFAULT, "JNI_GetCreatedJavaVMs");
    unsigned char* bytes = (unsigned char*)get_created_java_vms;
    
    if(bytes[0] == 0xE9 || bytes[0] == 0x0F) {
        printf(RED "[✗] JNI Function Hooked\n" RESET);
        return;
    }
    printf(GREEN "[✓] JNI: Clean\n" RESET);
}

// [5] SYSTEM PROPERTIES
void check_system_properties() {
    show_loading("SYSTEM PROPERTIES");
    char value[PROP_VALUE_MAX];
    
    __system_property_get("ro.debuggable", value);
    if(strcmp(value, "1") == 0) {
        printf(RED "[✗] ro.debuggable=1 (Debug Mode)\n" RESET);
    }
    
    __system_property_get("ro.secure", value);
    if(strcmp(value, "0") == 0) {
        printf(RED "[✗] ro.secure=0 (Insecure)\n" RESET);
    }
    
    __system_property_get("ro.build.tags", value);
    if(strstr(value, "test-keys")) {
        printf(RED "[✗] Build with test-keys\n" RESET);
    }
}

// ================ MAIN ================ //
int main() {
    printf(PURPLE "\n\n██████╗ █████╗ ███████╗███████╗███████╗\n");
    printf(PURPLE "██╔══██╗██╔══██╗██╔════╝██╔════╝██╔════╝\n");
    printf(PURPLE "██████╔╝███████║███████╗███████╗███████╗\n");
    printf(PURPLE "██╔══██╗██╔══██║╚════██║╚════██║╚════██║\n");
    printf(PURPLE "██████╔╝██║  ██║███████║███████║███████║\n");
    printf(PURPLE "╚═════╝ ╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝\n\n" RESET);

    int detected = 0;
    
    // [DETEKSI BARU]
    check_tracepid();
    check_memcmp_hooking();
    check_memory_anomalies();
    check_jni_abuse();
    check_system_properties();
    
    // [PESAN FINAL - TIDAK DIUBAH]
    printf(CYAN "\n╔════════════════════════════════════════════╗\n");
    if(detected > 0) {
        printf(CYAN "║ " RED "FINAL: %d ROOT METHODS DETECTED!          ║\n", detected);
        printf(CYAN "║ " RED "HP LU KEBANYAKAN MODIF! WKWKWK NTT! 😈    ║\n");
    } else {
        printf(CYAN "║ " GREEN "FINAL: HP AMAN, NO ROOT DETECTED!       ║\n");
        printf(CYAN "║ " GREEN "ATAU PAKAI MODULE HIDE LEVEL TINGGI !   ║\n");
    }
    printf(CYAN "╚════════════════════════════════════════════╝\n" RESET);

    return 0;
}
