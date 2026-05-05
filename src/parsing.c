#include "../includes/ft_ping.h"

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

t_config parse_args(int ac, char **av)
{
	t_config config;

	config.target = NULL;
	config.n = 0;
	config.w = 0;

	for (int i = 1; i < ac; i++)
	{
		if (av[i][0] == '-')
		{
			if (av[i][1] == 'n')
				config.n = 1;

			else if (av[i][1] == 'w')
			{
				// -w5
				if (av[i][2] != '\0')
				{
					if (!is_number(&av[i][2]))
					{
						printf("Error: invalid -w value\n");
						exit(1);
					}
					config.w = atoi(&av[i][2]);
				}
				// -w 5
				else
				{
					if (i + 1 >= ac || !is_number(av[i + 1]))
					{
						printf("Error: -w requires number\n");
						exit(1);
					}
					config.w = atoi(av[i + 1]);
					i++;
				}
			}
			else
			{
				printf("Error: unknown option\n");
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