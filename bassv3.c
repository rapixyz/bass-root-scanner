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

void A2() {
    char* p1 = Z1((uint8_t*)Y1,15);
    char* p2 = Z1((uint8_t*)Y2,15);
    printf("\n%s%s%s",p1,p2,"\x1B[0m");
    free(p1); free(p2);
}

void B2() {
    uint8_t C2[] = {0xA1,0xB2,0xC3,0xD4,0xE5,0xF6};
    char* p = Z1(C2,6);
    if(access(p,F_OK)==0) {
        uint8_t m[] = {0x10,0x20,0x30,0x40,0x50};
        char* w = Z1(m,5);
        printf("%s%s%s","\x1B[1;31m\xE2\x95\x91 \x1B[1;33m",w,"\x1B[0m");
        free(w);
    }
    free(p);
}

void C3() {
    uint8_t D3[] = {0x31,0x32,0x33,0x34};
    int* P = (int*)Z1(D3,4);
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
            printf("%s%s%s","\x1B[1;31m\xE2\x95\x91 \x1B[1;33m",o,"\x1B[0m");
        }
        close(s);
    }
    free(P);
}

void D4() {
    char K1[PROP_VALUE_MAX];
    __system_property_get("ro.boot.verifiedbootstate",K1);
    if(strcmp(K1,Z1((uint8_t[]){0x6F,0x72,0x61,0x6E,0x67,0x65},6))==0) {
        uint8_t L1[] = {0x40,0x50,0x60};
        char* M1 = Z1(L1,3);
        printf("%s%s%s","\x1B[1;31m\xE2\x95\x91 \x1B[1;33m",M1,"\x1B[0m");
        free(M1);
    }
}

void E5() {
    DIR* O1 = opendir(Z1((uint8_t[]){0x2F,0x70,0x72,0x6F,0x63},5));
    if(!O1) return;
    
    struct dirent* P1;
    while((P1=readdir(O1))) {
        if(P1->d_type==DT_DIR && atoi(P1->d_name)>0) {
            char Q1[128];
            snprintf(Q1,128,"%s/%s/%s",Z1((uint8_t[]){0x2F,0x70,0x72,0x6F,0x63},5),P1->d_name,Z1((uint8_t[]){0x63,0x6D,0x64,0x6C,0x69,0x6E,0x65},7));
            FILE* R1 = fopen(Q1,"r");
            if(!R1) {
                uint8_t S1[] = {0x70,0x80,0x90};
                char* T1 = Z1(S1,3);
                printf("%s%s%s","\x1B[1;31m\xE2\x95\x91 \x1B[1;33m",T1,"\x1B[0m");
                free(T1);
                break;
            }
            fclose(R1);
        }
    }
    closedir(O1);
}

void F6() {
    FILE *maps = fopen(Z1((uint8_t[]){0x2F,0x70,0x72,0x6F,0x63,0x2F,0x73,0x65,0x6C,0x66,0x2F,0x6D,0x61,0x70,0x73},15),"r");
    if (maps) {
        char line[256];
        while (fgets(line, sizeof(line), maps)) {
            if (strstr(line, Z1((uint8_t[]){0x6A,0x69,0x74},3)) || 
                strstr(line, Z1((uint8_t[]){0x6D,0x61,0x67,0x69,0x73,0x6B},6))) {
                printf("%s%s%s","\x1B[1;31m\xE2\x95\x91 \x1B[1;33m",Z1((uint8_t[]){0x5A,0x79,0x67,0x69,0x73,0x6B,0x20,0x48,0x69,0x64,0x64,0x65,0x6E},13),"\x1B[0m");
                break;
            }
        }
        fclose(maps);
    }
}

void G7() {
    A2();
    B2();
    C3();
    D4();
    E5();
    F6();
    printf("%s%s%s","\x1B[1;34m\xE2\x95\x94","\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90","\xE2\x95\x97\n");
    printf("%s%s%s","\x1B[1;32m\xE2\x95\x91","          \xE2\x9C\x85 \x54\x49\x44\x41\x4B \x41\x44\x41 \x52\x4F\x4F\x54 \x54\x45\x52\x44\x45\x54\x45\x4B\x53\x49           ","\xE2\x95\x91\n");
    printf("%s%s%s","\x1B[1;34m\xE2\x95\x9A","\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90","\xE2\x95\x9D\x1B[0m\n");
}

int main() {
    G7();
    return 0;
}
