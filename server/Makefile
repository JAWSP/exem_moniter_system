NAME = server
MAIN = main.c queue.c parse.c
SRCS = ../utils/utils.c 
MEM = -fsanitize=address

OBJS = ${SRCS:.c=.o}

CC = gcc
PTHREAD = -lpthread

RM = rm -f

all : ${NAME}


$(NAME) : ${OBJS}
	${CC} ${PTHREAD} ${MEM} ${MAIN} ${SRCS} -o ${NAME}
	${RM} ${OBJS}
	@echo "\033[33m[server                                  by juhaprk]\033[0m"
	@echo "\033[33m[    input \033[32m./server\033[33m to start this application      ]\033[0m"

.c.o :
	${CC} ${CFLAGS} -c $< -o ${<:.c=.o}

clean :
	${RM} ${OBJS}

fclean : clean
	${RM} ${NAME}

re : fclean all
