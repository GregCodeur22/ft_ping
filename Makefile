NAME := ft_ping

CC := gcc
CFLAGS := -Wall -Wextra -Werror -Iincludes

SRC_DIR := src
SRC := $(SRC_DIR)/main.c \
		$(SRC_DIR)/parsing.c \
		$(SRC_DIR)/parsing_target.c \
		$(SRC_DIR)/build_paquet.c \
		$(SRC_DIR)/send_paquet.c \
		$(SRC_DIR)/recv_paquet.c


OBJ := $(SRC:.c=.o)

RM := rm -f

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all