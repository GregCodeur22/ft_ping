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
#include <netinet/ip_icmp.h>

#define PAYLOAD_SIZE 56

typedef struct s_config
{
    char *target;
    int n;
    int w;
} t_config;



typedef struct s_ping
{
    struct sockaddr_in addr;
    int socket_fd;
} t_ping;

t_config parse_args(int ac, char **av);
int resolve_target(t_ping *ping, char *target);
int convert_ip_to_sockaddr(t_ping *ping, char *ip_string);
int build_icmp_paquet(char *buff, int seq, int pid, struct icmphdr *icmp);


// typedef struct s_ping
// {
//     // config utilisateur
//     t_config config;

//     // réseau
//     int socket_fd;
//     struct sockaddr_in addr;

//     // buffers
//     char send_buffer[64];
//     char recv_buffer[1024];

//     // timing
//     struct timeval time_start;
//     struct timeval time_end;

//     // stats
//     int seq;
//     int received;
// } t_ping;

