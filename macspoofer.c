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
#include <sys/stat.h>
#include <unistd.h>

typedef unsigned char int8;
const char *my_path = "/home/cyberhunter/Desktop/MacSpoofer";

unsigned char revert[6];
struct s_mac {
  unsigned char addr[6];
};

typedef struct s_mac Mac;

void parse_oui(const char *oui, unsigned char addr[6]) {
  sscanf(oui, "%2hhx:%2hhx:%2hhx", &addr[0], &addr[1], &addr[2]);
}

void set_p_mac(const char *interface) {
  char file[256];
  char a[18];
  sprintf(file, "%s/p_mac/%s.txt", my_path, interface);
  struct stat buffer;
  FILE *mp;
  char mpath[1024];

  snprintf(mpath, sizeof(mpath), "/sys/class/net/%s/address", interface);
  mp = fopen(mpath, "r");
  if (mp == NULL) {
    perror("Unable to open the class file");
    exit(EXIT_FAILURE);
  }
  if (fgets(a, 18, mp) == NULL) {
    perror("Error while reading MAC address");
    exit(EXIT_FAILURE);
  }
  a[strcspn(a, "\n")] = '\0';
  sscanf(a, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx", &revert[0], &revert[1],
         &revert[2], &revert[3], &revert[4], &revert[5]);

  if (stat(file, &buffer) != 0) {
    FILE *store;
    store = fopen(file, "w");
    if (store == NULL) {
      perror("Unable to open file for writing");
      exit(EXIT_FAILURE);
    }
    fprintf(store, "%s", a);
    fclose(store);
  }

  fclose(mp);
  return;
}

Mac generatemac(const char *filename) {
  Mac mac;

  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    perror("Error while opening OUI file");
    exit(EXIT_FAILURE);
  }
  int line_count = 0;
  char line[128];
  while (fgets(line, sizeof(line), file)) {
    line_count++;
  }

  rewind(file);

  int random_line = rand() % line_count;
  for (int i = 0; i <= random_line; i++) {
    fgets(line, sizeof(line), file);
  }

  line[strcspn(line, "\n")] = '\0';

  parse_oui(line, mac.addr);

  for (int i = 3; i < 6; i++) {
    mac.addr[i] = rand() % 256;
  }
  fclose(file);
  return mac;
}

Mac getUserMac(const char *value) {
  Mac mac;
  int result =
      sscanf(value,"%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx", &mac.addr[0], &mac.addr[1],
            &mac.addr[2], &mac.addr[3], &mac.addr[4], &mac.addr[5]);

  if (result != 6) {
    fprintf(stderr, "Invalid MAC address format.\n");
    exit(EXIT_FAILURE);
  }

  return mac;
}

Mac revert_mac(const char *interface) {
  Mac mac;
  char file[256];
  char a[18];
  sprintf(file, "%s/p_mac/%s.txt", my_path, interface);
  struct stat buffer;
  if (stat(file, &buffer) != 0) {
    perror("Your MAC is not yet spoofed..\n");
    exit(EXIT_FAILURE);
  }
  FILE *mp;
  mp = fopen(file, "r");
  if (mp == NULL) {
    perror("Unable to open the class file");
    exit(EXIT_FAILURE);
  }
  if (fgets(a, 18, mp) == NULL) {
    perror("Error while reading MAC address");
    exit(EXIT_FAILURE);
  }
  a[strcspn(a, "\n")] = '\0';
  sscanf(a, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx", &mac.addr[0], &mac.addr[1],
         &mac.addr[2], &mac.addr[3], &mac.addr[4], &mac.addr[5]);

  return mac;
}

bool chmac(const int8 *If, Mac mac) {
  struct ifreq ir;
  int fd, ret;

  set_p_mac((const char *)If);

  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    perror("Socket creation failed ... retry...");
    return false;
  }
  strncpy(ir.ifr_name, (const char *)If, (IFNAMSIZ - 1));
  ir.ifr_hwaddr.sa_family = ARPHRD_ETHER;
  memcpy(ir.ifr_hwaddr.sa_data, mac.addr, 6);

  ret = ioctl(fd, SIOCSIFHWADDR, &ir);
  close(fd);

  if (ret < 0) {
    perror("Failed to change MAC address");
    return false;
  }

  return true;
}

void display_help(const char *program){
  printf("Usage: sudo %s INTERFACE {-m [value], -r , -p, -h}\n",program);
  printf("\nOptions:\n");
  printf(" -m [value]  Set spoofed MAC address manually.\n");
  printf(" -r          Set Random MAC address\n");
  printf(" -p          Restore the original MAC address.\n");
  printf(" -h          Display this help message.\n");
}

int main(int argc, char *argv[]) {
  srand(getpid());
  const int8 *If;

  if (argc < 3) {
    fprintf(stderr, "Error: Too few arguments \n");
    display_help(argv[0]);
    return -1;
  } 
  If = (const int8 *)argv[1];
  int opt;
  const char *filename = "Database/filtered_db.txt";
  Mac ad;
  const char *mac_value=NULL;
  while((opt = getopt(argc-1,argv+1,"m:rph"))!=-1){
    switch(opt){
      case 'm':
        mac_value=optarg;
        ad=getUserMac(mac_value);
        break;

      case 'r':
        ad=generatemac(filename);
        break;

      case 'p':
        ad=revert_mac((const char *)If);
        break;

      case 'h':
        display_help(argv[0]);
        return 0;

      default:
        fprintf(stderr,"Error: Invalid option.\n");
        display_help(argv[0]);
        return 1;
        
    }
  }

  

  char command[128];
  snprintf(command, sizeof(command), "sudo ip link set %s down", If);

  if (system(command) != 0) {
    fprintf(stderr, "Failed to bring interface down\n");
    return -1;
  }

  if (chmac(If, ad)) {
    printf("Current MAC address : %02x:%02x:%02x:%02x:%02x:%02x\n", revert[0],
           revert[1], revert[2], revert[3], revert[4], revert[5]);

    printf("New MAC address     : %02x:%02x:%02x:%02x:%02x:%02x\n", ad.addr[0],
           ad.addr[1], ad.addr[2], ad.addr[3], ad.addr[4], ad.addr[5]);
  } else {
    fprintf(stderr, "Failed to spoof MAC address\n");
  }

  snprintf(command, sizeof(command), "sudo ip link set %s up", If);
  if (system(command) != 0) {
    fprintf(stderr, "Failed to bring interface up \n");
    return -1;
  }
  printf("Ready to go...");
  return 0;
}
