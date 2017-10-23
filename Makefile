all:
	make clean
	gcc -I./include src/proxyserver.c

clean:
	rm -f *.out
