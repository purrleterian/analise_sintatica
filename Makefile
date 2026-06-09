build:
	gcc -o prog src/main.c src/lexer.c src/utils.c src/symbols.c src/casa_token.c

run: build
	./prog res/prog_ok.txt
