TARGET = bench
CC = gcc
CFLAGS = -O3 -g

default: $(TARGET)
all: default

OBJS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o:%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<-o$@

.PRECIOUS: $(TARGET) $(OBJECTS)

test1: 
	./$(TARGET) 1000000 8 0 "test1_data.txt" "test1_rep_djb2.txt"
	./$(TARGET) 1000000 8 1 "test1_data.txt" "test1_rep_rand.txt"
	./$(TARGET) 1000000 8 2 "test1_data.txt" "test1_rep_xorshift.txt"

test2:
	./$(TARGET) 10000000 8 0 "test2_data.txt" "test2_rep_djb2.txt"
	./$(TARGET) 10000000 8 1 "test2_data.txt" "test2_rep_rand.txt"
	./$(TARGET) 10000000 8 2 "test2_data.txt" "test2_rep_xorshift.txt"

plot:
	echo 3 | sudo tee /proc/sys/vm/drop_caches;
	sudo perf stat --repeat 10 \
		-e cache-misses,cache-references,instructions,cycles \
		./$(TARGET) 1000000 8 0 "test1_data.txt" "test1_rep_djb2.txt"
	sudo perf stat --repeat 10 \
		-e cache-misses,cache-references,instructions,cycles \
		./$(TARGET) 1000000 8 1 "test1_data.txt" "test1_rep_rand.txt"
	sudo perf stat --repeat 10 \
		-e cache-misses,cache-references,instructions,cycles \
		./$(TARGET) 1000000 8 2 "test1_data.txt" "test1_rep_xorshift.txt"

testall: test1 test2

clean:
	$(RM) *.o
	$(RM) $(TARGET)
	$(RM) *.txt
