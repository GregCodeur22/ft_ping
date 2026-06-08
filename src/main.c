#include "../includes/ft_ping.h"
#include <signal.h>

int create_socket(void)
{
    int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (socket_fd < 0)
    {
        fprintf(stderr, "ft_ping: socket: %s\n", strerror(errno));        return -1;
    }
    return socket_fd;
}

void display_stats(t_ping *ping, t_config *config)
{
    printf("--- %s ping statistics ---\n", config->target);

    struct timeval end_time;
    gettimeofday(&end_time, NULL);

    double total_time = (end_time.tv_sec - ping->start_time.tv_sec) * 1000.0 +
                        (end_time.tv_usec - ping->start_time.tv_usec) / 1000.0;

    double loss = 0.0;
    if (ping->packets_transmitted > 0)
    {
        loss = ((double)(ping->packets_transmitted - ping->packets_received) / 
                ping->packets_transmitted) * 100.0;
    }

    printf("%d packets transmitted, %d received, %.0f%% packet loss, time %.0fms\n",
           ping->packets_transmitted,
           ping->packets_received,
           loss,
           total_time
    );

    if (ping->packets_received > 0)
    {
        double avg = ping->rtt_total / ping->packets_received;
        printf("rtt min/avg/max = %.3f/%.3f/%.3f ms\n",
               ping->rtt_min,
               avg,
               ping->rtt_max
        );
    }
}

int main(int ac, char **av)
{
    t_config config = parse_args(ac, av);
    t_ping ping;

    volatile sig_atomic_t g_stop = 0;
    void sig_handler(int sig)
    {
        (void)sig;
        g_stop = 1;
    }

    memset(&ping, 0, sizeof(t_ping));
    ping.pid = getpid();

    resolve_target(&ping, config.target);
    ping.socket_fd = create_socket();
    if (ping.socket_fd < 0)
        return (1);

    signal(SIGINT, sig_handler);
    printf("PING %s (%s) %d(%d) bytes of data.\n", 
       config.target, 
       ping.ip_str, 
       config.s, 
       config.s + 28); // 20 (IP) + 8 (ICMP)

    gettimeofday(&ping.start_time, NULL);

    while (!g_stop)
    {
        send_paquet(&ping, &config);
        ping.packets_transmitted++;
        recv_paquet(&ping, &config);
        ping.sequence++;
        sleep(1);
    }

    display_stats(&ping, &config);
    close(ping.socket_fd);



        //recv_paquet();
        //verify_icmp();
        //display_all();
        //sleep(1);
 
    return 0;
}