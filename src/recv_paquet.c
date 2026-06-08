#include "../includes/ft_ping.h"

void recv_paquet(t_ping *ping, t_config *config)
{
    int buffer_size = 20 + sizeof(struct icmphdr) + config->s;
    char buff[buffer_size];
    struct iovec iov[1];
    struct msghdr msg;
    
    iov[0].iov_base = buff;
    iov[0].iov_len = buffer_size;

    struct sockaddr_in from;
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = &from;
    msg.msg_namelen = sizeof(from);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    int bytes_received = recvmsg(ping->socket_fd, &msg, 0);

    if (bytes_received < 0)
    {
        return;
    }

    struct ip *ip_hdr = (struct ip *)buff;
    int ip_hdr_len = ip_hdr->ip_hl * 4;
    
    struct icmphdr *icmp_hdr = (struct icmphdr *)(buff + ip_hdr_len);

    if (icmp_hdr->type == ICMP_ECHOREPLY)
    {
        if (ntohs(icmp_hdr->un.echo.id) == ping->pid)
        {
            ping->packets_received++;

            struct timeval tv_receive;
            gettimeofday(&tv_receive, NULL);

            struct timeval tv_send;
            memcpy(&tv_send, (char *)icmp_hdr + sizeof(struct icmphdr), sizeof(struct timeval));

            double rtt = (tv_receive.tv_sec - tv_send.tv_sec) * 1000.0 +
                         (tv_receive.tv_usec - tv_send.tv_usec) / 1000.0;

            if (ping->packets_received == 1)
            {
                ping->rtt_min = rtt;
                ping->rtt_max = rtt;
            }
            else
            {
                if (rtt < ping->rtt_min) ping->rtt_min = rtt;
                if (rtt > ping->rtt_max) ping->rtt_max = rtt;
            }
            ping->rtt_total += rtt;

            char host[NI_MAXHOST];
            
            int ret = getnameinfo((struct sockaddr *)&from, sizeof(from),
                                  host, sizeof(host),
                                  NULL, 0, NI_NAMEREQD);
            
            if (ret == 0)
            {
                // Si la résolution inversée a réussi, on affiche "nom (IP)"
                printf("%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%.2f ms\n",
                       bytes_received - ip_hdr_len,
                       host,
                       ping->ip_str,
                       ntohs(icmp_hdr->un.echo.sequence),
                       ip_hdr->ip_ttl,
                       rtt
                );
            }
            else
            {
                // Si elle échoue (pas de nom associé), on fait comme avant : juste l'IP
                printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
                       bytes_received - ip_hdr_len,
                       ping->ip_str,
                       ntohs(icmp_hdr->un.echo.sequence),
                       ip_hdr->ip_ttl,
                       rtt
                );
            }
        }
    }
}