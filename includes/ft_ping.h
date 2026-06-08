#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/ip_icmp.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include <netinet/ip.h>

#define PAYLOAD_SIZE 56

typedef struct s_config
{
    char *target;
    int n;
    int w;
    int s;
    int W;
    int v;
} t_config;


typedef struct s_ping
{
    struct  sockaddr_in addr;
    char    ip_str[16];
    int     socket_fd;
    int     sequence;
    int     pid;
    int     packets_transmitted;
    int     packets_received;
    double  rtt_min;   // Stocke le temps le plus court
    double  rtt_max;   // Stocke le temps le plus long
    double  rtt_total; // Stocke la somme de tous les temps
    struct timeval start_time;
} t_ping;

t_config parse_args(int ac, char **av);
int resolve_target(t_ping *ping, char *target);
int build_icmp_paquet(char *buff, int seq, int pid, struct icmphdr *icmp);
void send_paquet(t_ping *ping, t_config *config);
void recv_paquet(t_ping *ping, t_config *config);

