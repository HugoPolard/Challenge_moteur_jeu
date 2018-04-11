all: install 
lib: libjoueur.a libjoueur.so

install: libjoueur.a libjoueur.so
	cp  libjoueur.a libjoueur.so ../tournoi/lib

libjoueur.a : libjoueur.c
	gcc -c libjoueur.c -o libjoueur.o
	ar -qvs libjoueur.a libjoueur.o

libjoueur.so : libjoueur.c
	gcc -shared -fPIC libjoueur.c -o libjoueur.so 

clean: 
	rm -rf libjoueur.a libjoueur.so libjoueur.o
	rm -rf ../tournoi/lib/libjoueur.a ../tournoi/lib/libjoueur.so
