# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: zdaouari <zdaouari@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/08/26 14:31:14 by zdaouari          #+#    #+#              #
#    Updated: 2026/08/26 14:31:15 by zdaouari         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ./codexion

CC = cc
#CFLAGS = -Wall -Wextra -Werror -g

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re