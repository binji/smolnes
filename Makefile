.PHONY: all clean
all: smolnes deobfuscated

WARN=-Wall \
     -Wno-parentheses \
		 -Wno-misleading-indentation \
		 -Wno-bool-operation \
		 -Wno-discarded-qualifiers \
		 -Wno-incompatible-pointer-types-discards-qualifiers \
		 -Wno-c2x-extensions \
		 -Wno-unknown-warning-option

smolnes: smolnes.c
	$(CC) -O2 -o $@ $< -lSDL2 -g ${WARN}

deobfuscated: deobfuscated.c
	$(CC) -O2 -o $@ $< -lSDL2 -g ${WARN}

clean:
	rm -f smolnes deobfuscated
