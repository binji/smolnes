#include "plugin.h"

void SuperMarioBrosSpecificHacks(uint16_t addr, uint8_t val, uint8_t write) {
    char str[40];
    if (addr == 0x000E && val == 0x06){
        //sprintf(str, "player state read/write at %04X val=%02X\n", addr, val);
        //OutputDebugString(str);
        memcpy(smbtmr, smbltmr, sizeof(smbtmr));
        dead = 0;
    } else if (addr == 0x000E && val == 0x0B) {
        //sprintf(str, "player state read/write at %04X val=%02X\n", addr, val);
        //OutputDebugString(str);
        dead = 0;
    } else if (addr == 0x000E && val == 0x0A) {
        //sprintf(str, "player state read/write at %04X val=%02X\n", addr, val);
        //OutputDebugString(str);
        dead = 0;
    } else if (addr == 0x00B5 && val == 0x02) {
        //sprintf(str, "player vertical position high byte write at %04X val=%02X\n", addr, val);
        //OutputDebugString(str);
        dead = 0;
    } else if (addr == 0x000E && val == 0x08) {
        //sprintf(str, "player state read/write at %04X val=%02X\n", addr, val);
        //OutputDebugString(str);
        dead = 1;
    }

    // 07F8, hundreds, 07F9, tens, 07FA, ones
    if ( (addr >= 0x07F8 && addr <= 0x07FA) && write ) {
        smbtmr[addr - 0x07F8] = val;
        //sprintf(str, "Timer digit read/write at %04X val=%02X\n", addr, val);
        //OutputDebugString(str);
    }

    // $0770: level state (00 = title, 01 = in game, 02 = victory, 03 = game over)
    if (addr == 0x0770 && write) {
        if (val == 0x01) {
            in_game = true;
        } else {
            in_game = false;
            length_determiner = false;  // reset when leaving game
            smbtmr[0] = 0; smbtmr[1] = 0; smbtmr[2] = 0;
            smbltmr[0] = 0; smbltmr[1] = 0; smbltmr[2] = 0;
        }
    }

    // $0766/$075F: world number
    if ((addr == 0x0766 && write) || (addr == 0x075F && write)) {
      //sprintf(str, "world at %04X val=%02X\n", addr, val);
        if (val != last_world) {
            last_world = val;
            length_determiner = false;  // new world -> allow recapture
        }
    }

    // $0767/$0760: level number
    if ((addr == 0x0767 && write) || (addr == 0x0760 && write)) {
      //sprintf(str, "level at %04X val=%02X\n", addr, val);
        if (val != last_level) {
            last_level = val;
            length_determiner = false;
        }
    }

    // $000E: player state
    if (addr == 0x000E && write) {
        if (in_game && !length_determiner) {
            if (val == 0x08 || val == 0x07) {
                // copy stored time into smbltmr once
                for (int i = 0; i < 3; i++) {
                    smbltmr[i] = smbtmr[i];
                }
                length_determiner = true;
            }
        }
    }
}