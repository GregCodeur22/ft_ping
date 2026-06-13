#include "../includes/ft_ping.h"

uint16_t   icmp_checksum(void *data, int len)
{
    uint32_t sum = 0;
    uint16_t *ptr = data;

    while (len > 1)
    {
        sum += *ptr++;
        len -= 2;
    }
    if (len == 1)
        sum += *(uint8_t *)ptr;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    return (uint16_t)(~sum);
}


int build_icmp_paquet(char *buff, int seq, int pid, int size, struct icmphdr *icmp)
{
    int total = sizeof(struct icmphdr) + size;
    struct icmphdr *hdr = (struct icmphdr *)buff;

    hdr->type = ICMP_ECHO;
    hdr->code = 0;
    hdr->un.echo.id = htons(pid);
    hdr->un.echo.sequence = htons(seq);
    hdr->checksum = 0;

    if (size >= (int)sizeof(struct timeval))
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        memcpy(buff + sizeof(struct icmphdr), &tv, sizeof(struct timeval));
        int offset = sizeof(struct icmphdr) + sizeof(struct timeval);
        for (int i = offset; i < total; i++)
            buff[i] = 'i';
    }

    hdr->checksum = icmp_checksum(buff, total);

    if (icmp)
        memcpy(icmp, hdr, sizeof(struct icmphdr));

    return (0);
}