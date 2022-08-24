NAME = monitor
MAIN = agent/main.c
SRCS = agent/utils.c agent/index_utils.c agent/parse.c
#MEM = -fsanitize=address

OBJS = ${SRCS:.c=.o}

CC = gcc
#CFLAGS = -Wall -Wextra -Werror
PTHREAD = -lpthread

RM = rm -f

all : ${NAME}


$(NAME) : ${OBJS}
	${CC} ${PTHREAD} ${MAIN} ${SRCS} -o ${NAME}
	${RM} ${OBJS}
	@echo "\033[33m[monitor                                  by juhaprk]\033[0m"
	@echo "\033[33m[    input \033[32m./monitor\033[33m to start this application      ]\033[0m"

.c.o :
	${CC} ${CFLAGS} -c $< -o ${<:.c=.o}

clean :
	${RM} ${OBJS}

fclean : clean
	${RM} ${NAME}

re : fclean all
