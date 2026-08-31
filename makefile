NAME = codexion

CC = cc

CFLAGS = -Wall -Wextra -Werror -pthread

SRCS = src/build_c_routin.c \
	   src/build_m_routine.c \
	   src/heap_help.c \
	   src/heap_op.c \
	   src/helper_c_build2.c \
	   src/helper2.c \
	   src/helper_c_build.c \
	   src/helpers_parsing.c \
	   src/helper_time.c \
	   src/init.c \
	   src/main.c \
	   src/parsing.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS) src/codexion.h
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re