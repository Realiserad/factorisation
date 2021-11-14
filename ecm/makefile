FLAGS = -std=c99 -Wall -g
LIBS = -lpthread
CC = clang

OBJ = ec.o main.o prime.o ecm.o queue.o ecm_params.o mini-gmp.o pollard.o

%.o : %.c %.h 
	$(CC) $(FLAGS) -c $<

main: $(OBJ)
	$(CC) $(LIBS) $(FLAGS) -o main $(OBJ)

clean:
	rm -f main
	rm -f $(OBJ)
