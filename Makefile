FILENAME = parsing

build:
	@cc -std=c99 -Wall $(FILENAME).c mpc.c -ledit -lm -o $(FILENAME)
build-windows:
	@cc -std=c99 -Wall $(FILENAME).c mpc.c -o $(FILENAME)


run:build
	@./$(FILENAME)