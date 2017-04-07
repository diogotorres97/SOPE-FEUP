all: sfind

sfind: main.c search.h
	cc main.c -o sfind

clean:
	rm -f sfind
