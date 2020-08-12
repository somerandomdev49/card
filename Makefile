all:
	gcc card.c -o main -Wno-int-conversion $(CFLAGS)

leak:
	gcc card.c -o main -Wno-int-conversion -fsanitize=leak -g $(CFLAGS)
