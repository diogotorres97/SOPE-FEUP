all: gerador sauna


gerador: gerador.c
	gcc -o gerador gerador.c -pthread

sauna: sauna.c
	gcc -o sauna sauna.c -pthread

clean:
	rm gerador sauna