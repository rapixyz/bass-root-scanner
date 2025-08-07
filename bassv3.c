#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/system_properties.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <elf.h>
#include <link.h>
#include <pthread.h>

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

// ================ DETECTION MODULES ================ //

int detect_magisk() {
    show_loading("MAGISK DEEP SCAN");
    int detected = 0;
    
    const char* magisk_paths[] = {
        "/sbin/.magisk", "/sbin/magisk", "/data/adb/magisk",
        "/cache/.disable_magisk", NULL
    };
    
    for(int i = 0; magisk_paths[i]; i++) {
        if(access(magisk_paths[i], F_OK) == 0) {
            printf(RED "[✗] Magisk file found: %s\n" RESET, magisk_paths[i]);
            detected = 1;
        }
    }
    
    printf(detected ? RED "[!] MAGISK DETECTED\n" RESET : GREEN "[✓] No Magisk\n" RESET);
    return detected;
}

int detect_superuser() {
    show_loading("SUPERUSER CHECK");
    int detected = 0;
    
    const char* su_paths[] = {
        "/system/bin/su", "/system/xbin/su", "/sbin/su",
        "/data/local/su", "/su/bin/su", NULL
    };
    
    for(int i = 0; su_paths[i]; i++) {
        if(access(su_paths[i], F_OK) == 0) {
            printf(RED "[✗] Superuser binary: %s\n" RESET, su_paths[i]);
            detected = 1;
        }
    }
    
    printf(detected ? RED "[!] SUPERUSER FOUND\n" RESET : GREEN "[✓] No Superuser\n" RESET);
    return detected;
}

int detect_xposed() {
    show_loading("XPOSED FRAMEWORK");
    int detected = 0;
    
    void* xposed = dlopen("libxposed_art.so", RTLD_NOW | RTLD_GLOBAL | RTLD_NOLOAD);
    if(xposed) {
        printf(RED "[✗] Xposed library loaded\n" RESET);
        detected = 1;
    }
    
    if(access("/data/data/de.robv.android.xposed.installer", F_OK) == 0) {
        printf(RED "[✗] Xposed installer found\n" RESET);
        detected = 1;
    }
    
    printf(detected ? RED "[!] XPOSED DETECTED\n" RESET : GREEN "[✓] No Xposed\n" RESET);
    return detected;
}

int detect_frida() {
    show_loading("FRIDA INJECTION");
    int detected = 0;
    
    void* frida = dlopen("libfrida-agent.so", RTLD_NOW | RTLD_GLOBAL | RTLD_NOLOAD);
    if(frida) {
        printf(RED "[✗] Frida library loaded\n" RESET);
        detected = 1;
    }
    
    FILE* maps = fopen("/proc/self/maps", "r");
    if(maps) {
        char line[256];
        while(fgets(line, sizeof(line), maps)) {
            if(strstr(line, "frida")) {
                printf(RED "[✗] Frida memory mapping: %s" RESET, line);
                detected = 1;
            }
        }
        fclose(maps);
    }
    
    printf(detected ? RED "[!] FRIDA DETECTED\n" RESET : GREEN "[✓] No Frida\n" RESET);
    return detected;
}

int detect_safetynet() {
    show_loading("SAFETYNET SPOOF");
    int detected = 0;
    
    char value[PROP_VALUE_MAX];
    __system_property_get("ro.boot.verifiedbootstate", value);
    if(strcmp(value, "orange") == 0) {
        printf(RED "[✗] Modified boot state: %s\n" RESET, value);
        detected = 1;
    }
    
    __system_property_get("ro.build.tags", value);
    if(strstr(value, "test-keys")) {
        printf(RED "[✗] Test keys build: %s\n" RESET, value);
        detected = 1;
    }
    
    printf(detected ? RED "[!] SAFETYNET MODIFIED\n" RESET : GREEN "[✓] SafetyNet OK\n" RESET);
    return detected;
}

int detect_memory_hooks() {
    show_loading("MEMORY HOOKS");
    int detected = 0;
    
    void* fopen_addr = dlsym(RTLD_NEXT, "fopen");
    unsigned char* bytes = (unsigned char*)fopen_addr;
    if(bytes[0] == 0xE9 || bytes[0] == 0xE8) {
        printf(RED "[✗] fopen() function hooked\n" RESET);
        detected = 1;
    }
    
    printf(detected ? RED "[!] MEMORY HOOKS FOUND\n" RESET : GREEN "[✓] No memory hooks\n" RESET);
    return detected;
}

int detect_emulator() {
    show_loading("EMULATOR CHECK");
    int detected = 0;
    
    char value[PROP_VALUE_MAX];
    __system_property_get("ro.hardware", value);
    if(strstr(value, "goldfish") || strstr(value, "ranchu")) {
        printf(RED "[✗] Emulator hardware: %s\n" RESET, value);
        detected = 1;
    }
    
    printf(detected ? RED "[!] RUNNING IN EMULATOR\n" RESET : GREEN "[✓] Real device\n" RESET);
    return detected;
}

// ================ MAIN ================ //
int main() {
    printf(PURPLE "\n\n██████╗ █████╗ ███████╗███████╗███████╗\n");
    printf(PURPLE "██╔══██╗██╔══██╗██╔════╝██╔════╝██╔════╝\n");
    printf(PURPLE "██████╔╝███████║███████╗███████╗███████╗\n");
    printf(PURPLE "██╔══██╗██╔══██║╚════██║╚════██║╚════██║\n");
    printf(PURPLE "██████╔╝██║  ██║███████║███████║███████║\n");
    printf(PURPLE "╚═════╝ ╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝\n\n" RESET);

    int total_detected = 0;
    
    total_detected += detect_magisk();
    total_detected += detect_superuser();
    total_detected += detect_xposed();
    total_detected += detect_frida();
    total_detected += detect_safetynet();
    total_detected += detect_memory_hooks();
    total_detected += detect_emulator();
    
    printf(CYAN "\n╔════════════════════════════════════════════╗\n");
    if(total_detected > 0) {
        printf(CYAN "║ " RED "TOTAL %d ANOMALIES DETECTED!             ║\n", total_detected);
        printf(CYAN "║ " RED "DEVICE COMPROMISED!                      ║\n");
    } else {
        printf(CYAN "║ " GREEN "DEVICE CLEAN - NO THREATS FOUND        ║\n");
    }
    printf(CYAN "╚════════════════════════════════════════════╝\n" RESET);

    return 0;
}
