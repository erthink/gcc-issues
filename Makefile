CFLAGS ?= -g -Ofast -Wall -Wextra -Wpedantic -ffunction-sections
all: heapsort

# LY: prefer seperated compile/link steps for better output of `objdump -S heapsort.o`
heapsort: heapsort.o Makefile
	$(CC) $(CFLAGS) heapsort.o -o $@
heapsort.o: heapsort.c Makefile
	$(CC) $(CFLAGS) -c -DBENCHMARK heapsort.c -o $@

# PGO
heapsort-pgo.c: heapsort.c Makefile
	$(CC) -E -P -DBENCHMARK heapsort.c >$@

heapsort4pgo: heapsort-pgo.c Makefile
	$(CC) $(CFLAGS) -fprofile-generate -DBENCHMARK heapsort-pgo.c -o $@

heapsort-pgo.gcda: heapsort4pgo
	./heapsort4pgo

heapsort-pgo.o: heapsort-pgo.c Makefile heapsort-pgo.gcda
	$(CC) $(CFLAGS) -fprofile-use -c -DBENCHMARK heapsort-pgo.c -o $@
heapsort-pgo: heapsort-pgo.o Makefile
	$(CC) $(CFLAGS) heapsort-pgo.o -o $@

clean:
	rm -rf *.o *.S heapsort core
