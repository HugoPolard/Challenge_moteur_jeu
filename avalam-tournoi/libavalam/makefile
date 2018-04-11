all: install
lib: libavalam.so libavalam.a

install: libavalam.so libavalam.a
	cp libavalam.so libavalam.a ../tournoi/lib

libavalam.so : libavalam.c
	gcc -shared -fPIC libavalam.c -o libavalam.so

libavalam.a : libavalam.c
	gcc -c  libavalam.c -o libavalam.o
	ar qvs libavalam.a  libavalam.o

clean: 
	rm -rf  libavalam.so libavalam.a libavalam.o
	rm -rf  ../tournoi/lib/libavalam.so ../tournoi/lib/libavalam.a

