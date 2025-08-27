#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include "../nes_nsfplay/common.h"
#include "MiniFB.h"
#include "../thirdparty/wacup/in2.h" // wacup input plugin api
#include "../thirdparty/wacup/wa_ipc.h" // wacup ipc api
#include "../nes_nsfplay/xtypes.h"
#include "plugin.h"

#define PLUGIN_VERSION "1.0.0"
char lastfn[MAX_PATH];

static char file_extensions[128] = { "NES\0iNES ROM (*.NES)\0" };

void about(HWND hwndParent);
void config(HWND hwndParent);
int init();
void quit();
void getfileinfo(const char* filename, char* title, int* length_in_ms);
int infoBox(const char* file, HWND hwndParent);
int isourfile(const char* fn);
int play(const char* fn);
void pause();
void unpause();
int ispaused();
void stop();
int getlength();
int getoutputtime();
void setoutputtime(int time_in_ms);
void setvolume(int volume);
void setpan(int pan);
void eq_set(int on, char data[10], int preamp);

void wa_main(int argc, char **argv);
void run_emulator_cycles(unsigned int target_cycles);

bool newrom = false;

int bitrate = 0;
int srate = 0;
int stereo = 0;
int synced = 0;

In_Module mod =		// the output module
{
  IN_VER_OLD,
  "smolnes plugin v"PLUGIN_VERSION,
  0,          // hMainWindow
  0,          // hDllInstance
  file_extensions,
  1,          // is_seekable
  1,          // uses output
  config,
  about,
  init,
  quit,
  getfileinfo,
  infoBox,
  isourfile,
  play,
  pause,
  unpause,
  ispaused,
  stop,
  getlength,
  getoutputtime,
  setoutputtime,
  setvolume,
  setpan,
  0,0,0,0,0,0,0,0,0,  // vis stuff
  0,0,                // dsp
  eq_set,
  NULL,               // setinfo
  0                   // out_mod
};

#define PULL mem(++S, 1, 0, 0)
#define PUSH(x) mem(S--, 1, x, 1)

static int Thread(void* arg);

void config(HWND hwndParent)
{
  MessageBoxA(hwndParent, "smolnes wrapped in a plugin.\nThere are no settings.", "Information", MB_OK);   // Must stay in ANSI
}
void about(HWND hwndParent)
{
  MessageBoxA(hwndParent, "smolnes -> winamp plugin", "About in_nes", MB_OK);   // Must stay in ANSI
}

uint8_t *rom, *chrrom,                // Points to the start of PRG/CHR ROM
    prg[4], chr[8],                   // Current PRG/CHR banks
    prgbits = 14, chrbits = 12,       // Number of bits per PRG/CHR bank
    A, X, Y, P = 4, S = ~2, PC_HI, PCL, // CPU Registers
    addr_lo, addr_hi,                 // Current instruction address
    nomem,  // 1 => current instruction doesn't write to memory
    result, // Temp variable
    val,    // Current instruction value
    cross,  // 1 => page crossing occurred
    tmp,    // Temp variables
    ppumask, ppuctrl, ppustatus, // PPU registers
    ppubuf,                      // PPU buffered reads
    W,                           // Write toggle PPU register
    fine_x,                      // X fine scroll offset, 0..7
    opcode,                      // Current instruction opcode
    nmi_irq,                     // 1 => IRQ occurred
                                 // 4 => NMI occurred
    ntb,                         // Nametable byte
    ptb_lo,                      // Pattern table lowbyte
    vram[2048],                  // Nametable RAM
    palette_ram[64],             // Palette RAM
    ram[8192],                   // CPU RAM
    chrram[8192],                // CHR RAM (only used for some games)
    prgram[8192],                // PRG RAM (only used for some games)
    oam[256],                    // Object Attribute Memory (sprite RAM)
    mask[] = {128, 64, 1, 2,     // Masks used in branch instructions
              1,   0,  0, 1, 4, 0, 0, 4, 0,
              0,   64, 0, 8, 0, 0, 8}, // Masks used in SE*/CL* instructions.
    keys,                              // Joypad shift register
    mirror,                            // Current mirroring mode
    mmc1_bits, mmc1_data, mmc1_ctrl,   // Mapper 1 (MMC1) registers
    mmc3_chrprg[8], mmc3_bits,         // Mapper 4 (MMC3) registers
    mmc3_irq, mmc3_latch,              //
    chrbank0, chrbank1, prgbank,       // Current PRG/CHR bank
    rombuf[1024 * 1024],               // Buffer to read ROM file into
    *key_state = {0},
    /*sqr1[4],
    sqr2[4],
    tri[3],
    noi[3],
    dmc[8],*/
    smbtmr[3],
    smbltmr[3],
    last_world,
    last_level;

uint16_t scany,          // Scanline Y
    T, V,                // "Loopy" PPU registers
    sum,                 // Sum used for ADC/SBC
    dot,                 // Horizontal position of PPU, from 0..340
    atb,                 // Attribute byte
    shift_hi, shift_lo,  // Pattern table shift registers
    cycles,              // Cycle count for current instruction
    frame_buffer[61440]; // 256x240 pixel frame buffer. Top and bottom 8 rows
                         // are not drawn.

int shift_at = 0;

HWND hwnd_winamp = FindWindow("Winamp v1.x",NULL);

int init()
{
  // i hate llama group and i am not gonna make it a secret
  if (SendMessage(hwnd_winamp,WM_WA_IPC,0,IPC_GETVERSION) > 0x5066){
    MessageBoxW(hwnd_winamp, L"llama group versions of winamp are not supported\ngo back to 5.666", L"FUCKO!!!!!", MB_OK);
    ExitProcess(0);
  }
  // yeah sure fuck you it works here i guess
  // and not when i start play()
  key_state = (uint8_t*) mfb_keystatus();

  xgm::nes1_NP=new xgm::NES_APU;
  xgm::nes1_NP->SetOption(xgm::NES_APU::OPT_NONLINEAR_MIXER,0);
  xgm::nes2_NP=new xgm::NES_DMC;
  xgm::nes2_NP->SetOption(xgm::NES_DMC::OPT_NONLINEAR_MIXER,0);
  xgm::nes2_NP->SetAPU(xgm::nes1_NP);
  /* for (int i = 0x4000; i < 0x4017; i++){
    xgm::nes1_NP->Write(i,0x00);
    xgm::nes2_NP->Write(i,0x00);
  } */

  xgm::nes1_NP->Reset();
  xgm::nes2_NP->Reset();

  /* xgm::nes1_NP->SetClock(xgm::DEFAULT_CLOCK);
  xgm::nes1_NP->SetRate(44100);
  xgm::nes2_NP->SetClock(xgm::DEFAULT_CLOCK);
  xgm::nes2_NP->SetRate(44100); */
  return 0;
}

void quit() {}

static HANDLE hThread;
bool wapaused = 0;
bool done = false;

void pause() { wapaused = 1; mod.outMod->Pause(1); }
void unpause() { wapaused = 0; mod.outMod->Pause(0); }
int ispaused() { return wapaused; }

int isourfile(const char* fn)
{
  //not sure what to do with this one yet
  //ensure we own WADs?
  return 0;
}

short sample_buffer[576*2];

// === audio loop ===
const int sample_rate = 44100;
double cycles_per_sample = xgm::DEFAULT_CLOCK / (double)sample_rate;

int play(const char* fn) {
  int maxlatency;
  strcpy(lastfn, fn);
  done = false;

  // create filename copy for the thread (thread frees it)
  char* filename_copy = _strdup(fn);

  wapaused = 0;

  maxlatency = mod.outMod->Open(44100,2,16, -1,-1);
	if(maxlatency < 0) return 1;

  char str[40];
  sprintf(str, "maxlatency (%d)\n", maxlatency);
  OutputDebugString(str);

  mod.SetInfo(bitrate, srate, stereo, synced);
  //mod.SAVSAInit(maxlatency, 44100);
  // where we're going, we don't need maxlatency
  // you cant see into the future in an NES emulator anyway
  mod.SAVSAInit(0, 44100);
  mod.VSASetInfo(44100, 2);
  mod.outMod->SetVolume(-666);
  mod.outMod->Flush(0);

  xgm::nes1_NP->Reset();
  xgm::nes2_NP->Reset();

  // create audio thread
  DWORD threadId;
  hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread, filename_copy, 0, &threadId);
  return 0;
}

double apu_clock_rest = 0.0;
int apu1[2] = {0,0};
int apu2[2] = {0,0};

static int Thread(void* arg) {
  char* filename = (char*)arg;
  int argc = 2;
  char** argv = (char**)malloc(argc * sizeof(char*));

  HWND hwnd_winamp = FindWindow("Winamp v1.x", NULL);
  char plugdir_buf[MAX_PATH] = {0};
  LRESULT result = SendMessageA(hwnd_winamp, WM_WA_IPC, 0, IPC_GETPLUGINDIRECTORY);
  // if the call returned a pointer, copy; otherwise fallback to empty string
  // (adjust if your environment returns differently)
  // NOTE: previously plugdir was NULL; ensure plugdir_buf contains a path
  // For safety, fallback to "." if no plugin dir found:
  if (!result) {
    strcpy(plugdir_buf, "."); // fallback
  } else {
    // If SendMessage returned char* in eax/stack - you may need a different retrieval.
    // Keep plugdir_buf as "." as safe default if unsure.
    strcpy(plugdir_buf, ".");
  }

  argv[0] = (char*)malloc(MAX_PATH);
  snprintf(argv[0], MAX_PATH, "%s\\in_nes.dll", plugdir_buf);
  argv[1] = strdup(filename);

  wa_main(argc, argv);

  free(argv[0]);
  free(argv);
  free(filename); // filename no longer needed after wa_main

  xgm::INT16 *packet_buf;
  int packet_size = 576 * 2 * 16 / 8; // �o�b�t�@�T�C�Y2048bytes�Œ�
  int blank_time = 10; // �ŏ��̐��p�P�b�g�͖����ɂ���(DirectX plugin�΍�)
  int wsize; // dsp������̏������݃T�C�Y
  int sample_size = 2 * 16 / 8;
  int packet_samples = packet_size / sample_size;

  packet_buf = new xgm::INT16[packet_size]; // dsp�̂��߂�2�{�̗̈���m��
  memset(packet_buf,0,packet_size*sizeof(xgm::INT16));

  const int sample_rate       = 44100;
  const int channels          = 2;
  const int frames_per_buffer = 576;
  const int frameSize         = channels * (int)sizeof(short);

  double cycles_accum = 0.0;
  double apu_clock_per_sample = (1789773 / sample_rate); 
  const double cpu_cycles_per_sample = (double)xgm::DEFAULT_CLOCK / (double)256/256.0;
  apu_clock_rest = 0.0;

  while (!done) {
    if(mod.outMod->CanWrite() >= (packet_size<<(mod.dsp_isactive()?1:0)))
    {
      // fill buffer with emulator audio
      short* s = packet_buf;
      for (int i = 0; i < packet_samples; i++) {
        apu_clock_rest += apu_clock_per_sample;
        int apu_clocks = (int)(apu_clock_rest);
          run_emulator_cycles(14);
          xgm::nes1_NP->Tick(apu_clock_per_sample);
          xgm::nes2_NP->TickFrameSequence(apu_clock_per_sample);
          xgm::nes2_NP->Tick(apu_clock_per_sample);
          apu_clock_rest += apu_clock_per_sample;
        if (apu_clocks > 0)
        {
          apu_clock_rest -= (double)(apu_clocks);
        }

        xgm::nes1_NP->Render(apu1);
        xgm::nes2_NP->Render(apu2);

        // scales the audio coming from NSFplay's APU and DMC code
        // also comes with very crude centering because of the DMC creating a DC offset
        // doesnt address the triangle channel going all wonky though
        const int SCALE = 8;
        int left  = (((apu1[0]/24) + apu2[0] - (xgm::nes2_NP->out[2] * (SCALE) * 5)) * SCALE) - 5800;
        //int left = ((xgm::nes2_NP->out[0] * (SCALE) * 5)) * SCALE;
        int right = left;

        if (left > 32767) left = 32767;
        if (left < -32768) left = -32768;
        if (right > 32767) right = 32767;
        if (right < -32768) right = -32768;

        *s++ = (short)left;
        *s++ = (short)right;
      }

      if(mod.dsp_isactive())
        wsize = mod.dsp_dosamples((short *)packet_buf,packet_samples,16,2,sample_rate) * (2*16/8);
      else
        wsize = packet_size;

      // timestamp for vis / SA / VSA
      int timestamp = mod.outMod->GetWrittenTime();
      /* char str[40];
      sprintf(str, "setoutputtime (%d)\n", timestamp);
      OutputDebugString(str); */
      mod.SAAddPCMData((char*)packet_buf, channels, 16, timestamp);
      mod.VSAAddPCMData((char*)packet_buf, channels, 16, timestamp);

      // write to output
      mod.outMod->Write((char*)packet_buf, wsize);
    }
    else Sleep(33);
  }

  mod.outMod->Close();
  mfb_close();
  return 0;
}

// Read a byte from CHR ROM or CHR RAM.
uint8_t *get_chr_byte(uint16_t a) {
  return &chrrom[chr[a >> chrbits] << chrbits | a % (1 << chrbits)];
}

// Read a byte from nametable RAM.
uint8_t *get_nametable_byte(uint16_t a) {
  return &vram[mirror == 0   ? a % 1024                  // single bank 0
               : mirror == 1 ? a % 1024 + 1024           // single bank 1
               : mirror == 2 ? a & 2047                  // vertical mirroring
                             : a / 2 & 1024 | a % 1024]; // horizontal mirroring
}

int dead = 0;
bool length_determiner = false;
bool in_game = false;

// If `write` is non-zero, writes `val` to the address `hi:lo`, otherwise reads
// a value from the address `hi:lo`.
uint8_t mem(uint8_t lo, uint8_t hi, uint8_t val, uint8_t write) {
  uint16_t addr = hi << 8 | lo;

  // my first 6502 assembly program :)
  // (just clears ram)
  if (newrom) {
    for (int i = 0x00; i < 0xFFFF; i++) {
      val = 0x00;
    }
    newrom = false;
  }

  // this all only works for super mario bros
  // any other game produces unexpected results
  SuperMarioBrosSpecificHacks(addr, val, write);

  mod.SetInfo(bitrate, srate, stereo, dead);

  xgm::nes1_NP->Write(addr,val);
  xgm::nes2_NP->Write(addr,val);

  // disable dpcm because its bugged right now
  if (addr == 0x4015) {
    uint8_t D = val;
    D &= ~(1 << 4);
    xgm::nes2_NP->Write(addr, D);
  }

  switch (hi >>= 4) {

  case 0: case 1: // $0000...$1fff RAM
    return write ? ram[addr] = val : ram[addr];

  case 2: case 3: // $2000..$2007 PPU (mirrored)
    lo &= 7;

    // read/write $2007
    if (lo == 7) {
      tmp = ppubuf;
      uint8_t *rom =
          // Access CHR ROM or CHR RAM
          V < 8192 ? write && chrrom != chrram ? &tmp : get_chr_byte(V)
          // Access nametable RAM
          : V < 16128 ? get_nametable_byte(V)
                      // Access palette RAM
                      : palette_ram + (uint8_t)((V & 19) == 16 ? V ^ 16 : V);
      write ? *rom = val : (ppubuf = *rom);
      V += ppuctrl & 4 ? 32 : 1;
      V %= 16384;
      return tmp;
    }

    if (write)
      switch (lo) {
      case 0: // $2000 ppuctrl
        ppuctrl = val;
        T = T & 0xf3ff | val % 4 << 10;
        break;

      case 1: // $2001 ppumask
        ppumask = val;
        break;

      case 5: // $2005 ppuscroll
        T = (W ^= 1)
          ? fine_x = val & 7, T & ~31 | val / 8
          : T & 0x8c1f | val % 8 << 12 | val * 4 & 0x3e0;
        break;

      case 6: // $2006 ppuaddr
        T = (W ^= 1)
          ? T & 0xff | val % 64 << 8
          : (V = T & ~0xff | val);
      }

    if (lo == 2) { // $2002 ppustatus
      tmp = ppustatus & 0xe0;
      ppustatus &= 0x7f;
      W = 0;
      return tmp;
    }
    break;

  case 4:
    /*if (write){
      if (lo < 0x04)
        sqr1[lo] = val;
      if (lo >= 0x04 && lo < 0x08)
        sqr2[lo - 0x04] = val;
      if (lo >= 0x08 && lo < 0x0C)
        tri[lo - 0x08] = val;
      if (lo >= 0x0C && lo < 0x10)
        noi[lo - 0x0C] = val;
      if (lo >= 0x10 && lo < 0x18)
        dmc[lo - 0x10] = val;
     //printf("dmc data: %d \n", dmc[1]);
    }*/
    if (write && lo == 20) // $4014 OAM DMA
      for (uint16_t i = 256; i--;)
        oam[i] = mem(i, val, 0, 0);
    // $4016 Joypad 1
    for (tmp = 0, hi = 8; hi--;)
    // losely based on niftsky's input mapping for his speedruns
      tmp = tmp * 2 + key_state[(uint8_t[]){
                          0x60,      // A
                          0x27,      // B
                          'G',    // Select
                          'J', // Start
                          'W',     // Dpad Up
                          'S',   // Dpad Down
                          'A',   // Dpad Left
                          'D',  // Dpad Right
                      }[hi]];
    if (lo == 22) {
      if (write) {
        keys = tmp;
      } else {
        tmp = keys & 1;
        keys /= 2;
        return tmp;
      }
    }
    return 0;

  case 6: case 7: // $6000...$7fff PRG RAM
    addr &= 8191;
    return write ? prgram[addr] = val : prgram[addr];

  default: // $8000...$ffff ROM
    // handle mapper writes
    if (write)
      switch (rombuf[6] >> 4) {
      case 7: // mapper 7
        mirror = !(val / 16);
        prg[0] = val % 8 * 2;
        prg[1] = prg[0] + 1;
        break;

      case 4: { // mapper 4
        uint8_t addr1 = addr & 1;
        switch (hi >> 1) {
          case 4: // Bank select/bank data
            *(addr1 ? &mmc3_chrprg[mmc3_bits & 7] : &mmc3_bits) = val;
            tmp = mmc3_bits >> 5 & 4;
            for (int i = 4; i--;) {
              chr[0 + i + tmp] = mmc3_chrprg[i / 2] & ~!(i % 2) | i % 2;
              chr[4 + i - tmp] = mmc3_chrprg[2 + i];
            }
            tmp = mmc3_bits >> 5 & 2;
            prg[0 + tmp] = mmc3_chrprg[6];
            prg[1] = mmc3_chrprg[7];
            prg[3] = rombuf[4] * 2 - 1;
            prg[2 - tmp] = prg[3] - 1;
            break;
          case 5: // Mirroring
            if (!addr1) {
              mirror = 2 + val % 2;
            }
            break;
          case 6:  // IRQ Latch
            if (!addr1) {
              mmc3_latch = val;
            }
            break;
          case 7:  // IRQ Enable
            mmc3_irq = addr1;
            break;
        }
        break;
      }

      case 3: // mapper 3
        chr[0] = val % 4 * 2;
        chr[1] = chr[0] + 1;
        break;

      case 2: // mapper 2
        prg[0] = val & 31;
        break;

      case 1: // mapper 1
        if (val & 0x80) {
          mmc1_bits = 5;
          mmc1_data = 0;
          mmc1_ctrl |= 12;
        } else if (mmc1_data = mmc1_data / 2 | val << 4 & 16, !--mmc1_bits) {
          mmc1_bits = 5;
          tmp = addr >> 13;
          *(tmp == 4 ? mirror = mmc1_data & 3, &mmc1_ctrl
          : tmp == 5 ? &chrbank0
          : tmp == 6 ? &chrbank1
                     : &prgbank) = mmc1_data;

          // Update CHR banks.
          chr[0] = chrbank0 & ~!(mmc1_ctrl & 16);
          chr[1] = mmc1_ctrl & 16 ? chrbank1 : chrbank0 | 1;

          // Update PRG banks.
          tmp = mmc1_ctrl / 4 % 4 - 2;
          prg[0] = !tmp ? 0 : tmp == 1 ? prgbank : prgbank & ~1;
          prg[1] = !tmp ? prgbank : tmp == 1 ? rombuf[4] - 1 : prgbank | 1;
        }
      }
    return rom[(prg[hi - 8 >> prgbits - 12] & (rombuf[4] << 14 - prgbits) - 1)
                   << prgbits |
               addr & (1 << prgbits) - 1];
  }

  return ~0;
}

// Read a byte at address `PC_HI:PCL` and increment PC.
uint8_t read_pc() {
  val = mem(PCL, PC_HI, 0, 0);
  !++PCL && ++PC_HI;
  return val;
}

// Set N (negative) and Z (zero) flags of `P` register, based on `val`.
uint8_t set_nz(uint8_t val) { return P = P & 125 | val & 128 | !val * 2; }

void readfile(const char * pathname, uint8_t * dst) {
  FILE *file = fopen(pathname, "rb");
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);

  fread(dst, sizeof(uint8_t), size, file);
}

// "wa_main" is a misleading name, but what am i supposed to name it
// it's just the main() function of smolnes
// wa_main implies the existence of a "winamp main" (lie)
void wa_main(int argc, char **argv) {
  // clear everything otherwise we load another rom into the same space
  // and shit fucking corrupts and or crashes the player
  newrom = true;
  if (newrom) {
    memset(ram, 0, sizeof(ram)), memset(vram, 0, sizeof(vram)),
        memset(palette_ram, 0, sizeof(palette_ram)), memset(prgram, 0, sizeof(prgram)),
        memset(oam, 0, sizeof(oam)), memset(chrram, 0, sizeof(chrram)),
        memset(smbtmr, 0, sizeof(smbtmr)); mmc3_bits = 0;
    memset(mmc3_chrprg, 0, sizeof(mmc3_chrprg));
    mmc3_irq = 0; mmc3_latch = 0; mmc1_bits = 0, mmc1_data = 0, mmc1_ctrl = 0;
    //memset(rom, 0, sizeof(rom)),memset(chrrom, 0, sizeof(chrrom));
    mirror = 0; prgbits = 14; chrbits = 12;
    prg[0] = 0; prg[1] = 1;
    chr[0] = 0; chr[1] = 1;
  }
  readfile(argv[1], rombuf);
  // Start PRG0 after 16-byte header.

  rom = rombuf + 16;
  // PRG1 is the last bank. `rombuf[4]` is the number of 16k PRG banks.
  prg[1] = rombuf[4] - 1;
  // CHR0 ROM is after all PRG data in the file. `rombuf[5]` is the number of
  // 8k CHR banks. If it is zero, assume the game uses CHR RAM.
  chrrom = rombuf[5] ? rom + ((prg[1] + 1) << 14) : chrram;
  // CHR1 is the last 4k bank.
  chr[1] = (rombuf[5] || 1) * 2 - 1;
  // Bit 0 of `rombuf[6]` is 0=>horizontal mirroring, 1=>vertical mirroring.
  mirror = 3 - rombuf[6] % 2;
  if (rombuf[6] / 16 == 4) {
    mem(0, 128, 0, 1); // Update to default mmc3 banks
    prgbits--;         // 8kb PRG banks
    chrbits -= 2;      // 1kb CHR banks
  }

  // Start at address in reset vector, at $FFFC.
  PCL = mem(~3, ~0, 0, 0);
  PC_HI = mem(~2, ~0, 0, 0);

  if (!mfb_open("smolnes", 256, 224, 3)) {}
    //return 0;
  // Create window 1024x840. The framebuffer is 256x240, but we don't draw the
  // top or bottom 8 rows. Scaling up by 4x gives 1024x960, but that looks
  // squished because the NES doesn't have square pixels. So shrink it by 7/8.
}

// poorly wrap the whole emulation thing into its own function
// may or may not work terribly for you
void run_emulator_cycles(unsigned int target_cycles)
{
  unsigned int consumed = 0;
  // locals used by many labels / gotos - declare early
  uint8_t opcodelo5;
  int cross = 0;
  int tmp_local = 0;
  int result_local = 0;

  // keep executing instructions until we've consumed the requested cycles
  while (consumed < target_cycles)
  {
    cycles = nomem = 0;
    if (nmi_irq)
      goto nmi_irq;

    opcode = read_pc();
    opcodelo5 = opcode & 31;
    switch (opcodelo5) {
      case 0:
        if (opcode & 0x80) { // LDY/CPY/CPX imm
          read_pc();
          nomem = 1;
          goto nomemop;
        }

        switch (opcode >> 5) {
          case 0: { // BRK or nmi_irq
            !++PCL && ++PC_HI;
            nmi_irq:
            PUSH(PC_HI);
            PUSH(PCL);
            PUSH(P | 32);
            // BRK/IRQ vector is $ffff, NMI vector is $fffa
            uint16_t veclo = ~1 - (nmi_irq & 4);
            PCL = mem(veclo, ~0, 0, 0);
            PC_HI = mem(veclo + 1, ~0, 0, 0);
            nmi_irq = 0;
            cycles++;
            break;
          }

          case 1: // JSR
            result = read_pc();
            PUSH(PC_HI);
            PUSH(PCL);
            PC_HI = read_pc();
            PCL = result;
            break;

          case 2: // RTI
            P = PULL & ~32;
            PCL = PULL;
            PC_HI = PULL;
            break;

          case 3: // RTS
            PCL = PULL;
            PC_HI = PULL;
            !++PCL && ++PC_HI;
            break;
        }

        cycles += 4;
        break;

          case 16: // BPL, BMI, BVC, BVS, BCC, BCS, BNE, BEQ
            read_pc();
            if (!(P & mask[opcode >> 6]) ^ opcode / 32 & 1) {
              cross = PCL + (int8_t)val >> 8;
              PC_HI += cross;
              PCL += val;
              cycles += cross ? 2 : 1;
            }
            break;

          case 8: case 24:
            switch (opcode >>= 4) {
              case 0: // PHP
                PUSH(P | 48);
                cycles++;
                break;

              case 2: // PLP
                P = PULL & ~16;
                cycles += 2;
                break;

              case 4: // PHA
                PUSH(A);
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
                P = P & ~mask[opcode + 3] | mask[opcode + 4];
                break;
            }
            break;

              case 10: case 26:
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
                    addr_lo = mem(val, 0, 0, 0);
                    addr_hi = mem(val + 1, 0, 0, 0);
                    cycles += 4;
                    goto opcode;

                  case 2: case 9: // Immediate
                    read_pc();
                    nomem = 1;
                    goto nomemop;

                  case 17: // Zeropage, Y-indexed
                    addr_lo = mem(read_pc(), 0, 0, 0);
                    addr_hi = mem(val + 1, 0, 0, 0);
                    cycles++;
                    goto add_x_or_y;

                  case 4: case 5: case 6:     // Zeropage               +1
                  case 20: case 21: case 22:  // Zeropage, X-indexed    +2
                    addr_lo = read_pc();
                    cross = opcodelo5 > 6;
                    if (cross) {
                      addr_lo += (opcode & 214) == 150 ? Y : X;  // LDX/STX use Y
                    }
                    addr_hi = 0;
                    cycles -= !cross;
                    goto opcode;

                  case 12: case 13: case 14: // Absolute               +2
                  case 25:                   // Absolute, Y-indexed    +2/3
                  case 28: case 29: case 30: // Absolute, X-indexed    +2/3
                    addr_lo = read_pc();
                    addr_hi = read_pc();
                    if (opcodelo5 < 25) goto opcode;
                    add_x_or_y:
                    val = opcodelo5 < 28 | opcode == 190 ? Y : X;
                    cross = addr_lo + val > 255;
                    addr_hi += cross;
                    addr_lo += val;
                    cycles +=
                    ((opcode & 224) == 128 | opcode % 16 == 14 & opcode != 190) | cross;
                    opcode:
                    cycles += 2;
                    if (opcode != 76 & (opcode & 224) != 128) {
                      val = mem(addr_lo, addr_hi, 0, 0);
                    }

                    nomemop:
                    result = 0;
                    switch (opcode & 227) {
                      case 1: set_nz(A |= val); break;  // ORA
                      case 33: set_nz(A &= val); break; // AND
                      case 65: set_nz(A ^= val); break; // EOR
                      case 225: // SBC
                        val = ~val;
                        // fallthrough
                      case 97: // ADC
                        sum = A + val + P % 2;
                        P = P & ~65 | sum > 255 | ((A ^ sum) & (val ^ sum) & 128) / 2;
                        set_nz(A = sum);
                        break;

                      case 34: // ROL
                        result = P & 1;
                        // fallthrough
                      case 2: // ASL
                        result |= val * 2;
                        P = P & ~1 | val / 128;
                        goto memop;

                      case 98: // ROR
                        result = P << 7;
                        // fallthrough
                      case 66: // LSR
                        result |= val / 2;
                        P = P & ~1 | val & 1;
                        goto memop;

                      case 194: // DEC
                        result = val - 1;
                        goto memop;

                      case 226: // INC
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
                        PC_HI = addr_hi;
                        cycles--;
                        break;

                      case 96: // JMP indirect
                        PCL = val;
                        PC_HI = mem(addr_lo + 1, addr_hi, 0, 0);
                        cycles++;
                        break;

                      default: {
                        uint8_t opcodehi3 = opcode / 32;
                        uint8_t *reg = opcode % 4 == 2 | opcodehi3 == 7 ? &X
                        : opcode % 4 == 1                ? &A
                        : &Y;
                        if (opcodehi3 == 4) {  // STY/STA/STX
                          mem(addr_lo, addr_hi, *reg, 1);
                        } else if (opcodehi3 != 5) {  // CPY/CMP/CPX
                          P = P & ~1 | *reg >= val;
                          set_nz(*reg - val);
                        } else {  // LDY/LDA/LDX
                          set_nz(*reg = val);
                        }
                        break;
                      }
                    }
    }

    // Update PPU, which runs 3 times faster than CPU. Each CPU instruction
    // takes at least 2 cycles.
    for (tmp = cycles * 3 + 6; tmp--;) {
      if (ppumask & 24) { // If background or sprites are enabled.
        if (scany < 240) {
          if (dot - 256 > 63u) {  // dot [0..255,320..340]
            // Draw a pixel to the framebuffer.
            if (dot < 256) {
              // Read color and palette from shift registers.
              uint8_t color = shift_hi >> 14 - fine_x & 2 |
              shift_lo >> 15 - fine_x & 1,
              palette = shift_at >> 28 - fine_x * 2 & 12;

              // If sprites are enabled.
              if (ppumask & 16) {
                // Loop through all sprites.
                for (uint8_t *sprite = oam; sprite < oam + 256; sprite += 4) {
                  uint16_t sprite_h = ppuctrl & 32 ? 16 : 8,
                  sprite_x = dot - sprite[3],
                  sprite_y = scany - sprite[0] - 1,
                  sx = sprite_x ^ !(sprite[2] & 64) * 7,
                  sy = sprite_y ^ (sprite[2] & 128 ? sprite_h - 1 : 0);
                  if (sprite_x < 8 && sprite_y < sprite_h) {
                    uint16_t sprite_tile = sprite[1],
                    sprite_addr =
                    (ppuctrl & 32
                    // 8x16 sprites
                    ? sprite_tile % 2 << 12 |
                    sprite_tile << 4 & -32 | sy * 2 & 16
                    // 8x8 sprites
                    : (ppuctrl & 8) << 9 | sprite_tile << 4) |
                    sy & 7,
                    sprite_color =
                    *get_chr_byte(sprite_addr + 8) >> sx << 1 & 2 |
                    *get_chr_byte(sprite_addr) >> sx & 1;
                    // Only draw sprite if color is not 0 (transparent)
                    if (sprite_color) {
                      // Don't draw sprite if BG has priority.
                      if (!(sprite[2] & 32 && color)) {
                        color = sprite_color;
                        palette = 16 | sprite[2] * 4 & 12;
                      }
                      // Maybe set sprite0 hit flag.
                      if (sprite == oam && color)
                        ppustatus |= 64;
                      break;
                    }
                  }
                }
              }

              // Write pixel to framebuffer. Always use palette 0 for color 0.
              // BGR565 palette is used instead of RGBA32 to reduce source code
              // size.
              frame_buffer[scany * 256 + dot] =
              (uint16_t[64]){
                25356, 34816, 39011, 30854, 24714, 4107,  106,   2311,
                2468,  2561,  4642,  6592,  20832, 0,     0,     0,
                44373, 49761, 55593, 51341, 43186, 18675, 434,   654,
                4939,  5058,  3074,  19362, 37667, 0,     0,     0,
                (uint16_t)~0,    (uint16_t)~819,  64497, 64342, 62331, 43932, 23612, 9465,
                1429,  1550,  20075, 36358, 52713, 16904, 0,     0,
                (uint16_t)~0,    (uint16_t)~328,  (uint16_t)~422,  (uint16_t)~452,  (uint16_t)~482,  58911, 50814, 42620,
                40667, 40729, 48951, 53078, 61238, 44405}
                [palette_ram[color ? palette | color : 0]];
            }

            // Update shift registers every cycle.
            if (dot < 336) {
              shift_hi *= 2;
              shift_lo *= 2;
              shift_at *= 4;
            }

            int temp = ppuctrl << 8 & 4096 | ntb << 4 | V >> 12;
            switch (dot & 7) {
              case 1: // Read nametable byte.
                ntb = *get_nametable_byte(V);
                break;
              case 3: // Read attribute byte.
                atb = (*get_nametable_byte(V & 0xc00 | 0x3c0 | V >> 4 & 0x38 |
                V / 4 & 7) >>
                (V >> 5 & 2 | V / 2 & 1) * 2) %
                4 * 0x5555;
                break;
              case 5: // Read pattern table low byte.
                ptb_lo = *get_chr_byte(temp);
                break;
              case 7: { // Read pattern table high byte.
                uint8_t ptb_hi = *get_chr_byte(temp | 8);
                // Increment horizontal VRAM read address.
                V = V % 32 == 31 ? V & ~31 ^ 1024 : V + 1;
                shift_hi |= ptb_hi;
                shift_lo |= ptb_lo;
                shift_at |= atb;
                break;
              }
            }
          }

          // Increment vertical VRAM address.
          if (dot == 256) {
            V = ((V & 7 << 12) != 7 << 12 ? V + 4096
            : (V & 0x3e0) == 928     ? V & 0x8c1f ^ 2048
            : (V & 0x3e0) == 0x3e0   ? V & 0x8c1f
            : V & 0x8c1f | V + 32 & 0x3e0) &
            // Reset horizontal VRAM address to T value.
            ~0x41f |
            T & 0x41f;
          }
        }

        // Check for MMC3 IRQ.
        if ((scany + 1) % 262 < 241 && dot == 261 && mmc3_irq && !mmc3_latch--)
          nmi_irq = 1;

        // Reset vertical VRAM address to T value.
        if (scany == 261 && dot - 280 < 25u)  // dot [280..304]
          V = V & 0x841f | T & 0x7be0;
      }

      if (dot == 1) {
        if (scany == 241) {
          // If NMI is enabled, trigger NMI.
          if (ppuctrl & 128)
            nmi_irq = 4;
          ppustatus |= 128;
          // Render frame, skipping the top and bottom 8 pixels (they're often
          // garbage).
          if (mfb_update(frame_buffer+ 2048, 60) == -1);
        }

        // Clear ppustatus.
        if (scany == 261)
          ppustatus = 0;
      }

      // Increment to next dot/scany. 341 dots per scanline, 262 scanlines per
      // frame. Scanline 261 is represented as -1.
      if (++dot == 341) {
        dot = 0;
        scany++;
        scany %= 262;
      }
    }

    // accumulate consumed CPU cycles for this instruction
    consumed += cycles;
  } // while(consumed < target_cycles)
} // run_emulator_cycles

void stop() {
  done = 1;
  WaitForSingleObject(hThread, 500);
  CloseHandle(hThread);
  hThread = INVALID_HANDLE_VALUE;
  mfb_close();
  mod.outMod->Close();
  mod.SAVSADeInit();
}

int getlength()
{
  int timer_val = smbltmr[0] * 100 + smbltmr[1] * 10 + smbltmr[2];
  return timer_val * 1000;
}

int getoutputtime()
{
  // rebuild decimal value from SMB timer digits ($07F8–$07FA)
  int timer_val = smbtmr[0] * 100 + smbtmr[1] * 10 + smbtmr[2];

  int elapsed_sec = (smbltmr[0] * 100 + smbltmr[1] * 10 + smbltmr[2]) - timer_val;

  // return ms
  return elapsed_sec * 1000;
}

void setoutputtime(int time_in_ms)
{
  // what am i meant to be doing here?
  #ifdef DEBUG_CONSOLE
  char str[40];
  sprintf(str, "setoutputtime (%d)\n", time_in_ms);
  debug_write(str);
  #endif
  //return (int)(((double)in_wad_realtics / 35.0f) * 1000.0f);
}

void setvolume(int volume)
{
  mod.outMod->SetVolume(volume);
}

void setpan(int pan)
{
  mod.outMod->SetPan(pan);
}

void getfileinfo(const char* filename, char* title, int* length_in_ms)
{
  //char str[40];
  //sprintf(str, "length_in_ms (%d)\n", length_in_ms);
  //OutputDebugString(str);
  //char str2[40];
  //sprintf(str2, "title (%d)\n", title);
  //OutputDebugString(str2);

  // change at some point
  strcpy(title, "smolnes");
}

void eq_set(int on, char data[10], int preamp)
{
  // what am i meant to be doing here?
}

int infoBox(const char* file, HWND hwndParent) {
  // what would i even need this for?
  return INFOBOX_UNCHANGED;
}

extern "C" __declspec(dllexport) In_Module* winampGetInModule2()
{
  return &mod;
}
