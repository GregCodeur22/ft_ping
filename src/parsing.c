#include "../includes/ft_ping.h"

#define NO_MAX    0
#define TTL_MAX   255
#define SIZE_MAX  65507

static int is_number(char *str)
{
	int i = 0;

	if (!str || !str[i])
		return 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return 0;
		i++;
	}
	return 1;
}

static int parse_validated_int(char *arg, char **av, int *i, int ac, int min, int max)
{
	char	flag = arg[1];
	char	*val = &arg[2];
	int		n;

	if (*val != '\0')
	{
		if (!is_number(val))
		{
			printf("Error: invalid -%c value\n", flag);
			exit(1);
		}
		n = atoi(val);
	}
	else
	{
		if (*i + 1 >= ac || !is_number(av[*i + 1]))
		{
			printf("Error: -%c requires a number\n", flag);
			exit(1);
		}
		n = atoi(av[++(*i)]);
	}
	if (n < min || (max > NO_MAX && n > max))
	{
		if (max > NO_MAX)
			printf("Error: -%c value must be between %d and %d\n", flag, min, max);
		else
			printf("Error: -%c value must be >= %d\n", flag, min);
		exit(1);
	}
	return n;
}

t_config parse_args(int ac, char **av)
{
	t_config config;

	config.target = NULL;
	config.s = 56;
	config.v = 0;
	config.c = 0;
	config.i = 1;
	config.q = 0;
	config.t = 0;

	for (int i = 1; i < ac; i++)
	{
		if (av[i][0] == '-')
		{
			if      (av[i][1] == 'v') config.v = 1;
			else if (av[i][1] == 'q') config.q = 1;
			else if (av[i][1] == 'c') config.c = parse_validated_int(av[i], av, &i, ac, 1,  NO_MAX);
			else if (av[i][1] == 'i') config.i = parse_validated_int(av[i], av, &i, ac, 1,  NO_MAX);
			else if (av[i][1] == 's') config.s = parse_validated_int(av[i], av, &i, ac, 0,  SIZE_MAX);
			else if (av[i][1] == 't') config.t = parse_validated_int(av[i], av, &i, ac, 1,  TTL_MAX);
			else if (av[i][1] == '?')
			{
				printf("Usage: ft_ping [options] <destination>\n\n");
				printf("Options:\n");
				printf("  <destination>   IP address or domain name\n");
				printf("  -v              Verbose mode\n");
				printf("  -q              Quiet mode (stats only)\n");
				printf("  -c <count>      Stop after sending count packets\n");
				printf("  -i <interval>   Wait interval seconds between packets (default: 1)\n");
				printf("  -s <size>       Set payload size in bytes (default: 56)\n");
				printf("  -t <ttl>        Set TTL (1-255)\n");
				printf("  -?              Show this help\n\n");
				printf("Example:\n");
				printf("  ./ft_ping google.com\n");
				printf("  ./ft_ping -c 5 -i 2 8.8.8.8\n");
				exit(0);
			}
			else
			{
				printf("Error: unknown option '-%c'\n", av[i][1]);
				printf("Try 'ft_ping -?' for more information.\n");
				exit(1);
			}
		}
		else
		{
			if (config.target != NULL)
			{
				printf("Error: multiple targets\n");
				exit(1);
			}
			config.target = av[i];
		}
	}
	if (!config.target)
	{
		printf("Error: no target\n");
		exit(1);
	}
	return config;
}
