smolnes: smolnes.cc
	$(CC) -O2 -Wall -o $@ $< -lSDL2 -g -Wno-parentheses -Wno-misleading-indentation -Wno-bool-operation

clean:
	rm -f smolnes
