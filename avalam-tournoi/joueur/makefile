all: dep install
dep:
	cd ../libavalam; make
	cd ../libjoueur; make

LIB=-L../tournoi/lib

	
exe: dep joueur1 joueur1-static joueur2-static joueur3-static 

joueur1 : joueur1.c
	gcc $(LIB) joueur1.c -o joueur1 -ljoueur -lpthread -lavalam

joueur1-static : joueur1.c
	gcc -static $(LIB) joueur1.c -o joueur1-static -ljoueur -lpthread -lavalam

joueur2-static : joueur2.c
	gcc -static $(LIB) joueur2.c -o joueur2-static -ljoueur -lpthread -lavalam

joueur3-static : joueur3.c
	gcc -static $(LIB) joueur3.c -o joueur3-static -ljoueur -lpthread -lavalam


install: joueur1 joueur1-static joueur2-static joueur3-static
	cp joueur1 joueur1-static joueur2-static joueur3-static ../tournoi/

clean: 
	rm -rf joueur1 joueur1-static joueur2-static joueur3-static
	rm -rf ../tournoi/joueur1 ../tournoi/joueur1-static ../tournoi/joueur2-static ../tournoi/joueur3-static

cleanall: clean
	cd ../libavalam; make clean
	cd ../libjoueur; make clean
