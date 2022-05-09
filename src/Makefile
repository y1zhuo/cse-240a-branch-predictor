CC=gcc
OPTS=-g -std=c99 -Werror

all: main.o predictor.o predictor.o ptable.o
	$(CC) $(OPTS) -lm -o predictor main.o predictor.o tournament.o ptable.o

main.o: main.c predictor.h
	$(CC) $(OPTS) -c main.c

predictor.o: predictor.h predictor.c
	$(CC) $(OPTS) -c predictor.c Tournament.c

tournament.o: tournament.c tournament.h
	$(CC) $(OPTS) -c tournament.c

ptable.o: ptable.c ptable.h
	$(CC) $(OPTS) -c ptable.c

clean:
	rm -f *.o predictor;
