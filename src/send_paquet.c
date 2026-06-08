#include "../includes/ft_ping.h"

void send_paquet(t_ping *ping, t_config *config)
{
	int paquet_size = sizeof(struct icmphdr) + config->s;
	char buff[paquet_size];

	build_icmp_paquet(buff, ping->sequence, ping->pid, NULL);

	int bytes_sent = sendto(ping->socket_fd,
							buff,
							paquet_size,
							0,
							(struct sockaddr *)&ping->addr,
							sizeof(ping->addr));

	if (bytes_sent < 0)
	{
		fprintf(stderr, "ft_ping: sendto: Echec de l'envoie du paquet\n");
		return;
	}
}