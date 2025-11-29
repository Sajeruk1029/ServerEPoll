CC	=	gcc

CFLAGS	=	-Wall -Wpedantic -Wextra

INCLUDEPATH	=	-I include/

SRCS	=	src/main.c \
				src/Message.c \
				src/log.c

OBJS	=	$(SRCS:.c=.o)

APP	=	Server

all	:	$(APP)

$(APP)	:	$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDEPATH) -g

%.o	:	%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDEPATH) -g

clean	:	
	rm -rf $(APP) $(OBJS)

rebuild	:	clean all
