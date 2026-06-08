#include "../includes/ft_ping.h"

int resolve_target(t_ping *ping, char *target)
{
    struct addrinfo hints;
    struct addrinfo *res;

    // 1. Initialisation des filtres
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // On force l'IPv4
    hints.ai_socktype = SOCK_RAW;    // On cible une raw socket

    // 2. getaddrinfo valide l'IP ou résout le DNS automatiquement
    if (getaddrinfo(target, NULL, &hints, &res) != 0)
    {
        fprintf(stderr, "ft_ping: %s: Nom ou service inconnu\n", target);
        return (1);
    }

    // 3. Extraction directe du premier résultat IPv4 trouvé
    ping->addr = *(struct sockaddr_in *)res->ai_addr;

    // 4. Copie de l'IP sous forme textuelle pour tes futurs printf
    inet_ntop(AF_INET, &(ping->addr.sin_addr), ping->ip_str, sizeof(ping->ip_str));

    // 5. Libération de la mémoire système
    freeaddrinfo(res);
    
    return (0);
}