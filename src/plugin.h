#ifndef SMOLNESPLUGIN
#define SMOLNESPLUGIN
#include <stdint.h>
#include <string.h>

extern uint8_t smbtmr[3],
    smbltmr[3],
    last_world,
    last_level;

extern int dead;
extern bool in_game;
extern bool length_determiner;

extern void SuperMarioBrosSpecificHacks(uint16_t addr, uint8_t val, uint8_t write);

#endif