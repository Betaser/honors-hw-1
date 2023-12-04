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

testall: test1 test2 test3

test1: main
	./main inputs/input1 > output1
	diff output1 expected/expected1
	
test2: main
	./main inputs/input2 > output2
	diff output2 expected/expected2

test3: main
	./main inputs/input3 > output3
	diff output3 expected/expected3

shouldbetest1: main
	./main inputs/input1 | sort | tee output1
	diff output1 expected/expected1 | sort


mem1: test1
	$(VAL) ./main inputs/input1 | tee
testmemory: test1 test2 test3
	$(VAL) ./main inputs/input1 | tee
	$(VAL) ./main inputs/input2 | tee
	$(VAL) ./main inputs/input3 | tee

clean:
	rm -f main *.o output? *~
