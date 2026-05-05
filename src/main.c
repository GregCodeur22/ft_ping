#include "../includes/ft_ping.h"

int create_socket(void)
{
    int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (socket_fd < 0)
    {
        printf("socket fails\n");
        return -1;
    }
    return socket_fd;
}

int main(int ac, char **av)
{
    t_config config = parse_args(ac, av);
    t_ping ping;
    struct icmphdr icmp;

    resolve_target(&ping, config.target);
    ping.socket_fd = create_socket();
    if (ping.socket_fd < 0)
        return (1);

    //printf("socket fd: %d\n", ping.socket_fd);

    int seq = 0;
    char buff[64];
    int pid = getpid();

    build_icmp_paquet(buff, seq, pid, &icmp);

    // printf("type: %d\n", icmp.type);
    // printf("code: %d\n", icmp.code);
    // printf("id: %d\n", ntohs(icmp.un.echo.id));
    // printf("seq: %d\n", ntohs(icmp.un.echo.sequence));


        //send_paquet();
        //recv_paquet();
        //verify_icmp();
        //display_all();
        //sleep(1);
 
    return 0;
}