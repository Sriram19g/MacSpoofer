/* MacSpoofer.c */
#define _GNU_SOURCE
#include <assert.h>
#include <errno.h>
#include <net/if.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <net/if_arp.h>

typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int int64;

struct s_mac {
  int64 addr : 48;
};

typedef struct s_mac Mac;

Mac generatenic() {
  int64 a, b;
  Mac mac;

  a = (long)random();
  b = (long)random();
  mac.addr = ((a * b) % 16777216);
  return mac;
}

bool chmac(int8 *If, Mac mac) {
  struct ifreq ir;
  int8 *If;
  int fd, ret;
  char *_ = "";

  fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  assert(fd > 0);

  strncpy(ir.ifr_ifrn.ifrn_name, If, (IFNAMSIZ - 1));
  ir.ifr_ifru.ifru_hwaddr.sa_family = ARPHRD_ETHER;
  strncpy(ir.ifr_ifru.ifru_hwaddr.sa_data, _, 18);

  ret = ioctl(fd, SIOCSIFHWADDR, ir);
  close(fd);

  return (!ret) ? true : false;
}

int main(int argc, char *argv[]) {
  srand(getpid());

  if (argc < 2) {
    fprintf(stderr, "Usage: %s INTERFACE\n", *argv);
    return -1;
  }

  Mac ad = generatenic();
  printf("%llx\n", (long long)ad.addr);

  return 0;
}

