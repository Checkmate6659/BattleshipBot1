CC=clang

all:
	$(CC) *.c -o battleship -O3 -Ofast -flto
	# $(CC) *.c -o battleship -fsanitize=address
