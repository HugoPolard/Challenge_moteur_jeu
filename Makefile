all: standalone
	cd ./standalone
	gcc -c main.c
	gcc main.o libavalam.a -o standalone.exe

all: diag
	cd ./diag
	gcc -c main.c
	gcc main.o libavalam.a -o standalone.exe

clean:
	rm *.o *.exe
