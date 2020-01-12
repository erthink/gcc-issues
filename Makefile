CFLAGS ?= -g -Ofast -Wall -Wextra -Wpedantic -Wno-unused-function

all: heapsort heapsort-pgo
	./heapsort && ./heapsort-pgo

# LY: prefer seperated compile/link steps for better output of `objdump -S heapsort.o`
heapsort: heapsort.o Makefile
	$(CC) $(CFLAGS) -ffunction-sections heapsort.o -o $@
heapsort.o: heapsort.c Makefile
	$(CC) $(CFLAGS) -c -DBENCHMARK heapsort.c -o $@

# PGO
heapsort-pgo.c: heapsort.c Makefile
	$(CC) -E -P -DBENCHMARK heapsort.c >$@
heapsort4pgo: heapsort-pgo.c Makefile
	$(CC) $(CFLAGS) -fprofile-generate -DBENCHMARK heapsort-pgo.c -o $@
heapsort-pgo.gcda: heapsort4pgo
	echo -n "Runinng PGO..." && ./heapsort4pgo >/dev/null && echo "done"
heapsort-pgo: heapsort-pgo.c Makefile heapsort-pgo.gcda
	$(CC) $(CFLAGS) -fprofile-use -DBENCHMARK heapsort-pgo.c -o $@

clean:
	rm -rf *.o heapsort-pgo.c *.S heapsort heapsort-pgo heapsort4pgo heapsort-pgo.gcda core
