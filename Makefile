NAME = monitor
MAIN = main.c
#SRCS = utils.c err.c init.c act.c 

OBJS = ${SRCS:.c=.o}

CC = gcc
CFLAGS = -Wall -Wextra -Werror
PTHREAD = -lpthread

RM = rm -f

all : ${NAME}


$(NAME) : ${OBJS}
	${CC} ${PTHREAD} ${MAIN} ${SRCS} -o ${NAME}
	${RM} ${OBJS}
	@echo "\033[33m[philosophers                                  by juhaprk]\033[0m"

.c.o :
	${CC} ${CFLAGS} -c $< -o ${<:.c=.o}

clean :
	${RM} ${OBJS}

fclean : clean
	${RM} ${NAME}

re : fclean all
