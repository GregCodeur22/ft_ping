#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

typedef struct s_config
{
	char	*target;
	int		s;
	int		v;
	int		c;
	int		i;
	int		q;
	int		t;
}	t_config;

typedef struct s_ping
{
	struct sockaddr_in	addr;
	char				ip_str[16];
	int					socket_fd;
	int					sequence;
	int					pid;
	int					packets_transmitted;
	int					packets_received;
	double				rtt_min;
	double				rtt_max;
	double				rtt_total;
	struct timeval		start_time;
}	t_ping;

t_config	parse_args(int ac, char **av);
int			resolve_target(t_ping *ping, char *target);
int			build_icmp_paquet(char *buff, int seq, int pid, int size, struct icmphdr *icmp);
void		send_paquet(t_ping *ping, t_config *config);
void		recv_paquet(t_ping *ping, t_config *config);
