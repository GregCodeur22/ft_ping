#include "../includes/ft_ping.h"

int convert_ip_to_sockaddr(t_ping *ping, char*ip_string)
{
    memset(&ping->addr, 0, sizeof(ping->addr));

    ping->addr.sin_family = AF_INET;
    ping->addr.sin_port = 0;

    int result = inet_pton(AF_INET, ip_string, &ping->addr.sin_addr);
    if (result != 1)
    {
        printf("inet_pton  a echouer\n");
        return (1);
    }
    printf("ip: %s sa fonctionne \n", inet_ntoa(ping->addr.sin_addr));
    return (0);
}

int convert_dns_binary(t_ping *ping, char *target)
{
    memset(&ping->addr, 0, sizeof(ping->addr));

    struct addrinfo hints;
    struct addrinfo *res = NULL;
    struct addrinfo *node = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_flags = 0;

    int result = getaddrinfo(target, NULL, &hints, &res);
    if (result != 0)
    {
        printf("getaddrinfo a echouer\n");
        return (1);
    }

    for (node = res; node != NULL; node = node->ai_next)
    {
        if (node->ai_family == AF_INET)
        {
            ping->addr = *(struct sockaddr_in *)node->ai_addr;
            printf("convert_dns_binary OK: %s\n", inet_ntoa(ping->addr.sin_addr));
            break;
        }
    }

    freeaddrinfo(res);

    if (node == NULL)
        return (1);

    return (0);
}

bool is_valid_ip(char *str)
{
    if (!str || *str == '\0')
    {
        printf("il n'y a pas de target\n");
        return false;
    }

    char *copy = strdup(str);
    if (!copy)
    {
        printf("la copie a un probleme\n");
        return false;
    }

    int bloc_len = 0;
    char *split = strtok(copy, ".");

    while (split != NULL)
    {
        int i = 0;

        if (split[0] == '\0')
        {
            printf("les blocs ne doivent pas etre vides\n");
            free(copy);
            return false;
        }

        while (split[i])
        {
            if (!isdigit((unsigned char)split[i]))
            {
                printf("les blocs doit contenir que des chiffres\n");
                free(copy);
                return false;
            }
            i++;
        }

        int value = atoi(split);
        if (value < 0 || value > 255)
        {
            printf("les blocs doit contenir des chiffre entre 0 et 255");
            free(copy);
            return false;
        }

        bloc_len++;
        split = strtok(NULL, ".");
    }

    if (bloc_len != 4)
    {
        printf("il faut 4 bloc pour passer le parsing\n");
        free(copy);
        return (false);
    }

    free(copy);

    return (true);
}

int resolve_target(t_ping *ping, char *target)
{
    if (is_valid_ip(target) == true)
    {
        printf("on entre dans is_valid_ip\n");
        convert_ip_to_sockaddr(ping, target);
    }
    else
    {
        convert_dns_binary(ping, target);
        printf("convert dns en binaire reussit\n");
    }

    return (0);
}