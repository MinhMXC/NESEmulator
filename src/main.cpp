// #define NDEBUG
#include "constants.h"
#include "./display/display.h"
#include "utils.h"
#include "ppu/ppu.h"
#include "cpu/cpu.h"
#include "initializer/initializer.h"
#include <SDL.h>
#include <SDL_image.h>
#include <cstdio>
#include <iostream>
#include <chrono>


int main(int argv, char** args) {
  // So that any line printed is displayed immediately
  setbuf(stdout, nullptr);

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    return -1;
  }

  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    printf("SDL_image could not initialize! SDL_image Error: %s\n", SDL_GetError());
  }

  // Ignore Scaling
  SDL_bool set = SDL_SetHintWithPriority(SDL_HINT_WINDOWS_DPI_AWARENESS, "system", SDL_HINT_OVERRIDE);
  if (set == SDL_FALSE) {
    printf("Hint not set");
  }

  // Making Window
  SDL_Window* window = SDL_CreateWindow(
    "NES Emulator",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    EmuConst::SCALED_SCREEN_WIDTH,
    EmuConst::SCALED_SCREEN_HEIGHT,
    SDL_WINDOW_SHOWN
  );
  if (window == nullptr) {
    printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
  }

  // Setting Icon
  SDL_Surface* nesIcon = IMG_Load("assets/nes_logo.png");
  if (nesIcon == nullptr) {
    printf("Cannot load program icon! SDL_image Error: %s\n", SDL_GetError());
  }
  SDL_SetWindowIcon(window, nesIcon);

  // Making Renderer
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    printf("Cannot create renderer! SDL Error: %s\n", SDL_GetError());
    return -1;
  }

  SDL_Texture* texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    EmuConst::SCREEN_WIDTH,
    EmuConst::SCREEN_HEIGHT
  );
  if (texture == nullptr) {
    printf("Cannot create texture! SDL Error: %s\n", SDL_GetError());
    return -1;
  }

  Display display{renderer, texture};
  PPU ppu{display};
  CPU cpu{ppu};
  Initializer initializer{cpu, ppu};
  std::string res{ initializer.loadFile("../test_rom/nestest.nes") };
  if (!res.empty()) {
    printf("Error: %s\n", res.c_str());
  }

  for (int i{}; i < 960; i++) {
    ppu.memory[0x2000 + i] = 5;
  }

  for (int i{}; i < 64; i++) {
    ppu.memory[0x23C0 + i] = 0b1010'1010;
  }

  ppu.writePPUCtrl(0b0000'0000);
  ppu.writePPUMask(0b0001'1110);
//  ppu.writePPUScroll(20);
//  ppu.writePPUScroll(100);

  ppu.memory[0x3F00] = 0;
  ppu.memory[0x3F08] = 15;
  ppu.memory[0x3F09] = 28;
  ppu.memory[0x3F0A] = 26;
  ppu.memory[0x3F0B] = 35;

  ppu.memory[0x0050] = 0b0000'0000;
  ppu.memory[0x0051] = 0b0111'1110;
  ppu.memory[0x0052] = 0b0101'1010;
  ppu.memory[0x0053] = 0b0111'1110;
  ppu.memory[0x0054] = 0b0101'1010;
  ppu.memory[0x0055] = 0b0100'0010;
  ppu.memory[0x0056] = 0b0111'1110;
  ppu.memory[0x0057] = 0b0000'0000;

  ppu.memory[0x0058] = 0b0000'0000;
  ppu.memory[0x0059] = 0b0000'0000;
  ppu.memory[0x005A] = 0b0010'0100;
  ppu.memory[0x005B] = 0b0001'1000;
  ppu.memory[0x005C] = 0b0010'0100;
  ppu.memory[0x005D] = 0b0011'1100;
  ppu.memory[0x005E] = 0b0000'0000;
  ppu.memory[0x005F] = 0b0000'0000;

//  ppu.memory[0x0050] = 0b1111'1111;
//  ppu.memory[0x0051] = 0b1111'1111;
//  ppu.memory[0x0052] = 0b1111'1111;
//  ppu.memory[0x0053] = 0b1111'1111;
//  ppu.memory[0x0054] = 0b1111'1111;
//  ppu.memory[0x0055] = 0b1111'1111;
//  ppu.memory[0x0056] = 0b1111'1111;
//  ppu.memory[0x0057] = 0b1111'1111;

  // 8x8 test
//  for (int i{}; i <= 16; i++) {
//    ppu.oam[i * 4] = 8;
//    ppu.oam[i * 4 + 1] = 0x6;
//    ppu.oam[i * 4 + 2] = 0b0000'0001;
//    ppu.oam[i * 4 + 3] = i * 4;
//  }
//
//  ppu.memory[0x3F15] = 48;
//  ppu.memory[0x3F16] = 38;
//  ppu.memory[0x3F17] = 39;
//
//  ppu.memory[0x0060] = 0b1111'1111;
//  ppu.memory[0x0061] = 0b1111'1111;
//  ppu.memory[0x0062] = 0b1111'1111;
//  ppu.memory[0x0063] = 0b1111'1111;
//  ppu.memory[0x0064] = 0b1111'1111;
//  ppu.memory[0x0065] = 0b1111'1111;
//  ppu.memory[0x0066] = 0b1111'1111;
//  ppu.memory[0x0067] = 0b1111'1111;
//
//  ppu.memory[0x0068] = 0b0000'0000;
//  ppu.memory[0x0069] = 0b0111'1110;
//  ppu.memory[0x006A] = 0b0100'0000;
//  ppu.memory[0x006B] = 0b0100'0000;
//  ppu.memory[0x006C] = 0b0100'0000;
//  ppu.memory[0x006D] = 0b0100'0000;
//  ppu.memory[0x006E] = 0b0100'0000;
//  ppu.memory[0x006F] = 0b0000'0000;

  // 8 x 16 test
//  ppu.oam[0] = 100;
//  ppu.oam[1] = 0b0000'1110;
//  ppu.oam[2] = 0b1100'0011;
//  ppu.oam[3] = 100;
//
//  ppu.memory[0x3F1D] = 48;
//  ppu.memory[0x3F1E] = 38;
//  ppu.memory[0x3F1F] = 39;
//
//  ppu.memory[0x0070] = 0b1111'1111;
//  ppu.memory[0x0071] = 0b1111'1111;
//  ppu.memory[0x0072] = 0b1111'1111;
//  ppu.memory[0x0073] = 0b1111'1111;
//  ppu.memory[0x0074] = 0b1111'1111;
//  ppu.memory[0x0075] = 0b1111'1111;
//  ppu.memory[0x0076] = 0b1111'1111;
//  ppu.memory[0x0077] = 0b1111'1111;
//
//  ppu.memory[0x0078] = 0b1000'0000;
//  ppu.memory[0x0079] = 0b1000'0000;
//  ppu.memory[0x007A] = 0b1000'0000;
//  ppu.memory[0x007B] = 0b1000'0000;
//  ppu.memory[0x007C] = 0b1000'0000;
//  ppu.memory[0x007D] = 0b1000'0000;
//  ppu.memory[0x007E] = 0b1000'0000;
//  ppu.memory[0x007F] = 0b1000'0000;
//
//
//  ppu.memory[0x0080] = 0b1111'1111;
//  ppu.memory[0x0081] = 0b1111'1111;
//  ppu.memory[0x0082] = 0b1111'1111;
//  ppu.memory[0x0083] = 0b1111'1111;
//  ppu.memory[0x0084] = 0b1111'1111;
//  ppu.memory[0x0085] = 0b1111'1111;
//  ppu.memory[0x0086] = 0b1111'1111;
//  ppu.memory[0x0087] = 0b1111'1111;
//
//  ppu.memory[0x0088] = 0b1000'0000;
//  ppu.memory[0x0089] = 0b1000'0000;
//  ppu.memory[0x008A] = 0b1000'0000;
//  ppu.memory[0x008B] = 0b1000'0000;
//  ppu.memory[0x008C] = 0b1000'0000;
//  ppu.memory[0x008D] = 0b1000'0000;
//  ppu.memory[0x008E] = 0b1000'0000;
//  ppu.memory[0x008F] = 0b1111'1111;

  auto start{ std::chrono::system_clock::now() };

  for (int t{}; t < 60; t++) {
    for (int i{}; i < 341 * 256; i++) {
      ppu.executeNextClock();
    }
  }

  auto end{ std::chrono::system_clock::now() };
  std::printf("%lld\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());


  bool quit{false};
  SDL_Event e;

  while (!quit) {
    while (SDL_WaitEvent(&e) != 0) {

      if (e.type == SDL_QUIT) {
        quit = true;

        // Destroy SDL Stuff
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyTexture(texture);
        window = nullptr;
        SDL_Quit();
      }
    }
  }

  return 0;
}

