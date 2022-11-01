smolnes: smolnes.cc
	$(CC) -O2 -Wall -o $@ $< -lSDL2 -g -Wno-parentheses -Wno-misleading-indentation -Wno-bool-operation -Wno-unused-value

clean:
	rm -f smolnes
