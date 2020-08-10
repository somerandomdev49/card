all:
	gcc card.c -o main -Wno-int-conversion

leak:
	gcc card.c -o main -Wno-int-conversion -fsanitize=leak -g
