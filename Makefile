EXE=allocate

$(EXE): allocate.c
	cc -Wall -o $(EXE) $< memory.c process_q.c frame.c -lm

format:
	clang-format -style=file -i *.c

clean:
	rm $(EXE) -f