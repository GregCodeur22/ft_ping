#include "../includes/ft_ping.h"

static void update_rtt(t_ping *ping, double rtt)
{
	ping->packets_received++;
	if (ping->packets_received == 1)
		ping->rtt_min = ping->rtt_max = rtt;
	else
	{
		if (rtt < ping->rtt_min) ping->rtt_min = rtt;
		if (rtt > ping->rtt_max) ping->rtt_max = rtt;
	}
	ping->rtt_total += rtt;
}

static void print_reply(int bytes, int ip_len, struct ip *ip_hdr,
						struct icmphdr *icmp, char *ip_str, double rtt)
{
	printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
		   bytes - ip_len, ip_str,
		   ntohs(icmp->un.echo.sequence), ip_hdr->ip_ttl, rtt);
}

static const char *icmp_error_str(int type)
{
	if (type == ICMP_DEST_UNREACH)  return "Destination Net Unreachable";
	if (type == ICMP_TIME_EXCEEDED) return "Time to live exceeded";
	return "Redirect";
}

static int handle_echo_reply(t_ping *ping, t_config *config, int bytes,
							int ip_len, struct ip *ip_hdr, struct icmphdr *icmp)
{
	if (ntohs(icmp->un.echo.id) != ping->pid)
		return 0;
	struct timeval tv_recv, tv_send;
	gettimeofday(&tv_recv, NULL);
	memcpy(&tv_send, (char *)icmp + sizeof(struct icmphdr), sizeof(tv_send));
	double rtt = (tv_recv.tv_sec  - tv_send.tv_sec)  * 1000.0
			   + (tv_recv.tv_usec - tv_send.tv_usec) / 1000.0;
	update_rtt(ping, rtt);
	if (!config->q)
		print_reply(bytes, ip_len, ip_hdr, icmp, ping->ip_str, rtt);
	return 1;
}

static int handle_icmp_error(t_ping *ping, t_config *config,
							struct icmphdr *icmp, struct sockaddr_in *from)
{
	struct ip      *inner_ip   = (struct ip *)((char *)icmp + sizeof(struct icmphdr));
	struct icmphdr *inner_icmp = (struct icmphdr *)((char *)inner_ip + inner_ip->ip_hl * 4);
	if (ntohs(inner_icmp->un.echo.id) != ping->pid)
		return 0;
	if (config->v && !config->q)
	{
		char from_ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &from->sin_addr, from_ip, sizeof(from_ip));
		printf("From %s: icmp_seq=%d icmp_type=%d icmp_code=%d (%s)\n",
			   from_ip, ntohs(inner_icmp->un.echo.sequence),
			   icmp->type, icmp->code, icmp_error_str(icmp->type));
	}
	return 1;
}

void recv_paquet(t_ping *ping, t_config *config)
{
	int				buffer_size = 20 + sizeof(struct icmphdr) + config->s;
	char			buff[buffer_size];
	struct sockaddr_in from;
	struct iovec	iov = {buff, buffer_size};
	struct msghdr	msg = {&from, sizeof(from), &iov, 1, NULL, 0, 0};

	while (1)
	{
		int bytes = recvmsg(ping->socket_fd, &msg, 0);
		if (bytes < 0)
			return;

		struct ip      *ip_hdr = (struct ip *)buff;
		int             ip_len = ip_hdr->ip_hl * 4;
		struct icmphdr *icmp   = (struct icmphdr *)(buff + ip_len);

		if (icmp->type == ICMP_ECHOREPLY
			&& handle_echo_reply(ping, config, bytes, ip_len, ip_hdr, icmp))
			return;
		if ((icmp->type == ICMP_DEST_UNREACH || icmp->type == ICMP_TIME_EXCEEDED
			|| icmp->type == ICMP_REDIRECT)
			&& handle_icmp_error(ping, config, icmp, &from))
			return;
	}
}
