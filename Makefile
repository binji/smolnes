.PHONY: all clean
all: smolnes deobfuscated

WARN=-Wno-parentheses -Wno-misleading-indentation -Wno-bool-operation -Wno-unused-value

smolnes: smolnes.cc
	$(CC) -O2 -Wall -o $@ $< -lSDL2 -g ${WARN}

deobfuscated: deobfuscated.cc
	$(CC) -O2 -Wall -o $@ $< -lSDL2 -g ${WARN}

clean:
	rm -f smolnes deobfuscated
