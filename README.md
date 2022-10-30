# smolnes

A NES emulator in ~5000 significant bytes of c++.

## Features

Plays some mapper 0 and mapper 1 games.

## Screenshots

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

![Source Code](img/smolnes.png)

## Building

Probably only builds on Linux and macOS. Needs a compiler with the
[case range extension](https://gcc.gnu.org/onlinedocs/gcc/Case-Ranges.html), so gcc or clang probably.

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

Look for lines 23 and 24 in the source code. The following table shows which
numbers map to which keyboard keys:

| number | default key | gameboy button |
| - | - | - |
| 27 | X | A Button |
| 29 | Z | B Button |
| 43 | Tab | Select Button |
| 40 | Return | Start Button |
| 79 | Arrow Right | DPAD Right |
| 80 | Arrow Left | DPAD Left |
| 81 | Arrow Down | DPAD Down |
| 82 | Arrow Up | DPAD Up |

Replace the numbers on this line with one from the [SDL scancode list](https://www.libsdl.org/tmp/SDL/include/SDL_scancode.h).

