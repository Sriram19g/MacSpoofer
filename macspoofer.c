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
    printf("%02x\n",mac.addr[i]);
  }

  return mac;
}
Mac getUserMac() {
    Mac mac;
    printf("Enter MAC address (format: XX:XX:XX:XX:XX:XX): ");
    int result = scanf(
        "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx", 
        &mac.addr[0], &mac.addr[1], &mac.addr[2],
        &mac.addr[3], &mac.addr[4], &mac.addr[5]
    );
    
    if (result != 6) {
        fprintf(stderr, "Invalid MAC address format.\n");
        exit(EXIT_FAILURE);
    }
    
    return mac;
}

int8 getoui() {
  return 0; 
}

bool chmac(const int8 *If, Mac mac) {
  struct ifreq ir;
  int fd, ret;

  fd = socket(AF_INET, SOCK_STREAM, 0);
  if(fd < 0){
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
  const int8 *If;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s INTERFACE\n", argv[0]);
    return -1;
  } else
    If = (const int8 *)argv[1];

  Mac ad = getUserMac();

  char command[128];
  snprintf(command,sizeof(command),"sudo ip link set %s down",If);

  if(system(command)!= 0){
    fprintf(stderr,"Failed to bring interface down\n");
    return -1;
  }

  if (chmac(If, ad)) {
    printf("MAC address changed to %02x:%02x:%02x:%02x:%02x:%02x\n",ad.addr[0],ad.addr[1],ad.addr[2],ad.addr[3],ad.addr[4],ad.addr[5]); 
  }
  else{
    fprintf(stderr, "Failed to spoof MAC address\n");
  }

  snprintf(command, sizeof(command),"sudo ip link set %s up", If);
  if(system(command)!=0){
    fprintf(stderr, "Failed to bring interface up \n");
    return -1;
  }
  printf("Ready to go...");
  return 0;
}
