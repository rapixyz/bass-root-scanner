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

// ================ MAGISK DETECTION ================ //

int detect_magisk() {
    show_loading("MAGISK CHECK");
    int detected = 0;
    int hide_level = 0; // 0 = tidak hide, 1 = hide biasa, 2 = hide dalem
    
    // [1] Check Magisk files (basic detection)
    const char* magisk_paths[] = {
        "/sbin/.magisk",
        "/sbin/magisk",
        "/cache/.disable_magisk",
        "/cache/magisk.log",
        "/data/adb/magisk",
        "/data/adb/modules",
        NULL
    };
    
    for(int i = 0; magisk_paths[i]; i++) {
        if(access(magisk_paths[i], F_OK) == 0) {
            printf(RED "[!] MAGISK FILE DETECTED: %s\n" RESET, magisk_paths[i]);
            detected = 1;
            hide_level = 1;
        }
    }
    
    // [2] Check Magisk in mounts (more advanced)
    FILE* mounts = fopen("/proc/self/mounts", "r");
    if(mounts) {
        char line[512];
        while(fgets(line, sizeof(line), mounts)) {
            if(strstr(line, "magisk") || strstr(line, "core/mirror")) {
                printf(RED "[!] MAGISK MOUNT DETECTED: %s" RESET, line);
                detected = 1;
                hide_level = 1;
            }
        }
        fclose(mounts);
    }
    
    // [3] Check Magisk modules in props (very deep)
    char value[PROP_VALUE_MAX];
    __system_property_get("ro.boot.verifiedbootstate", value);
    if(strcmp(value, "orange") == 0) {
        printf(RED "[!] MAGISK BOOT STATE DETECTED\n" RESET);
        detected = 1;
        hide_level = 2;
    }
    
    // [4] Check Zygisk (most hidden)
    void* zygisk = dlopen("libzygisk.so", RTLD_NOW | RTLD_GLOBAL | RTLD_NOLOAD);
    if(zygisk) {
        printf(RED "[!] ZYGISK DETECTED (libzygisk.so)\n" RESET);
        detected = 1;
        hide_level = 2;
    }
    
    // [5] Check MagiskHide props (super hidden)
    FILE* cmdline = fopen("/proc/self/cmdline", "r");
    if(cmdline) {
        char cmd[256];
        if(fgets(cmd, sizeof(cmd), cmdline)) {
            if(strstr(cmd, "magisk") || strstr(cmd, "zygisk")) {
                printf(RED "[!] MAGISK PROCESS DETECTED: %s\n" RESET, cmd);
                detected = 1;
                hide_level = 2;
            }
        }
        fclose(cmdline);
    }
    
    // [6] Check for Riru (old school hiding)
    void* riru = dlopen("libriru.so", RTLD_NOW | RTLD_GLOBAL | RTLD_NOLOAD);
    if(riru) {
        printf(RED "[!] RIRU DETECTED (libriru.so)\n" RESET);
        detected = 1;
        hide_level = 2;
    }
    
    // [7] Check for Shamiko (new hiding method)
    if(access("/data/adb/modules/shamiko", F_OK) == 0) {
        printf(RED "[!] SHAMIKO MODULE DETECTED\n" RESET);
        detected = 1;
        hide_level = 2;
    }
    
    // [8] Check for LSPlant (newest hiding)
    void* lsplant = dlopen("liblsplant.so", RTLD_NOW | RTLD_GLOBAL | RTLD_NOLOAD);
    if(lsplant) {
        printf(RED "[!] LSPLANT DETECTED (liblsplant.so)\n" RESET);
        detected = 1;
        hide_level = 2;
    }
    
    // Print result based on hide level
    if(detected) {
        if(hide_level == 1) {
            printf(RED "\n[ROOT DETECT // HIDE KURANG DALAM]\n" RESET);
        } else {
            printf(RED "\n[ROOT KETAHUAN // MODULE HIDE DALEM BANGET]\n" RESET);
        }
    } else {
        printf(GREEN "\n[NO MAGISK DETECTED]\n" RESET);
    }
    
    return detected;
}

// ================ BANKING APPS BYPASS DETECTION ================ //

int detect_banking_bypass() {
    show_loading("BANKING BYPASS CHECK");
    int detected = 0;
    
    // [1] Check for known banking bypass modules
    const char* bypass_modules[] = {
        "/data/adb/modules/hide",
        "/data/adb/modules/magiskhide",
        "/data/adb/modules/safetynet",
        "/data/adb/modules/universal",
        "/data/adb/modules/zygisk",
        "/data/adb/modules/riru",
        NULL
    };
    
    for(int i = 0; bypass_modules[i]; i++) {
        if(access(bypass_modules[i], F_OK) == 0) {
            printf(RED "[!] BANKING BYPASS MODULE: %s\n" RESET, bypass_modules[i]);
            detected = 1;
        }
    }
    
    // [2] Check for Xposed modules targeting banking apps
    FILE* xposed = fopen("/data/data/de.robv.android.xposed.installer/conf/modules.list", "r");
    if(xposed) {
        char line[256];
        while(fgets(line, sizeof(line), xposed)) {
            if(strstr(line, "bank") || strstr(line, "financial") || strstr(line, "safety")) {
                printf(RED "[!] XPOSED BANKING MODULE: %s" RESET, line);
                detected = 1;
            }
        }
        fclose(xposed);
    }
    
    // [3] Check for modified API responses
    void* libc = dlopen("libc.so", RTLD_NOW);
    if(libc) {
        typedef int (*getprop_func)(const char*, char*);
        getprop_func orig_getprop = (getprop_func)dlsym(libc, "__system_property_get");
        
        char value[PROP_VALUE_MAX];
        orig_getprop("ro.build.tags", value);
        if(strstr(value, "test-keys")) {
            printf(RED "[!] MODIFIED BUILD TAGS: %s\n" RESET, value);
            detected = 1;
        }
        
        orig_getprop("ro.boot.verifiedbootstate", value);
        if(strcmp(value, "green") != 0) {
            printf(RED "[!] MODIFIED BOOT STATE: %s\n" RESET, value);
            detected = 1;
        }
        
        dlclose(libc);
    }
    
    // [4] Check for hooked security functions
    void* fopen_addr = dlsym(RTLD_NEXT, "fopen");
    unsigned char* bytes = (unsigned char*)fopen_addr;
    if(bytes[0] == 0xE9 || bytes[0] == 0xE8) {
        printf(RED "[!] SECURITY FUNCTION HOOKED (fopen)\n" RESET);
        detected = 1;
    }
    
    // [5] Check for fake SafetyNet attestation
    if(access("/data/data/com.google.android.gms/datasets/safetynet", F_OK) == 0) {
        printf(RED "[!] FAKE SAFETYNET DATA DETECTED\n" RESET);
        detected = 1;
    }
    
    if(detected) {
        printf(RED "\n[BANKING BYPASS DETECTED!]\n" RESET);
    } else {
        printf(GREEN "\n[NO BANKING BYPASS DETECTED]\n" RESET);
    }
    
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

    int magisk_detected = detect_magisk();
    int bypass_detected = detect_banking_bypass();
    
    printf(CYAN "\n╔════════════════════════════════════════════╗\n");
    if(magisk_detected || bypass_detected) {
        printf(CYAN "║ " RED "FINAL: ROOT/BYPASS DETECTED!              ║\n");
        if(bypass_detected) {
            printf(CYAN "║ " RED "APLIKASI BANK MUNGKIN DIBYPASS!        ║\n");
        }
        printf(CYAN "║ " RED "HP LU KEBANYAKAN MODIF! WKWKWK NTT! 😈    ║\n");
    } else {
        printf(CYAN "║ " GREEN "FINAL: HP GA KEDETECT                   ║\n");
        printf(CYAN "║ " GREEN "ATAU PAKAI MODULE HIDE SGT DALAM        ║\n");
    }
    printf(CYAN "╚════════════════════════════════════════════╝\n" RESET);

    return 0;
}
