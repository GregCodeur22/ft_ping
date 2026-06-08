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
    struct icmphdr *hdr = (struct icmphdr *)buff;

    // 1. Forge de l'en-tête (8 octets)
    hdr->type = ICMP_ECHO;
    hdr->code = 0;
    hdr->un.echo.id = htons(pid);
    hdr->un.echo.sequence = htons(seq);
    hdr->checksum = 0;

    // 2. Injection du Timestamp au début du payload (octets 8 à 24)
    struct timeval tv;
    gettimeofday(&tv, NULL);
    // On copie la structure tv juste après l'en-tête ICMP dans le buffer
    memcpy(buff + sizeof(struct icmphdr), &tv, sizeof(struct timeval));

    // 3. Remplissage du reste du payload avec des données bidons (optionnel)
    // On commence après le timestamp
    int offset = sizeof(struct icmphdr) + sizeof(struct timeval);
    for (int i = offset; i < 64; i++)
    {
        buff[i] = 'i'; // Remplissage d'octets purement fictifs
    }

    // 4. Calcul du Checksum sur la TOTALITÉ du paquet (Header + Payload complet)
    hdr->checksum = icmp_checksum(buff, 64);

    // Optionnel : si ton main a besoin d'une copie pour le debug
    if (icmp)
        memcpy(icmp, hdr, sizeof(struct icmphdr));

    return (0);
}