OBJS=updater.o

updater: ${OBJS}
	gcc -o $@ ${OBJS} C:\MinGW\lib\libhul.a -lwininet

%.o: %.c
	gcc ${COPT} -c $<
