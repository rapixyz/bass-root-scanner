#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/system_properties.h>
#include <dirent.h>

void check_superuser() {
    // Check common su binaries
    char* su_paths[] = {"/system/bin/su", "/system/xbin/su", "/sbin/su", "/data/local/su"};
    for (int i = 0; i < 4; i++) {
        if (access(su_paths[i], F_OK) == 0) {
            printf("║ ‼️ File SU terdeteksi di %-16s ║\n", su_paths[i]);
            return 1;
        }
    }
    
    return 0;
}

void check_magisk() {
    // Check Magisk files
    char* magisk_paths[] = {"/sbin/magisk", "/data/adb/magisk", "/cache/.disable_magisk"};
    for (int i = 0; i < 3; i++) {
        if (access(magisk_paths[i], F_OK) == 0) {
            printf("║ ‼️ Magisk terdeteksi di %-16s ║\n", magisk_paths[i]);
            return 1;
        }
    }
    
    return 0;
}

void check_busybox() {
    // Check BusyBox (common in rooted devices)
    if (access("/system/xbin/busybox", F_OK) == 0) {
        printf("║ ‼️ BusyBox terdeteksi                   ║\n");
        return 1;
    }
    return 0;
}

void check_selinux() {
    // Check SELinux status
    char selinux[10];
    FILE* f = fopen("/sys/fs/selinux/enforce", "r");
    if (f) {
        fgets(selinux, sizeof(selinux), f);
        fclose(f);
        if (strcmp(selinux, "0") == 0) {
            printf("║ ‼️ SELinux Permissive                  ║\n");
            return 1;
        }
    }
    return 0;
}

int main() {
    int detected = 0;
    
    printf("\n╔══════════════════════════════════════════════════╗\n");
    printf("║          🛡️  DETEKTOR ROOT v3.0              ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");

    detected |= check_superuser();
    detected |= check_magisk();
    detected |= check_busybox();
    detected |= check_selinux();

    printf("╔══════════════════════════════════════════════════╗\n");
    if (detected) {
        printf("║          ❌ ROOT TERDETEKSI! HATI-HATI!        ║\n");
    } else {
        printf("║          ✅ SISTEM AMAN (Tidak terdeteksi/      ║\n");
        printf("║             Root di hide sangat dalam!)        ║\n");
    }
    printf("╚══════════════════════════════════════════════════╝\n");

    return detected;
}