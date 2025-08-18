# smolnes

A NES emulator in ~5000 significant bytes of c.

## Features

Plays some mapper 0/1/2/3/4/7 games.

## Screenshots

![Batman](img/batman.gif)
![Kirby's Adventure](img/kirby.gif)
![Mega Man 3](img/mm3.gif)
![Little Nemo the Dream Master](img/nemo.gif)
![River City Ransom](img/rcr.gif)
![Super Mario Bros. 2](img/smb2.gif)
![Super Mario Bros. 3](img/smb3.gif)
![Teenage Mutant Ninja Turtles 2](img/tmnt2.gif)
![Bionic Commando](img/bc.gif)
![Donkey Kong](img/dk.gif)
![Dragon Warrior](img/dw.gif)
![Adventures of Lolo](img/lolo.gif)
![Legend of Zelda](img/loz.gif)
![Metroid](img/met.gif)
![Mega Man 2](img/mm2.gif)
![Ninja Gaiden](img/ng.gif)
![Rad Racer](img/rr.gif)
![Super Mario Bros.](img/smb.gif)
![Teenage Mutant Ninja Turtle](img/tmnt.gif)
![Contra](img/contra.gif)
![Goonies 2](img/g2.gif)
![Castlevania](img/cv.gif)
![DuckTales](img/dt.gif)
![Alwa's Awakening](img/alwa.gif)
![Witch n' Wiz](img/wnw.gif)
![Battletoads](img/bt.gif)
![Wizard's and Warriors II](img/ww2.gif)
![Gradius](img/grad.gif)

![Source Code](img/smolnes.png)

## Building

Probably only builds on Linux and macOS. Try gcc or clang.

```
$ make
```

## Running

```
$ ./smolnes <rom.nes>
```

Keys:

| Action | Key |
| --- | --- |
| DPAD-UP | <kbd>↑</kbd> |
| DPAD-DOWN | <kbd>↓</kbd> |
| DPAD-LEFT | <kbd>←</kbd> |
| DPAD-RIGHT | <kbd>→</kbd> |
| B | <kbd>Z</kbd> |
| A | <kbd>X</kbd> |
| START | <kbd>Enter</kbd> |
| SELECT | <kbd>Tab</kbd> |

## Updating keys

Look for line 20 in the source code. The following table shows which
numbers map to which keyboard keys:

| character | number | default key | NES button |
| - | - | - | - |
| \33 | 27 | X | A Button |
| \35 | 29 | Z | B Button |
| + | 43 | Tab | Select Button |
| ( | 40 | Return | Start Button |
| R | 82 | Arrow Up | DPAD Up |
| Q | 81 | Arrow Down | DPAD Down |
| P | 80 | Arrow Left | DPAD Left |
| O | 79 | Arrow Right | DPAD Right |

1. Look up the keys you want to replace from this list [SDL scancode list](https://github.com/libsdl-org/SDL/blob/SDL2/include/SDL_scancode.h)
1. Convert the number to its ascii character, or C character literal.
1. Replace this character in the string.

## Projects using smolnes

* [p2r3/smolnes-nut](https://github.com/p2r3/smolnes-nut) The smolnes emulator
  ported to Squirrel for Portal 2 - https://youtu.be/PUTRvF7slYs
* [nevesnunes/ghidra-plays-mario](https://github.com/nevesnunes/ghidra-plays-mario)
  Playing NES ROMs with Ghidra's PCode Emulator
* [sysprog21/rv32emu](https://github.com/sysprog21/rv32emu) Compact and
  Efficient RISC-V RV32I[MAFC] emulator (uses smolnes as a demo)

Let me know if you've used smolnes and I'll add it to the list.
