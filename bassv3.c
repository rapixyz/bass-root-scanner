#define _GNU_SOURCE
#include <features.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/system_properties.h>
#include <dirent.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dlfcn.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <fcntl.h>

// ================ WARNA EKSTRIM! ================ //
#define RED "\x1B[1;31m"
#define BLUE "\x1B[1;34m"
#define CYAN "\x1B[1;36m"
#define PURPLE "\x1B[1;35m"
#define FLASH "\x1B[5;31m"
#define RESET "\x1B[0m"

// ================ EFIK SUARA ================ //
void play_sound(const char* type) {
    if(access("/usr/bin/play", F_OK) == 0) {
        if(strcmp(type, "alert") == 0) {
            system("play -nq -t alsa synth 0.3 sine 880 >/dev/null 2>&1");
        } else {
            system("play -nq -t alsa synth 0.2 sine 440 >/dev/null 2>&1");
        }
    }
}

// ================ ANIMASI BOMBASTIS ================ //
void show_loading(const char* text) {
    printf(BLUE "╔════════════════════════════════════════════╗\n");
    printf(BLUE "║ " FLASH "%s ", text);
    
    for(int i=0; i<15; i++) {
        printf(RED "■ " RESET);
        fflush(stdout);
        usleep(100000);
        play_sound("scan");
    }
    
    printf(BLUE "║\n╚════════════════════════════════════════════╝\n" RESET);
}

// ================ DETEKSI ROOT LEVEL GOD ================ //
int check_magisk() {
    show_loading("MAGISK SCAN");
    if(access("/sbin/.magisk", F_OK) == 0) {
        printf(PURPLE "╔════════════════════════════════════════════╗\n");
        printf(PURPLE "║ " RED "✗ MAGISK DETECTED! " FLASH "ROOT CONFIRMED!" PURPLE " ║\n");
        printf(PURPLE "╚════════════════════════════════════════════╝\n" RESET);
        play_sound("alert");
        return 1;
    }
    return 0;
}

int check_superuser() {
    show_loading("SU BINARY CHECK");
    if(access("/system/bin/su", F_OK) == 0) {
        printf(CYAN "╔════════════════════════════════════════════╗\n");
        printf(CYAN "║ " RED "✗ SUPERUSER BINARY FOUND! " FLASH "DANGER!" CYAN " ║\n");
        printf(CYAN "╚════════════════════════════════════════════╝\n" RESET);
        play_sound("alert");
        return 1;
    }
    return 0;
}

// ================ DETEKSI TAMBAHAN EKSTRIM ================ //
void check_emulator() {
    show_loading("EMULATOR SCAN");
    FILE* fp = fopen("/proc/cpuinfo", "r");
    if(fp) {
        char line[256];
        while(fgets(line, sizeof(line), fp)) {
            if(strstr(line, "qemu") || strstr(line, "emulator")) {
                printf(BLUE "╔════════════════════════════════════════════╗\n");
                printf(BLUE "║ " RED "✗ RUNNING ON EMULATOR! " FLASH "FAKE ENV!" BLUE " ║\n");
                printf(BLUE "╚════════════════════════════════════════════╝\n" RESET);
                play_sound("alert");
                break;
            }
        }
        fclose(fp);
    }
}

// ================ MAIN FUNCTION ================ //
int main() {
    printf(RED "\n\n██████╗  █████╗ ███████╗███████╗███████╗\n");
    printf(RED "██╔══██╗██╔══██╗██╔════╝██╔════╝██╔════╝\n");
    printf(RED "██████╔╝███████║███████╗███████╗███████╗\n");
    printf(RED "██╔══██╗██╔══██║╚════██║╚════██║╚════██║\n");
    printf(RED "██████╔╝██║  ██║███████║███████║███████║\n");
    printf(RED "╚═════╝ ╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝\n\n" RESET);
    
    play_sound("start");
    
    int is_root = 0;
    is_root |= check_magisk();
    is_root |= check_superuser();
    check_emulator();
    
    printf(PURPLE "╔════════════════════════════════════════════╗\n");
    if(is_root) {
        printf(PURPLE "║ " FLASH "FINAL: HP LU ROOT! " RED "TERTAWA WKWKWK!!11! " PURPLE "║\n");
        printf(PURPLE "║ " RED "HIDE GOBLOK, TAU SEMUA! " FLASH "HAHAHA" PURPLE "      ║\n");
    } else {
        printf(PURPLE "║ " CYAN "FINAL: HP AMAN, NO ROOT " BLUE "100%% CLEAN " PURPLE "║\n");
        printf(PURPLE "║ " BLUE "ATAU LU PAKAI " FLASH "MANTAP INVISIBLE!" PURPLE "   ║\n");
    }
    printf(PURPLE "╚════════════════════════════════════════════╝\n" RESET);
    
    play_sound(is_root ? "alert" : "scan");
    return 0;
}
