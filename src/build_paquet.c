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


int build_icmp_paquet(char *buff, int seq, int pid, struct icmphdr *icmp)
{
    int paquet_size = sizeof(struct icmphdr) + PAYLOAD_SIZE;
    struct icmphdr *hdr = (struct icmphdr *)buff;

    memset(buff, 0, paquet_size);

    hdr->type = ICMP_ECHO;
    hdr->code = 0;
    hdr->un.echo.id = htons(pid);
    hdr->un.echo.sequence = htons(seq);
    hdr->checksum = 0;

    char *payload = buff + sizeof(struct icmphdr);
    for (int i = 0; i < PAYLOAD_SIZE; i++)
        payload[i] = (char)i;

    hdr->checksum = icmp_checksum(buff, paquet_size);
    if (icmp != NULL)
        *icmp = *hdr;
    return (paquet_size);
}