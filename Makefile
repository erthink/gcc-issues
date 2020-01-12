all: heapsort

heapsort.o: heapsort.c Makefile
	$(CC) -g -Ofast -Wpedantic -ffunction-sections -c -DBENCHMARK $< -o $@

heapsort: heapsort.o Makefile
	$(CC) $< -o $@

clean:
	rm -rf *.o *.S heapsort core
