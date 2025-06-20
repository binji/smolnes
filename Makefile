.PHONY: all clean
all: smolnes deobfuscated

WARN=-std=c99 \
		 -Wall \
     -Wno-parentheses \
		 -Wno-misleading-indentation \
		 -Wno-bool-operation \
		 -Wno-discarded-qualifiers \
		 -Wno-incompatible-pointer-types-discards-qualifiers \
		 -Wno-unknown-warning-option \
		 -Wno-switch-outside-range \
		 -Wno-unused-value \
		 -Wno-char-subscripts \
		 -Wno-switch

smolnes: smolnes.c
	$(CC) -O2 -o $@ $< -lSDL2 -g ${WARN}

deobfuscated: deobfuscated.c
	$(CC) -O2 -o $@ $< -lSDL2 -g ${WARN}

clean:
	rm -f smolnes deobfuscated
