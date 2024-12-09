/* altermac.c */ 
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int int64;

struct s_mac{
    int64 addr:48;
};

typedef struct s_mac Mac ;

Mac generatenic(){
    int64 a,b;
    Mac mac;

    a=(long)random();
    b=(long)random();
    mac.addr=((a*b) % 16777216);
    return mac;
}

int main(int argc, char *argv[]) {
    srand(getpid());
    
    Mac ad=generatenic();
    printf("%llx\n",(long long)ad.addr);

    return 0;
}

