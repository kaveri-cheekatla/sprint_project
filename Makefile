BIN=bin/
SRC=source/
OBJ=obj/
CFLAG= -o
OFLAG= -c
CCMD= gcc
server:
	${CCMD} ${CFLAG} ${BIN}server  ${SRC}ftpserver.c
client:
	${CCMD} ${CFLAG} ${BIN}client  ${SRC}ftpclient.c

clean:
	rm ${BIN}client
	rm ${BIN}server
