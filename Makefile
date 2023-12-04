WARNING = -Wall -Wshadow --pedantic
ERROR = -Wvla -Werror
GCC = gcc -std=c99 -g $(WARNING) $(ERROR)
VAL = valgrind --tool=memcheck --leak-check=full --verbose

SRCS = main.c
OBJS = $(SRCS:%.c=%.o)

main: $(OBJS)
	$(GCC) $(OBJS) -o main

.c.o:
	$(GCC) -c $*.c

testall: test1 test2 test3 test4 test5

run: main
	./main inputs/input1

test1: main
	./main inputs/input1 | sort | tee output1
	diff output1 expected/expected1 | sort


testmemory: test1 test2 test3 test4 test5
	$(VAL) ./main inputs/input1 | tee

clean:
	rm -f main *.o output? *~
