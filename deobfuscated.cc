#include <SDL2/SDL.h>
#include <cstdint>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define PULL mem(++S, 1)
#define PUSH(x) mem(S--, 1, x, 1);

#define OP16(x)                                                                \
  break;                                                                       \
  case x:                                                                      \
  case x + 16:

// BGR565 palette. Used instead of RGBA32 to reduce source code size.
int rgba[64] = {25356, 34816, 39011, 30854, 24714, 4107,  106,   2311,  2468,
                2561,  4642,  6592,  20832, 0,     0,     0,     44373, 49761,
                55593, 51341, 43186, 18675, 434,   654,   4939,  5058,  3074,
                19362, 37667, 0,     0,     0,     ~0,    ~819,  64497, 64342,
                62331, 43932, 23612, 9465,  1429,  1550,  20075, 36358, 52713,
                16904, 0,     0,     ~0,    ~328,  ~422,  ~452,  ~482,  58911,
                50814, 42620, 40667, 40729, 48951, 53078, 61238, 44405},
    scany,     // Scanline Y
    shift_at,  // Attribute shift register
    p_mask[] = // Masks used in SE*/CL* instructions.
    {~1, 0, ~0, 1, ~4, 0, ~0, 4, ~0, 0, ~64, 0, ~8, 0, ~0, 8};

uint8_t *rom, *chrrom,                // Points to the start of PRG/CHR ROM
    *prg[2], *chr[2],                 // Points to current PRG/CHR banks
    A, X, Y, P = 4, S = ~2, PCH, PCL, // CPU Registers
    addr_lo, addr_hi,                 // Current instruction address
    nomem,     // 1 => current instruction doesn't write to memory
    result,    // Temp variable
    val,       // Current instruction value
    cross,     // 1 => page crossing occurred
    tmp, tmp2, // Temp variables
    ppumask, ppuctrl, ppustatus,     // PPU registers
    ppubuf,                          // PPU buffered reads
    W,                               // Write toggle PPU register
    fine_x,                          // X fine scroll offset, 0..7
    opcode,                          // Current instruction opcode
    nmi,                             // 1 => NMI occurred
    ntb,                             // Nametable byte
    ptb_lo, ptb_hi,                  // Pattern table low/high byte
    vram[2048],                      // Nametable RAM
    palette_ram[64],                 // Palette RAM
    ram[8192],                       // CPU RAM
    chrram[8192],                    // CHR RAM (only used for some games)
    prgram[8192],                    // PRG RAM (only used for some games)
    oam[256],                        // Object Attribute Memory (sprite RAM)
    br_mask[] = {128, 64, 1, 2},     // Masks used in branch instructions
    keys,                            // Joypad shift register
    mirror,                          // Current mirroring mode
    mmc1_bits, mmc1_data, mmc1_ctrl, // Mapper 1 (MMC1) registers
    chrbank0, chrbank1, prgbank,     // Current PRG/CHR bank
    rombuf[1 << 20],                 // Buffer to read ROM file into
    *key_state;

uint16_t T, V,           // "Loopy" PPU registers
    sum,                 // Sum used for ADC/SBC
    dot,                 // Horizontal position of PPU, from 0..340
    atb,                 // Attribute byte
    shift_hi, shift_lo,  // Pattern table shift registers
    cycles,              // Cycle count for current instruction
    frame_buffer[61440]; // 256x240 pixel frame buffer. Top and bottom 8 rows
                         // are not drawn.

// Add `val` to `addr_hi:addr_lo` and set `cross` to 1 if a page was crossed,
// or 0 otherwise.
uint8_t add(uint8_t val) {
  addr_hi += cross = addr_lo + val > 255;
  return addr_lo += val;
}

// Read a byte from CHR ROM or CHR RAM.
uint8_t &get_chr_byte(uint16_t a) { return chr[a >> 12][a & 4095]; }

// Read a byte from nametable RAM.
uint8_t &get_nametable_byte(uint16_t a) {
  return vram[!mirror       ? a & 1023                  // single bank 0
              : mirror == 1 ? a % 1024 + 1024           // single bank 1
              : mirror == 2 ? a & 2047                  // vertical mirroring
                            : a / 2 & 1024 | a & 1023]; // horizontal mirroring
}

// If `write` is non-zero, writes `val` to the address `hi:lo`, otherwise reads
// a value from the address `hi:lo`.
uint8_t mem(uint8_t lo, uint8_t hi = 0, uint8_t val = 0, uint8_t write = 0) {
  uint16_t a = hi << 8 | lo;

  switch (hi >> 4) {
  case 0 ... 1: // $0000...$1fff RAM
    return write ? ram[a] = val : ram[a];

  case 2 ... 3: // $2000..$2007 PPU (mirrored)
    lo &= 7;

    // read/write $2007
    if (lo == 7) {
      tmp = ppubuf;
      uint8_t &rom =
          // Access CHR ROM or CHR RAM
          V < 8192 ? !write || chrrom == chrram ? get_chr_byte(V) : tmp2
          // Access nametable RAM
          : V < 16128 ? get_nametable_byte(V)
                      // Access palette RAM
                      : palette_ram[uint8_t((V & 19) == 16 ? V ^ 16 : V)];
      write ? rom = val : ppubuf = rom;
      V += (ppuctrl & 4 ? 32 : 1) & 16383;
      return tmp;
    }

    if (write)
      switch (lo) {
      case 0: // $2000 ppuctrl
        ppuctrl = val;
        T = T & 62463 | val % 4 << 10;
        break;

      case 1: // $2001 ppumask
        ppumask = val;
        break;

      case 5: // $2005 ppuscroll
        T = (W ^= 1)      ? fine_x = val & 7,
        T & ~31 | val / 8 : T & 35871 | val % 8 << 12 | (val & 248) * 4;
        break;

      case 6: // $2006 ppuaddr
        T = (W ^= 1) ? T & 255 | val % 64 << 8 : V = T & ~255 | val;
      }

    if (lo == 2) // $2002 ppustatus
      return tmp = ppustatus & 224, ppustatus &= ~80, W = 0, tmp;

    break;

  case 4:
    if (write && lo == 20) // $4014 OAM DMA
      for (sum = 256; sum--;)
        oam[sum] = mem(sum, val);
    else if (lo == 22) // $4016 Joypad 1
      return write ? keys = key_state[SDL_SCANCODE_RIGHT] * 128 +
                            key_state[SDL_SCANCODE_LEFT] * 64 +
                            key_state[SDL_SCANCODE_DOWN] * 32 +
                            key_state[SDL_SCANCODE_UP] * 16 +
                            key_state[SDL_SCANCODE_RETURN] * 8 +
                            key_state[SDL_SCANCODE_TAB] * 4 +
                            key_state[SDL_SCANCODE_Z] * 2 +
                            key_state[SDL_SCANCODE_X]
                   : (tmp = keys & 1, keys /= 2, tmp);
    return 0;

  case 6 ... 7: // $6000...$7fff PRG RAM
    return write ? prgram[a & 8191] = val : prgram[a & 8191];

  case 8 ... 15: // $8000...$ffff ROM
    // handle mmc1 writes
    if (write)
      if (val & 128)
        mmc1_bits = 5, mmc1_data = 0, mmc1_ctrl |= 12;
      else if (mmc1_data = mmc1_data / 2 | val << 4 & 16, !--mmc1_bits)
        mmc1_bits = 5, tmp = a >> 13,
        (tmp == 4 ? mirror = mmc1_data & 3, mmc1_ctrl
     : tmp == 5   ? chrbank0
     : tmp == 6   ? chrbank1
                  : prgbank) = mmc1_data,

        // Update CHR pointers.
        chr[0] = chrrom + (chrbank0 & ~!(mmc1_ctrl & 16)) * 4096,
        chr[1] = chrrom + (mmc1_ctrl & 16 ? chrbank1 : chrbank0 | 1) * 4096,

        // Update PRG pointers.
        tmp = mmc1_ctrl / 4 & 3,
        prg[0] = rom + ((tmp == 2   ? 0
                         : tmp == 3 ? prgbank
                                    : prgbank & ~1)
                        << 14),
        prg[1] = rom + ((tmp == 2   ? prgbank
                         : tmp == 3 ? rom[-12] - 1
                                    : prgbank | 1)
                        << 14);
    return prg[(a >> 14) - 2][a & 16383];
  }

  return ~0;
}

// Read a byte at address `PCH:PCL` and increment PC.
uint8_t read_pc() {
  val = mem(PCL, PCH);
  !++PCL ? ++PCH : 0;
  return val;
}

// Set N (negative) and Z (zero) flags of `P` register, based on `val`.
uint8_t set_nz(uint8_t val) { return P = P & ~130 | val & 128 | !val * 2; }

int main(int, char **argv) {
  SDL_RWread(SDL_RWFromFile(argv[1], "rb"), rombuf, 1 << 20, 1);
  // Start PRG0 after 16-byte header.
  prg[0] = rom = rombuf + 16;
  // PRG1 is the last bank. `rombuf[4]` is the number of 16k PRG banks.
  prg[1] = rom + (rombuf[4] - 1 << 14);
  // CHR0 ROM is after all PRG data in the file. `rombuf[5]` is the number of
  // 8k CHR banks. If it is zero, assume the game uses CHR RAM.
  chr[0] = chrrom = rombuf[5] ? prg[1] + 16384 : chrram;
  // CHR1 is the last 4k bank.
  chr[1] = chrrom + rombuf[5] * 8192 - 4096;
  // Bit 0 of `rombuf[6]` is 0=>horizontal mirroring, 1=>vertical mirroring.
  mirror = !(rombuf[6] & 1) + 2;

  // Start at address in reset vector, at $FFFC.
  PCL = mem(~3, ~0);
  PCH = mem(~2, ~0);

  SDL_Init(SDL_INIT_VIDEO);
  // Create window 1024x840. The framebuffer is 256x240, but we don't draw the
  // top or bottom 8 rows. Scaling up by 4x gives 1024x960, but that looks
  // squished because the NES doesn't have square pixels. So shrink it by 7/8.
  auto *renderer = SDL_CreateRenderer(
      SDL_CreateWindow("smol  nes", 0, 0, 1024, 840, SDL_WINDOW_SHOWN), -1,
      SDL_RENDERER_PRESENTVSYNC);
  auto *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR565,
                                    SDL_TEXTUREACCESS_STREAMING, 256, 224);
  key_state = (uint8_t*)SDL_GetKeyboardState(0);

  for (;;) {
    cycles = nomem = 0;
    if (nmi)
      goto nmi;

    switch ((opcode = read_pc()) & 31) {
    case 0:
      if (opcode & 128) { // LDY/CPY/CPX imm
        read_pc();
        nomem = 1;
        goto nomemop;
      }

      switch (opcode >> 5) {
      case 0: // BRK or NMI
        !++PCL ? ++PCH : 0;
      nmi:
        PUSH(PCH)
        PUSH(PCL)
        PUSH(P | 32)
        // BRK vector is $ffff, NMI vector is $fffa
        PCL = mem(~1 - nmi * 4, ~0);
        PCH = mem(~0 - nmi * 4, ~0);
        cycles++;
        nmi = 0;
        break;

      case 1: // JSR
        result = read_pc();
        PUSH(PCH)
        PUSH(PCL)
        PCH = read_pc();
        PCL = result;
        break;

      case 2: // RTI
        P = PULL & ~32;
        PCL = PULL;
        PCH = PULL;
        break;

      case 3: // RTS
        PCL = PULL;
        PCH = PULL;
        !++PCL ? ++PCH : 0;
        break;
      }

      cycles += 4;
      break;

    case 16: // BPL, BMI, BVC, BVS, BCC, BCS, BNE, BEQ
      read_pc();
      if (!(P & br_mask[opcode >> 6 & 3]) ^ (opcode >> 5) & 1)
        (cross = PCL + (int8_t)val >> 8) ? PCH += cross, cycles++ : 0, cycles++,
            PCL += (int)val;

    OP16(8)
      switch (opcode >>= 4) {
      case 0: // PHP
        PUSH(P | 48)
        cycles++;
        break;

      case 2: // PLP
        P = PULL & ~16;
        cycles += 2;
        break;

      case 4: // PHA
        PUSH(A)
        cycles++;
        break;

      case 6: // PLA
        set_nz(A = PULL);
        cycles += 2;
        break;

      case 8: // DEY
        set_nz(--Y);
        break;

      case 9: // TYA
        set_nz(A = Y);
        break;

      case 10: // TAY
        set_nz(Y = A);
        break;

      case 12: // INY
        set_nz(++Y);
        break;

      case 14: // INX
        set_nz(++X);
        break;

      default: // CLC, SEC, CLI, SEI, CLV, CLD, SED
        P = P & p_mask[opcode - 1] | p_mask[opcode];
        break;
      }

    OP16(10)
      switch (opcode >> 4) {
      case 8: // TXA
        set_nz(A = X);
        break;

      case 9: // TXS
        S = X;
        break;

      case 10: // TAX
        set_nz(X = A);
        break;

      case 11: // TSX
        set_nz(X = S);
        break;

      case 12: // DEX
        set_nz(--X);
        break;

      case 14: // NOP
        break;

      default: // ASL/ROL/LSR/ROR A
        nomem = 1;
        val = A;
        goto nomemop;
      }
      break;

    case 1: // X-indexed, indirect
      read_pc();
      val += X;
      addr_lo = mem(val);
      addr_hi = mem(val + 1);
      cycles += 4;
      goto opcode;

    case 4 ... 6: // Zeropage
      addr_lo = read_pc();
      addr_hi = 0;
      cycles++;
      goto opcode;

    case 2: case 9: // Immediate
      read_pc();
      nomem = 1;
      goto nomemop;

    case 12 ... 14: // Absolute
      addr_lo = read_pc();
      addr_hi = read_pc();
      cycles += 2;
      goto opcode;

    case 17: // Zeropage, Y-indexed
      addr_lo = mem(read_pc());
      addr_hi = mem(val + 1);
      add(Y);
      cycles += 3 + (opcode == 145 || cross); // STA always uses extra cycle.
      goto opcode;

    case 20 ... 22: // Zeropage, X-indexed
      addr_lo = read_pc() + ((opcode & 214) == 150 ? Y : X); // LDX/STX use Y
      addr_hi = 0;
      cycles += 2;
      goto opcode;

    case 25: // Absolute, Y-indexed.
      addr_lo = read_pc();
      addr_hi = read_pc();
      add(Y);
      cycles += 2 + (opcode == 153 || cross); // STA always uses extra cycle.
      goto opcode;

    case 28 ... 30: // Absolute, X-indexed.
      addr_lo = read_pc();
      addr_hi = read_pc();
      add(opcode == 190 ? Y : X); // LDX uses Y
      cycles += 2 +
                // STA always uses extra cycle.
                (opcode == 157 ||
                 // ASL/ROL/LSR/ROR/INC/DEC all uses extra cycle.
                 opcode % 16 == 14 && opcode != 190 || cross);
      // fallthrough

    opcode:
      // Read from the given address into `val` for convenience below, except
      // for the STA/STX/STY instructions, and JMP.
      (opcode & 224) != 128 &&opcode != 76 ? val = mem(addr_lo, addr_hi) : 0;

    nomemop:
      switch (opcode & 243) {
      OP16(1) set_nz(A |= val);  // ORA
      OP16(33) set_nz(A &= val); // AND
      OP16(65) set_nz(A ^= val); // EOR

      OP16(225) // SBC
        val = ~val;
        goto a;

      OP16(97) // ADC
      a:
        sum = A + val + (P & 1);
        P = P & ~65 | sum > 255 | (~(A ^ val) & (val ^ sum) & 128) / 2;
        set_nz(A = sum);

      OP16(2) // ASL
        result = val * 2;
        P = P & ~1 | val >> 7;
        goto memop;

      OP16(34) // ROL
        result = val * 2 | P & 1;
        P = P & ~1 | val >> 7;
        goto memop;

      OP16(66) // LSR
        result = val / 2;
        P = P & ~1 | val & 1;
        goto memop;

      OP16(98) // ROR
        result = val / 2 | P << 7;
        P = P & ~1 | val & 1;
        goto memop;

      OP16(194) // DEC
        result = val - 1;
        goto memop;

      OP16(226) // INC
        result = val + 1;
        // fallthrough

      memop:
        set_nz(result);
        // Write result to A or back to memory.
        nomem ? A = result : (cycles += 2, mem(addr_lo, addr_hi, result, 1));
        break;

      case 32: // BIT
        P = P & 61 | val & 192 | !(A & val) * 2;
        break;

      case 64: // JMP
        PCL = addr_lo;
        PCH = addr_hi;
        cycles--;
        break;

      case 96: // JMP indirect
        PCL = val;
        PCH = mem(addr_lo + 1, addr_hi);
        cycles++;

      OP16(160) set_nz(Y = val); // LDY
      OP16(161) set_nz(A = val); // LDA
      OP16(162) set_nz(X = val); // LDX

      OP16(128) result = Y; goto store; // STY
      OP16(129) result = A; goto store; // STA
      OP16(130) result = X;             // STX

      store:
        mem(addr_lo, addr_hi, result, 1);

      OP16(192) result = Y; goto cmp; // CPY
      OP16(193) result = A; goto cmp; // CMP
      OP16(224) result = X;           // CPX
      cmp:
        P = P & ~1 | result >= val;
        set_nz(result - val);
        break;
      }
    }

    // Update PPU, which runs 3 times faster than CPU. Each CPU instruction
    // takes at least 2 cycles.
    for (tmp = cycles * 3 + 6; tmp--;) {
      if (ppumask & 24) { // If background or sprites are enabled.
        if (scany < 240) {
          if (dot < 256 || dot > 319) {
            switch (dot & 7) {
            case 1: // Read nametable byte.
              ntb = get_nametable_byte(V);
              break;
            case 3: // Read attribute byte.
              atb = (get_nametable_byte(
                         (960 | V & 3072 | V >> 4 & 56 | V / 4 & 7)) >>
                     (V >> 5 & 2 | V / 2 & 1) * 2) &
                    3;
              atb |= atb * 4;
              atb |= atb << 4;
              atb |= atb << 8;
              break;
            case 5: // Read pattern table low byte.
              ptb_lo = get_chr_byte(ppuctrl << 8 & 4096 | ntb << 4 | V >> 12);
              break;
            case 7: // Read pattern table high byte.
              ptb_hi =
                  get_chr_byte(ppuctrl << 8 & 4096 | ntb << 4 | V >> 12 | 8);
              // Increment horizontal VRAM read address.
              V = (V & 31) == 31 ? V & ~31 ^ 1024 : V + 1;
              break;
            }

            // Draw a pixel to the framebuffer.
            if ((uint16_t)scany < 240 && dot < 256) {
              // Read color and palette from shift registers.
              uint8_t color = shift_hi >> 14 - fine_x & 2 |
                              shift_lo >> 15 - fine_x & 1,
                      palette = shift_at >> 28 - fine_x * 2 & 12;

              // If sprites are enabled.
              if (ppumask & 16)
                // Loop through all sprites.
                for (uint8_t *sprite = oam; sprite < oam + 256; sprite += 4) {
                  uint16_t sprite_h = ppuctrl & 32 ? 16 : 8,
                           sprite_x = dot - sprite[3],
                           sprite_y = scany - *sprite - 1,
                           sx = sprite_x ^ (sprite[2] & 64 ? 0 : 7),
                           sy = sprite_y ^ (sprite[2] & 128 ? sprite_h - 1 : 0);
                  if (sprite_x < 8 && sprite_y < sprite_h) {
                    uint16_t sprite_tile = sprite[1],
                             sprite_addr = ppuctrl & 32
                                               // 8x16 sprites
                                               ? sprite_tile % 2 << 12 |
                                                     (sprite_tile & ~1) << 4 |
                                                     (sy & 8) * 2 | sy & 7
                                               // 8x8 sprites
                                               : (ppuctrl & 8) << 9 |
                                                     sprite_tile << 4 | sy & 7,
                             sprite_color =
                                 get_chr_byte(sprite_addr + 8) >> sx << 1 & 2 |
                                 get_chr_byte(sprite_addr) >> sx & 1;
                    // Only draw sprite if color is not 0 (transparent)
                    if (sprite_color) {
                      // Don't draw sprite if BG has priority.
                      !(sprite[2] & 32 && color)
                          ? color = sprite_color,
                            palette = 16 | sprite[2] * 4 & 12 : 0;
                      // Maybe set sprite0 hit flag.
                      sprite == oam &&color ? ppustatus |= 64 : 0;
                      break;
                    }
                  }
                }

              // Write pixel to framebuffer. Always use palette 0 for color 0.
              frame_buffer[scany * 256 + dot] =
                  rgba[palette_ram[color ? palette | color : 0]];
            }

            // Update shift registers every cycle.
            dot < 336 ? shift_hi *= 2, shift_lo *= 2, shift_at *= 4 : 0;

            // Reload shift registers every 8 cycles.
            dot % 8 == 7        ? shift_hi |= ptb_hi, shift_lo |= ptb_lo,
                shift_at |= atb : 0;
          }

          // Increment vertical VRAM address.
          dot == 256 ? V = ((V & 7 << 12) != 7 << 12 ? V + 4096
                            : (V & 992) == 928       ? V & 35871 ^ 2048
                            : (V & 992) == 992       ? V & 35871
                                               : V & 35871 | V + 32 & 992) &
                               // Reset horizontal VRAM address to T value.
                               ~1055 |
                           T & 1055
                     : 0;
        }

        // Reset vertical VRAM address to T value.
        scany == -1 &&dot > 279 &&dot < 305 ? V = V & 33823 | T & 31712 : 0;
      }

      if (scany == 241 && dot == 1) {
        // If NMI is enabled, trigger NMI.
        ppuctrl & 128 ? nmi = 1 : 0;
        ppustatus |= 128;
        // Render frame, skipping the top and bottom 8 pixels (they're often
        // garbage).
        SDL_UpdateTexture(texture, 0, frame_buffer + 2048, 512);
        SDL_RenderCopy(renderer, texture, 0, 0);
        SDL_RenderPresent(renderer);
        // Handle SDL events.
        for (SDL_Event event; SDL_PollEvent(&event);)
          if (event.type == SDL_QUIT)
            return 0;
      }

      // Clear ppustatus.
      scany == -1 &&dot == 1 ? ppustatus = 0 : 0;

      // Increment to next dot/scany. 341 dots per scanline, 262 scanlines per
      // frame. Scanline 261 is represented as -1.
      ++dot == 341 ? dot = 0, scany = scany == 260 ? -1 : scany + 1 : 0;
    }
  }
}
