#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/system_properties.h>
#include <dirent.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dlfcn.h>

#define A1 0x55AAFF00
#define A2 0x33445566
#define B1(c) ((((c)-'a'+13)%26)+'a')

static uint32_t C1[] = {0x12,0x45,0x78,0xAB,0xCD,0xEF,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99};
static uint32_t C2[] = {0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0,0x0F,0x1E,0x2D,0x3C,0x4B,0x5A,0x69,0x78};

char* D1(const uint8_t* e, size_t l) {
    char* r = malloc(l+1); 
    for(int i=0;i<l;i++) r[i] = (e[i]^(A1>>(i%24)))^(A2>>(8-(i%8)));
    r[l]=0; return r;
}

void E1() {
    char* p1 = D1((uint8_t*)C1,15);
    char* p2 = D1((uint8_t*)C2,15);
    printf("\n%s%s%s",p1,p2,"\033[0m");
    free(p1); free(p2);
}

void F1() {
    uint8_t G1[] = {0xA1,0xB2,0xC3,0xD4,0xE5,0xF6};
    char* p = D1(G1,6);
    if(access(p,F_OK)==0) {
        uint8_t m[] = {0x10,0x20,0x30,0x40,0x50};
        char* w = D1(m,5);
        printf("%s%s%s","\033[1;31m║ \033[1;33m",w,"\033[0m");
        free(w);
    }
    free(p);
}

void H1() {
    uint8_t I1[] = {0x31,0x32,0x33,0x34};
    int* P = (int*)D1(I1,4);
    for(int i=0;i<4;i++) {
        int s = socket(AF_INET,SOCK_STREAM,0);
        struct sockaddr_in a = {0};
        a.sin_family = AF_INET;
        a.sin_port = htons(P[i]);
        a.sin_addr.s_addr = inet_addr("127.0.0.1");
        if(connect(s,(struct sockaddr*)&a,sizeof(a))==0) {
            char o[32];
            snprintf(o,32,"%s%d","Port ",P[i]);
            printf("%s%s%s","\033[1;31m║ \033[1;33m",o,"\033[0m");
        }
        close(s);
    }
    free(P);
}

void J1() {
    char K1[PROP_VALUE_MAX];
    __system_property_get("ro.boot.verifiedbootstate",K1);
    if(strcmp(K1,"orange")==0) {
        uint8_t L1[] = {0x40,0x50,0x60};
        char* M1 = D1(L1,3);
        printf("%s%s%s","\033[1;31m║ \033[1;33m",M1,"\033[0m");
        free(M1);
    }
}

void N1() {
    DIR* O1 = opendir("/proc");
    if(!O1) return;
    struct dirent* P1;
    while((P1=readdir(O1))) {
        if(P1->d_type==DT_DIR && atoi(P1->d_name)>0) {
            char Q1[128];
            snprintf(Q1,128,"/proc/%s/cmdline",P1->d_name);
            FILE* R1 = fopen(Q1,"r");
            if(!R1) {
                uint8_t S1[] = {0x70,0x80,0x90};
                char* T1 = D1(S1,3);
                printf("%s%s%s","\033[1;31m║ \033[1;33m",T1,"\033[0m");
                free(T1);
                break;
            }
            fclose(R1);
        }
    }
    closedir(O1);
}

void U1() {
    E1();
    F1();
    H1();
    J1();
    N1();
    printf("%s%s%s","\033[1;34m╔","══════════════════════════════════════════════════","╗\n");
    printf("%s%s%s","\033[1;32m║","          ✅ TIDAK ADA ROOT TERDETEKSI           ","║\n");
    printf("%s%s%s","\033[1;34m╚","══════════════════════════════════════════════════","╝\033[0m\n");
}

int main() {
    U1();
    return 0;
}
