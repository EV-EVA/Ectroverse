#Check for colorgcc installation, use it if found.
ifeq ($(wildcard /usr/bin/colorgcc),) 
CC = gcc
else
CC = colorgcc
endif
DEFS = 
FLAGS = -s -mtune=core2 -O2 --fast-math -Wall  -fno-strict-aliasing
LIBS = -lm

sv: sv.o io.o db.o cmd.o
	$(CC) sv.o io.o db.o cmd.o $(DEFS) -o sv $(FLAGS) $(LIBS)

sv.o: sv.c svban.c config.h sv.h io.h db.h cmd.h artefact.h config.h
	$(CC) sv.c $(DEFS) -o sv.o -c $(FLAGS)
io.o: io.c config.h sv.h io.h db.h cmd.h artefact.h config.h iohttpvars.c iohttp.c iohttp2.c iohttp3.c iohttpmime.c ioevm.c
	$(CC) io.c $(DEFS) -o io.o -c $(FLAGS)
db.o: db.c sv.h io.h db.h cmd.h config.h
	$(CC) db.c $(DEFS) -o db.o -c $(FLAGS)
cmd.o: cmd.c cmdexec.c cmdtick.c battle.c specop.c sv.h io.h db.h cmd.h artefact.h config.h
	$(CC) cmd.c $(DEFS) -o cmd.o -c $(FLAGS)

clean:
	rm *.o -rf

map: map.c
	$(CC) map.c $(DEFS) -o map $(FLAGS) $(LIBS)
