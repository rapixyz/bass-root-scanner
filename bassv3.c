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

#define X1 0x55AAFF00
#define X2 0x33445566
#define X3(c) ((((c)-'a'+13)%26)+'a')

static uint32_t Y1[] = {0x12,0x45,0x78,0xAB,0xCD,0xEF,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99};
static uint32_t Y2[] = {0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0,0x0F,0x1E,0x2D,0x3C,0x4B,0x5A,0x69,0x78};
static uint32_t Y3[] = {0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x11,0x22,0x33,0x44,0x55,0x66};

char* Z1(const uint8_t* e, size_t l) {
    char* r = malloc(l+1); 
    for(int i=0;i<l;i++) r[i] = (e[i]^(X1>>(i%24)))^(X2>>(8-(i%8)));
    r[l]=0; return r;
}

void show_progress(const char* name, int duration_ms) {
    const int total_blocks = 25;
    const int delay = duration_ms / total_blocks;
    
    printf("\x1B[1;35m\xE2\x95\x91 \x1B[1;36m%s [", name);
    fflush(stdout);
    
    for (int i = 0; i < total_blocks; i++) {
        usleep(delay * 1000);
        printf("\x1B[1;33m\xE2\x96\x88");
        fflush(stdout);
        
        if (i < total_blocks - 1) {
            printf("\x1B[1;36m] %d%%", (i+1)*100/total_blocks);
            fflush(stdout);
            printf("\r\x1B[1;35m\xE2\x95\x91 \x1B[1;36m%s [", name);
            for (int j = 0; j <= i; j++) {
                printf("\x1B[1;33m\xE2\x96\x88");
            }
        }
    }
    
    printf("\x1B[1;36m] 100%%\x1B[0m\n");
}

void show_startup_animation() {
    const char* logo[] = {
        "\x1B[1;35m╔════════════════════════════════════════════╗",
        "\x1B[1;35m║  \x1B[1;36m▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓  \x1B[1;35m║",
        "\x1B[1;35m║  \x1B[1;36m▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓  \x1B[1;35m║",
        "\x1B[1;35m║  \x1B[1;33m  R O O T   D E T E C T O R   v 5.0    \x1B[1;35m║",
        "\x1B[1;35m║  \x1B[1;36m▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓  \x1B[1;35m║",
        "\x1B[1;35m╚════════════════════════════════════════════╝\x1B[0m"
    };
    
    for(int i=0; i<6; i++) {
        printf("%s\n", logo[i]);
        usleep(200000);
    }
    printf("\n");
}

void print_header() {
    printf("\x1B[1;35m╔════════════════════════════════════════════╗\n");
    printf("\x1B[1;35m║   \x1B[1;33m    R O O T   S C A N N E R   v5    \x1B[1;35m  ║\n");
    printf("\x1B[1;35m╚════════════════════════════════════════════╝\x1B[0m\n\n");
}

void print_footer(int detected) {
    printf("\x1B[1;35m╔════════════════════════════════════════════╗\n");
    if(detected) {
        printf("\x1B[1;35m║  \x1B[1;31m✗ ROOT DETECTED! SYSTEM COMPROMISED! \x1B[1;35m ║\n");
    } else {
        printf("\x1B[1;35m║   \x1B[1;32m✓ SCAN COMPLETE! NO ROOT FOUND    \x1B[1;35m  ║\n");
    }
    printf("\x1B[1;35m╚════════════════════════════════════════════╝\x1B[0m\n");
}

int count_detected(int* results, int count) {
    int total = 0;
    for(int i=0; i<count; i++) {
        if(results[i]) total++;
    }
    return total;
}

void calculate_risk_score(int detected_flags) {
    int risk_level = detected_flags * 20;
    printf("\x1B[1;35m╠════════════════════════════════════════════╣\n");
    printf("\x1B[1;35m║ \x1B[1;36mRisk Level: ");
    if(risk_level > 60) {
        printf("\x1B[1;31mCRITICAL (%d%%)", risk_level);
    } else if(risk_level > 30) {
        printf("\x1B[1;33mWARNING (%d%%)", risk_level);
    } else {
        printf("\x1B[1;32mLOW (%d%%)", risk_level);
    }
    printf("\x1B[1;35m ║\n");
}

void print_results_table(int* results, int count) {
    printf("\x1B[1;35m╔══════════════════════════════╦═════════════╗\n");
    printf("\x1B[1;35m║ \x1B[1;36mCheck Point           \x1B[1;35m║ \x1B[1;36mStatus    \x1B[1;35m║\n");
    printf("\x1B[1;35m╠══════════════════════════════╬═════════════╣\n");
    
    char* names[] = {"SU Binary", "Open Ports", "Boot State", "Processes", 
                    "Memory Maps", "Magisk", "SELinux", "Xposed", "System Integrity"};
    for(int i=0; i<count; i++) {
        printf("\x1B[1;35m║ \x1B[1;33m%-22s\x1B[1;35m║ ", names[i]);
        if(results[i]) {
            printf("\x1B[1;31m%-11s\x1B[1;35m║\n", "DETECTED");
        } else {
            printf("\x1B[1;32m%-11s\x1B[1;35m║\n", "CLEAN");
        }
    }
    printf("\x1B[1;35m╚══════════════════════════════╩═════════════╝\x1B[0m\n");
}

void log_results(int detected) {
    time_t now = time(NULL);
    FILE* log = fopen("root_scan.log", "a");
    if(log) {
        fprintf(log, "Scan at: %s", ctime(&now));
        fprintf(log, "Result: %s\n\n", detected ? "ROOT DETECTED" : "CLEAN");
        fclose(log);
    }
}

void A2() {
    char* p1 = Z1((uint8_t*)Y1,15);
    char* p2 = Z1((uint8_t*)Y2,15);
    printf("\n%s%s%s",p1,p2,"\x1B[0m");
    free(p1); free(p2);
}

int B2() {
    show_progress("Checking SU binary", 1500);
    
    uint8_t C2[] = {0xA1,0xB2,0xC3,0xD4,0xE5,0xF6};
    char* p = Z1(C2,6);
    int found = 0;
    if(access(p,F_OK)==0) {
        uint8_t m[] = {0x10,0x20,0x30,0x40,0x50};
        char* w = Z1(m,5);
        printf("\x1B[1;35m║ \x1B[1;31m✗ SU Binary: \x1B[1;33m%s\x1B[0m\n",w);
        free(w);
        found = 1;
    } else {
        printf("\x1B[1;35m║ \x1B[1;32m✓ SU Binary: \x1B[1;33mNot detected\x1B[0m\n");
    }
    free(p);
    return found;
}

int C3() {
    show_progress("Checking open ports", 1500);
    
    uint8_t D3[] = {0x31,0x32,0x33,0x34};
    int* P = (int*)Z1(D3,4);
    int found = 0;
    for(int i=0;i<4;i++) {
        int s = socket(AF_INET,SOCK_STREAM,0);
        if (s < 0) continue;
        
        struct sockaddr_in a = {0};
        a.sin_family = AF_INET;
        a.sin_port = htons(P[i]);
        
        if (inet_pton(AF_INET, "127.0.0.1", &a.sin_addr) <= 0) {
            close(s);
            continue;
        }

        if(connect(s,(struct sockaddr*)&a,sizeof(a))==0) {
            char o[32];
            snprintf(o,32,"%s%d",Z1((uint8_t[]){0x50,0x6F,0x72,0x74},4),P[i]);
            printf("\x1B[1;35m║ \x1B[1;31m✗ Open port: \x1B[1;33m%s\x1B[0m\n",o);
            found = 1;
        }
        close(s);
    }
    
    if (!found) {
        printf("\x1B[1;35m║ \x1B[1;32m✓ Open ports: \x1B[1;33mNo suspicious ports\x1B[0m\n");
    }
    
    free(P);
    return found;
}

int D4() {
    show_progress("Checking boot state", 1500);
    
    char K1[PROP_VALUE_MAX];
    __system_property_get("ro.boot.verifiedbootstate",K1);
    if(strcmp(K1,Z1((uint8_t[]){0x6F,0x72,0x61,0x6E,0x67,0x65},6))==0) {
        uint8_t L1[] = {0x40,0x50,0x60};
        char* M1 = Z1(L1,3);
        printf("\x1B[1;35m║ \x1B[1;31m✗ Boot state: \x1B[1;33m%s\x1B[0m\n",M1);
        free(M1);
        return 1;
    }
    
    printf("\x1B[1;35m║ \x1B[1;32m✓ Boot state: \x1B[1;33mNormal\x1B[0m\n");
    return 0;
}

int E5() {
    show_progress("Checking processes", 1500);
    
    DIR* O1 = opendir(Z1((uint8_t[]){0x2F,0x70,0x72,0x6F,0x63},5));
    if(!O1) {
        printf("\x1B[1;35m║ \x1B[1;31m✗ Process check: \x1B[1;33mFailed\x1B[0m\n");
        return 0;
    }
    
    int found = 0;
    struct dirent* P1;
    while((P1=readdir(O1))) {
        if(P1->d_type==DT_DIR && atoi(P1->d_name)>0) {
            char Q1[128];
            snprintf(Q1,128,"%s/%s/%s",Z1((uint8_t[]){0x2F,0x70,0x72,0x6F,0x63},5),P1->d_name,Z1((uint8_t[]){0x63,0x6D,0x64,0x6C,0x69,0x6E,0x65},7));
            FILE* R1 = fopen(Q1,"r");
            if(!R1) {
                uint8_t S1[] = {0x70,0x80,0x90};
                char* T1 = Z1(S1,3);
                printf("\x1B[1;35m║ \x1B[1;31m✗ Process cmdline: \x1B[1;33m%s\x1B[0m\n",T1);
                free(T1);
                found = 1;
                break;
            }
            fclose(R1);
        }
    }
    
    if (!found) {
        printf("\x1B[1;35m║ \x1B[1;32m✓ Processes: \x1B[1;33mNormal\x1B[0m\n");
    }
    
    closedir(O1);
    return found;
}

int F6() {
    show_progress("Checking memory maps", 1500);
    
    FILE *maps = fopen(Z1((uint8_t[]){0x2F,0x70,0x72,0x6F,0x63,0x2F,0x73,0x65,0x6C,0x66,0x2F,0x6D,0x61,0x70,0x73},15),"r");
    if (maps) {
        char line[256];
        while (fgets(line, sizeof(line), maps)) {
            if (strstr(line, Z1((uint8_t[]){0x6A,0x69,0x74},3)) || 
                strstr(line, Z1((uint8_t[]){0x6D,0x61,0x67,0x69,0x73,0x6B},6))) {
                printf("\x1B[1;35m║ \x1B[1;31m✗ Memory maps: \x1B[1;33m%s\x1B[0m\n",
                       Z1((uint8_t[]){0x5A,0x79,0x67,0x69,0x73,0x6B,0x20,0x48,0x69,0x64,0x64,0x65,0x6E},13));
                fclose(maps);
                return 1;
            }
        }
        fclose(maps);
    }
    
    printf("\x1B[1;35m║ \x1B[1;32m✓ Memory maps: \x1B[1;33mClean\x1B[0m\n");
    return 0;
}

int check_magisk() {
    show_progress("Scanning Magisk", 1500);
    char* paths[] = {"/sbin/.magisk", "/data/adb/magisk"};
    int found = 0;
    for(int i=0; i<2; i++) {
        if(access(paths[i], F_OK) == 0) {
            printf("\x1B[1;35m║ \x1B[1;31m✗ Magisk detected at: \x1B[1;33m%s\x1B[0m\n", paths[i]);
            found = 1;
        }
    }
    if(!found) {
        printf("\x1B[1;35m║ \x1B[1;32m✓ Magisk: \x1B[1;33mNot detected\x1B[0m\n");
    }
    return found;
}

void check_system_integrity() {
    show_progress("System Integrity", 1500);
    FILE* mount = fopen("/proc/mounts", "r");
    if(mount) {
        char line[256];
        while(fgets(line, sizeof(line), mount)) {
            if(strstr(line, "/system") && strstr(line, "rw")) {
                printf("\x1B[1;35m║ \x1B[1;31m✗ System mounted as RW!\x1B[0m\n");
                fclose(mount);
                return;
            }
        }
        fclose(mount);
    }
    printf("\x1B[1;35m║ \x1B[1;32m✓ System Integrity: \x1B[1;33mOK\x1B[0m\n");
}

void check_selinux() {
    show_progress("Checking SELinux", 1000);
    char* result = Z1((uint8_t[]){0x50,0x65,0x72,0x6D,0x69,0x73,0x73,0x69,0x76,0x65},10);
    FILE* fp = popen("getenforce", "r");
    if(fp) {
        char status[16];
        if(fgets(status, sizeof(status), fp)) {
            if(strstr(status, "Disabled") || strstr(status, "Permissive")) {
                printf("\x1B[1;35m║ \x1B[1;31m✗ SELinux: \x1B[1;33m%s\x1B[0m\n", result);
            } else {
                printf("\x1B[1;35m║ \x1B[1;32m✓ SELinux: \x1B[1;33mEnforcing\x1B[0m\n");
            }
        }
        pclose(fp);
    }
    free(result);
}

int check_xposed() {
    show_progress("Checking Xposed", 1500);
    void* handle = dlopen("libxposed_art.so", RTLD_NOW);
    if(handle) {
        printf("\x1B[1;35m║ \x1B[1;31m✗ Xposed Framework: \x1B[1;33mDetected!\x1B[0m\n");
        dlclose(handle);
        return 1;
    }
    printf("\x1B[1;35m║ \x1B[1;32m✓ Xposed Framework: \x1B[1;33mNot found\x1B[0m\n");
    return 0;
}

void G7() {
    show_startup_animation();
    print_header();
    
    int results[9] = {0};
    results[0] = B2();
    results[1] = C3();
    results[2] = D4();
    results[3] = E5();
    results[4] = F6();
    results[5] = check_magisk();
    check_selinux(); // Doesn't return value
    results[6] = check_xposed();
    check_system_integrity(); // Doesn't return value
    
    printf("\x1B[1;35m╠════════════════════════════════════════════╣\n");
    print_results_table(results, 7);
    calculate_risk_score(count_detected(results, 7));
    log_results(count_detected(results, 7));
    
    print_footer(count_detected(results, 7));
}

int main() {
    G7();
    return 0;
}
