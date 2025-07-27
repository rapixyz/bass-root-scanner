#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>

// ================ WARNA EKSTRIM ================ //
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
        usleep(100000); // 1.5 detik total
    }
    
    printf(BLUE "║\n╚════════════════════════════════════════════╝\n" RESET);
}

// ================ DETEKSI UTAMA ================ //

// [1] MAGISK & MODULE DETECTION
int check_magisk() {
    show_loading("SCANNING MAGISK");
    char* magisk_paths[] = {
        "/sbin/.magisk", "/data/adb/magisk", 
        "/cache/.disable_magisk", "/data/magisk",
        "/data/adb/modules"
    };
    
    for(int i=0; i<5; i++) {
        if(access(magisk_paths[i], F_OK) == 0) {
            printf(RED "[✗] Magisk: %s (DETECTED!)\n" RESET, magisk_paths[i]);
            return 1;
        }
    }
    printf(GREEN "[✓] Magisk: Clean\n" RESET);
    return 0;
}

// [2] SUPERUSER BINARY CHECK
int check_su_binary() {
    show_loading("CHECKING SU BINARY");
    char* su_paths[] = {
        "/system/bin/su", "/system/xbin/su",
        "/sbin/su", "/data/local/su",
        "/system/bin/.ext/su"
    };
    
    for(int i=0; i<5; i++) {
        if(access(su_paths[i], F_OK) == 0) {
            printf(RED "[✗] SU Binary: %s (DETECTED!)\n" RESET, su_paths[i]);
            return 1;
        }
    }
    printf(GREEN "[✓] SU Binary: Clean\n" RESET);
    return 0;
}

// [3] EMULATOR DETECTION
int check_emulator() {
    show_loading("DETECTING EMULATOR");
    FILE* fp = fopen("/proc/cpuinfo", "r");
    if(fp) {
        char line[256];
        while(fgets(line, sizeof(line), fp)) {
            if(strstr(line, "qemu") || strstr(line, "emulator")) {
                printf(RED "[✗] Emulator: QEMU Detected!\n" RESET);
                fclose(fp);
                return 1;
            }
        }
        fclose(fp);
    }
    printf(GREEN "[✓] Emulator: Not Detected\n" RESET);
    return 0;
}

// [4] KERNELSU DETECTION
int check_kernelsu() {
    show_loading("SCANNING KERNELSU");
    if(access("/data/adb/ksu", F_OK) == 0 || 
       access("/system/bin/ksud", F_OK) == 0) {
        printf(RED "[✗] KernelSU: Detected!\n" RESET);
        return 1;
    }
    printf(GREEN "[✓] KernelSU: Clean\n" RESET);
    return 0;
}

// [5] APATCH DETECTION
int check_apatch() {
    show_loading("SCANNING APATCH");
    if(access("/data/adb/ap", F_OK) == 0 || 
       access("/data/adb/apd", F_OK) == 0) {
        printf(RED "[✗] APatch: Detected!\n" RESET);
        return 1;
    }
    printf(GREEN "[✓] APatch: Clean\n" RESET);
    return 0;
}

// [6] XPOSED DETECTION
int check_xposed() {
    show_loading("CHECKING XPOSED");
    if(dlopen("libxposed_art.so", RTLD_NOW) != NULL || 
       access("/system/framework/XposedBridge.jar", F_OK) == 0) {
        printf(RED "[✗] Xposed: Detected!\n" RESET);
        return 1;
    }
    printf(GREEN "[✓] Xposed: Clean\n" RESET);
    return 0;
}

// [7] FRIDA DETECTION
int check_frida() {
    show_loading("SCANNING FRIDA");
    if(dlopen("libfrida-gadget.so", RTLD_NOW) != NULL || 
       access("/data/local/tmp/re.frida.server", F_OK) == 0) {
        printf(RED "[✗] Frida: Detected!\n" RESET);
        return 1;
    }
    printf(GREEN "[✓] Frida: Clean\n" RESET);
    return 0;
}

// [8] SELINUX STATUS
int check_selinux() {
    show_loading("CHECKING SELINUX");
    FILE* fp = popen("getenforce", "r");
    if(fp) {
        char status[16];
        if(fgets(status, sizeof(status), fp)) {
            if(strstr(status, "Permissive") || strstr(status, "Disabled")) {
                printf(RED "[✗] SELinux: %s (WEAK!)\n" RESET, status);
                pclose(fp);
                return 1;
            }
        }
        pclose(fp);
    }
    printf(GREEN "[✓] SELinux: Enforcing\n" RESET);
    return 0;
}

// [9] BANKING TROJAN SCAN
int check_banking_trojans() {
    show_loading("SCANNING BANKING TROJANS");
    char* trojans[] = {
        "com.cerberus", "com.spyware.banker",
        "com.gustavo.dropper", "com.ahmad.root"
    };
    
    for(int i=0; i<4; i++) {
        char cmd[128];
        sprintf(cmd, "pm list packages | grep %s", trojans[i]);
        FILE* fp = popen(cmd, "r");
        if(fp && fgetc(fp) != EOF) {
            printf(RED "[✗] Trojan: %s Found!\n" RESET, trojans[i]);
            pclose(fp);
            return 1;
        }
        pclose(fp);
    }
    printf(GREEN "[✓] Banking Trojans: Clean\n" RESET);
    return 0;
}

// [10] FAKE GPS DETECTION
int check_fake_gps() {
    show_loading("CHECKING FAKE GPS");
    if(access("/data/mockgps", F_OK) == 0 || 
       access("/system/app/FakeGps", F_OK) == 0) {
        printf(RED "[✗] Fake GPS: Detected!\n" RESET);
        return 1;
    }
    printf(GREEN "[✓] Fake GPS: Clean\n" RESET);
    return 0;
}

// ================ MAIN FUNCTION ================ //
int main() {
    printf(PURPLE "\n\n██████╗ █████╗ ███████╗███████╗███████╗\n");
    printf(PURPLE "██╔══██╗██╔══██╗██╔════╝██╔════╝██╔════╝\n");
    printf(PURPLE "██████╔╝███████║███████╗███████╗███████╗\n");
    printf(PURPLE "██╔══██╗██╔══██║╚════██║╚════██║╚════██║\n");
    printf(PURPLE "██████╔╝██║  ██║███████║███████║███████║\n");
    printf(PURPLE "╚═════╝ ╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝\n\n" RESET);

    int detected = 0;
    detected += check_magisk();
    detected += check_su_binary();
    detected += check_emulator();
    detected += check_kernelsu();
    detected += check_apatch();
    detected += check_xposed();
    detected += check_frida();
    detected += check_selinux();
    detected += check_banking_trojans();
    detected += check_fake_gps();

    printf(CYAN "\n╔════════════════════════════════════════════╗\n");
    if(detected > 0) {
        printf(CYAN "║ " RED "FINAL: %d ROOT METHODS DETECTED!          ║\n", detected);
        printf(CYAN "║ " RED "HP LU KEBANYAKAN MODIF! WKWKWK NTT! 😈    ║\n");
    } else {
        printf(CYAN "║ " GREEN "FINAL: HP AMAN, NO ROOT DETECTED!       ║\n");
        printf(CYAN "║ " GREEN "ATAU PAKAI MANTAP INVISIBLE LEVEL 99!   ║\n");
    }
    printf(CYAN "╚════════════════════════════════════════════╝\n" RESET);

    return 0;
}
