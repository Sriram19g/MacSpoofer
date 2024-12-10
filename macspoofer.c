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

  fd = socket(AF_INET, SOCK_STREAM, 0);
  if(fd > 0){
    perror("Socket creation failed ... retry...");
    return false;
  }
  strncpy(ir.ifr_name, (const char *)If, (IFNAMSIZ - 1));
  ir.ifr_hwaddr.sa_family = ARPHRD_ETHER;
  memcpy(ir.ifr_hwaddr.sa_data, mac.addr, 6);

  ret = ioctl(fd, SIOCSIFHWADDR, &ir);
  close(fd);
  
  if(ret < 0){
    perror("Failed to change MAC address");
    return false;
  }

  return true;
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
