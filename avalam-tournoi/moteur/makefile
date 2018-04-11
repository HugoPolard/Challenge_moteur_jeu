all: dep install 
exe: dep moteur moteur-static
dep: 
	cd ../libavalam; make

install: moteur moteur-static
	cp moteur moteur-static ../tournoi

LIB=-L../tournoi/lib

moteur:moteur.c 
	gcc $(LIB) -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast moteur.c -o moteur -lpthread -lavalam

moteur-static:moteur.c 
	gcc -static $(LIB) -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast moteur.c -o moteur-static -lpthread -lavalam

clean: 
	rm -rf moteur moteur-static
	rm -rf ../tournoi/moteur ../tournoi/moteur-static
	
cleanall: clean
	cd ../libavalam; make clean
