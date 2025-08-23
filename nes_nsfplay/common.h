#pragma once
#include "nes_apu.h"
#include <iostream>
// code taken from furnace's nsfplay

namespace xgm {
  const unsigned int DEFAULT_CLOCK = 1789773;
  const unsigned int DEFAULT_RATE  = 44100;

  extern NES_APU* nes1_NP;  // declaration only
  extern NES_DMC* nes2_NP;  // declaration only
}
