CC      = gcc -g
CFLAGS  = -Wall -Werror -std=gnu17 -ggdb3 -Og
LDFLAGS =
PRG     = MyTraceroute

OBJFILE = IcmpHeader.o InputParser.o Main.o Repository.o SocketCreate.o SocketReceive.o SocketSend.o

${PRG}: ${OBJFILE}
	${CC} ${OBJFILE} ${LDFLAGS} -o $@

%.o: %.c *.h
	${CC} $<  -c ${CFLAGS} -o $@

.PHONY: clean  distclean

distclean:
	rm -f ${PRG}
	rm -f ${OBJFILE} 

clean:
	rm -f ${OBJFILE}