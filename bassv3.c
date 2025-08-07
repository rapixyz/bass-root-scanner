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

// Encrypted string storage
typedef struct {
    int key;
    const char* enc;
    char* dec;
} enc_str;

// Encrypted strings
#define ENC_STR_COUNT 12
static enc_str enc_strings[ENC_STR_COUNT] = {
    {0x7B, "\x1A\x1F\x1E\x59\x1C\x1B\x4C\x0D\x0D\x4C\x1A\x1F\x0C\x1E\x1B\x4C\x1C\x1F\x1A\x59\x1C\x1B\x4C\x1A\x1F\x1E\x59\x1C\x1B\x00", NULL},
    {0x55, "\x3E\x3F\x3C\x3D\x32\x77\x32\x3B\x3E\x37\x3C\x3D\x32\x3F\x3E\x77\x32\x3C\x3F\x3E\x37\x3C\x3D\x00", NULL},
    {0x42, "\x07\x00\x0B\x0A\x1D\x4C\x13\x0A\x17\x0E\x0B\x4C\x0D\x0A\x1D\x0E\x0B\x4C\x1C\x13\x0A\x17\x0E\x0B\x00", NULL},
    {0x63, "\x2A\x3D\x3E\x3F\x24\x69\x24\x27\x2A\x3D\x3E\x3F\x24\x3D\x3E\x69\x24\x3E\x3F\x2A\x3D\x3E\x00", NULL},
    {0x2F, "\x5C\x5D\x5E\x5F\x50\x15\x50\x53\x5C\x5D\x5E\x5F\x50\x5D\x5E\x15\x50\x5E\x5F\x5C\x5D\x5E\x00", NULL},
    {0x19, "\x78\x7F\x7E\x19\x7C\x7B\x0C\x6D\x6D\x0C\x78\x7F\x6C\x7E\x7B\x0C\x7C\x7F\x78\x19\x7C\x7B\x0C\x78\x7F\x7E\x19\x7C\x7B\x00", NULL},
    {0x34, "\x47\x40\x45\x42\x57\x14\x57\x54\x47\x40\x45\x42\x57\x40\x45\x14\x57\x45\x42\x47\x40\x45\x00", NULL},
    {0x5A, "\x10\x17\x16\x49\x1C\x1B\x5A\x0D\x0D\x5A\x10\x17\x0C\x16\x1B\x5A\x1C\x17\x10\x49\x1C\x1B\x5A\x10\x17\x16\x49\x1C\x1B\x00", NULL},
    {0x27, "\x4E\x49\x4C\x4B\x5E\x29\x5E\x5F\x4E\x49\x4C\x4B\x5E\x49\x4C\x29\x5E\x4C\x4B\x4E\x49\x4C\x00", NULL},
    {0x6D, "\x06\x01\x04\x03\x12\x6D\x12\x13\x06\x01\x04\x03\x12\x01\x04\x6D\x12\x04\x03\x06\x01\x04\x00", NULL},
    {0x3E, "\x70\x77\x76\x09\x7C\x7B\x0E\x6D\x6D\x0E\x70\x77\x6C\x76\x7B\x0E\x7C\x77\x70\x09\x7C\x7B\x0E\x70\x77\x76\x09\x7C\x7B\x00", NULL},
    {0x11, "\x62\x65\x64\x67\x72\x11\x72\x73\x62\x65\x64\x67\x72\x65\x64\x11\x72\x64\x67\x62\x65\x64\x00", NULL}
};

// String decryption function
char* decrypt_string(int index) {
    if (index < 0 || index >= ENC_STR_COUNT) return NULL;
    
    if (!enc_strings[index].dec) {
        const char* enc = enc_strings[index].enc;
        int len = strlen(enc);
        enc_strings[index].dec = malloc(len + 1);
        
        for (int i = 0; i < len; i++) {
            enc_strings[index].dec[i] = enc[i] ^ enc_strings[index].key;
        }
        enc_strings[index].dec[len] = '\0';
    }
    
    return enc_strings[index].dec;
}

// Fake scan animation with encrypted strings
void fake_scan_animation() {
    // Decrypt all strings first
    for (int i = 0; i < ENC_STR_COUNT; i++) {
        decrypt_string(i);
    }

    printf("\n+------------------------------------------+\n");
    printf("|  %-40s  |\n", decrypt_string(0)); // 
    printf("+------------------------------------------+\n");
    printf("| %-40s |\n", decrypt_string(1)); // STATUS : SCANNING (20-30 DETIK)
    printf("+------------------------------------------+\n");
    
    usleep(500000);
    printf("| %-40s |\n", decrypt_string(2)); // [✓] SYSTEM ANALYSIS DONE
    usleep(300000);
    printf("| %-40s |\n", decrypt_string(3)); // [✓] CHECKING FILES, SYMLINKS, BINARY
    usleep(400000);
    printf("| %-40s |\n", decrypt_string(4)); // [✓] DETECTING ROOT HIDING MODULES
    usleep(600000);
    printf("| %-40s |\n", decrypt_string(5)); // [!] ROOT DETECTED
    usleep(200000);
    printf("| %-40s |\n", decrypt_string(6)); // [!] MODULE: Zygisk, Shamiko, MagiskHide
    usleep(300000);
    printf("| %-40s |\n", decrypt_string(7)); // [!] INTEGRITY BYPASS DETECTED
    printf("+------------------------------------------+\n");
    printf("| %-40s |\n", decrypt_string(8)); // RESULT : DEVICE COMPROMISED (ROOT DETECTED)
    printf("+------------------------------------------+\n\n");
}



int detect_magisk() {
    int detected = 0;
    int hide_level = 0;
    
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
            detected = 1;
            hide_level = 1;
        }
    }
    
    FILE* mounts = fopen("/proc/self/mounts", "r");
    if(mounts) {
        char line[512];
        while(fgets(line, sizeof(line), mounts)) {
            if(strstr(line, "magisk") || strstr(line, "core/mirror")) {
                detected = 1;
                hide_level = 1;
            }
        }
        fclose(mounts);
    }
    
    char value[PROP_VALUE_MAX];
    __system_property_get("ro.boot.verifiedbootstate", value);
    if(strcmp(value, "orange") == 0) {
        detected = 1;
        hide_level = 2;
    }
    
    void* zygisk = dlopen("libzygisk.so", RTLD_NOW | RTLD_GLOBAL | RTLD_NOLOAD);
    if(zygisk) {
        detected = 1;
        hide_level = 2;
    }
    
    FILE* cmdline = fopen("/proc/self/cmdline", "r");
    if(cmdline) {
        char cmd[256];
        if(fgets(cmd, sizeof(cmd), cmdline) {
            if(strstr(cmd, "magisk") || strstr(cmd, "zygisk")) {
                detected = 1;
                hide_level = 2;
            }
        }
        fclose(cmdline);
    }
    
    void* riru = dlopen("libriru.so", RTLD_NOW | RTLD_GLOBAL | RTLD_NOLOAD);
    if(riru) {
        detected = 1;
        hide_level = 2;
    }
    
    if(access("/data/adb/modules/shamiko", F_OK) == 0) {
        detected = 1;
        hide_level = 2;
    }
    
    void* lsplant = dlopen("liblsplant.so", RTLD_NOW | RTLD_GLOBAL | RTLD_NOLOAD);
    if(lsplant) {
        detected = 1;
        hide_level = 2;
    }
    
    return detected;
}

int detect_banking_bypass() {
    int detected = 0;
    
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
            detected = 1;
        }
    }
    
    FILE* xposed = fopen("/data/data/de.robv.android.xposed.installer/conf/modules.list", "r");
    if(xposed) {
        char line[256];
        while(fgets(line, sizeof(line), xposed)) {
            if(strstr(line, "bank") || strstr(line, "financial") || strstr(line, "safety")) {
                detected = 1;
            }
        }
        fclose(xposed);
    }
    
    void* libc = dlopen("libc.so", RTLD_NOW);
    if(libc) {
        typedef int (*getprop_func)(const char*, char*);
        getprop_func orig_getprop = (getprop_func)dlsym(libc, "__system_property_get");
        
        char value[PROP_VALUE_MAX];
        orig_getprop("ro.build.tags", value);
        if(strstr(value, "test-keys")) {
            detected = 1;
        }
        
        orig_getprop("ro.boot.verifiedbootstate", value);
        if(strcmp(value, "green") != 0) {
            detected = 1;
        }
        
        dlclose(libc);
    }
    
    void* fopen_addr = dlsym(RTLD_NEXT, "fopen");
    unsigned char* bytes = (unsigned char*)fopen_addr;
    if(bytes[0] == 0xE9 || bytes[0] == 0xE8) {
        detected = 1;
    }
    
    if(access("/data/data/com.google.android.gms/datasets/safetynet", F_OK) == 0) {
        detected = 1;
    }
    
    return detected;
}

// ================ MAIN FUNCTION ================ //

int main() {
    
    fake_scan_animation();
    
    // Run real detection (but ignore results)
    detect_magisk();
    detect_banking_bypass();
    
    // Always show compromised result
    printf("\n");
    printf("+------------------------------------------+\n");
    printf("| %-40s |\n", decrypt_string(9)); // FINAL: ROOT/BYPASS DETECTED!
    printf("| %-40s |\n", decrypt_string(10)); // APLIKASI BANK MUNGKIN DIBYPASS!
    printf("| %-40s |\n", decrypt_string(11)); // HP LU KEBANYAKAN MODIF! WKWKWK NTT!
    printf("+------------------------------------------+\n");

    return 0;
}

// Cleanup encrypted strings
void __attribute__((destructor)) cleanup() {
    for (int i = 0; i < ENC_STR_COUNT; i++) {
        if (enc_strings[i].dec) {
            free(enc_strings[i].dec);
            enc_strings[i].dec = NULL;
        }
    }
}
