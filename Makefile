CFLAGS = -Wall -g

EXE = allocate
SRC = main.c linkedlists.c utility.c
OBJ = $(SRC:%.c=%.o)


$(EXE): $(OBJ)
	cc $(CFLAGS) -o $(EXE) $^ -lm

clean:
	rm -f $(OBJ) $(EXE) output.txt

format:
	clang-format -style=file -i *.c 
