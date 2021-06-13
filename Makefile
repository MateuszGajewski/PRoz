SOURCES=$(wildcard *.c)
HEADERS=$(SOURCES:.c=.h)
FLAGS=-DDEBUG -g

all: main

main: $(SOURCES) $(HEADERS)
	mpicc $(SOURCES) $(FLAGS) -o main

clean:
	rm main a.out

run: main
	mpirun --mca orte_base_help_aggregate 0 -np 7 ./main 3 3 1
