game: main.c rules.c
	gcc main.c rules.c -o life -I include -L lib -l SDL2-2.0.0

clean:
	rm life
