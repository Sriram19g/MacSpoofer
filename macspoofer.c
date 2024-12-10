/* MacSpoofer.c */
#include <assert.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

typedef unsigned char int8;

struct s_mac {
  unsigned char addr[6];
};

typedef struct s_mac Mac;

Mac generatenic() {
  Mac mac;
  mac.addr[0] = 0x02;
  for (int i = 1; i < 6; i++) {
    mac.addr[i] = rand() % 256;
  }

  return mac;
}

bool chmac(int8 *If, Mac mac) {
  struct ifreq ir;
  int fd, ret;

  fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  assert(fd > 0);

  strncpy(ir.ifr_ifrn.ifrn_name, (char *)If, (IFNAMSIZ - 1));
  ir.ifr_ifru.ifru_hwaddr.sa_family = ARPHRD_ETHER;
  memcpy(ir.ifr_ifru.ifru_hwaddr.sa_data, &mac, 6);

  ret = ioctl(fd, SIOCSIFHWADDR, &ir);
  close(fd);

  return (!ret) ? true : false;
}

int main(int argc, char *argv[]) {
  srand(getpid());
  int8 *If;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s INTERFACE\n", *argv);
    return -1;
  } else
    If = (int8 *)argv[1];

  Mac ad = generatenic();
  if (chmac(If, ad)) {
    printf("%llx\n", (long long)ad.addr);
  }

  return 0;
}
