#as    23123213  213213123  78897 789 1221  8901812 123
NAME = monitor
MAIN = main.c
SRCS = utils.c index_utils.c
MEM = -fsanitize=address

OBJS = ${SRCS:.c=.o}

CC = gcc
#CFLAGS = -Wall -Wextra -Werror
PTHREAD = -lpthread

RM = rm -f

all : ${NAME}


$(NAME) : ${OBJS}
	${CC} ${PTHREAD} ${MEM} ${MAIN} ${SRCS} -o ${NAME}
	${RM} ${OBJS}
	@echo "\033[33m[monitor                                  by juhaprk]\033[0m"

.c.o :
	${CC} ${CFLAGS} -c $< -o ${<:.c=.o}

clean :
	${RM} ${OBJS}

fclean : clean
	${RM} ${NAME}

re : fclean all
