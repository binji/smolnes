.PHONY: all clean

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

LDFLAGS = -shared -lgdi32 -luser32

OBJS = minifb.o nes_apu.o nes_dmc.o common.o

all: in_nes.dll

in_nes.dll: src/plugin.c $(OBJS)
	i686-w64-mingw32-c++ -g3 -O3 $(CFLAGS) -o $@ $^ $(LDFLAGS) -static
	cp in_nes.dll "/home/erisl/.wine/drive_c/Program Files (x86)/Winamp5666/Plugins/in_nes.dll"

minifb.o: src/WinMiniFB.c
	i686-w64-mingw32-c++ -g3 -O3 $(CFLAGS) -c -o $@ $< -static

nes_apu.o: nes_nsfplay/nes_apu.cpp
	i686-w64-mingw32-c++ -g3 -O3 $(CFLAGS) -c -o $@ $< -static

nes_dmc.o: nes_nsfplay/nes_dmc.cpp
	i686-w64-mingw32-c++ -g3 -O3 $(CFLAGS) -c -o $@ $< -static

common.o: nes_nsfplay/common.cpp
	i686-w64-mingw32-c++ -g3 -O3 $(CFLAGS) -c -o $@ $< -static

clean:
	rm -f in_nes.dll $(OBJS)
