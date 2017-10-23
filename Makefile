all:
	make clean
	g++ -I./include src/proxyserver.cpp -o bin/runserver

clean:
	rm -f bin/*
