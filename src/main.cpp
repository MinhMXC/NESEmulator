// #define NDEBUG
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cstdio>
#include <iostream>
#include <chrono>
#include <thread>

#include "constants.h"
#include "./display/display.h"
#include "utils.h"
#include "ppu/ppu.h"
#include "cpu/cpu.h"
#include "initializer/initializer.h"
#include "display/debug_display.h"


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

  if (TTF_Init() == -1) {
    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", SDL_GetError());
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
  InputHandler inputHandler{};
  CPU cpu{ppu, inputHandler};
  Initializer initializer{cpu, ppu};

  std::string res{ initializer.loadFile("../test_rom/supermariobros.nes") };
  if (!res.empty()) {
    printf("Error: %s\n", res.c_str());
  }

  // Debug Screen
  SDL_Window* debugWindow = SDL_CreateWindow(
    "Debug",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    1028,
    1100,
    SDL_WINDOW_SHOWN
  );
  if (debugWindow == nullptr) {
    printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
  }

  // Making Renderer
  SDL_Renderer* debugRenderer = SDL_CreateRenderer(debugWindow, -1, SDL_RENDERER_ACCELERATED);
  if (debugRenderer == nullptr) {
    printf("Cannot create renderer! SDL Error: %s\n", SDL_GetError());
    return -1;
  }

  SDL_Texture* debugTexture = SDL_CreateTexture(
    debugRenderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    EmuConst::SCALED_SCREEN_WIDTH,
    EmuConst::SCALED_SCREEN_HEIGHT
  );
  if (debugTexture == nullptr) {
    printf("Cannot create texture! SDL Error: %s\n", SDL_GetError());
    return -1;
  }

  DebugDisplay debugDisplay{ppu, debugRenderer, debugTexture};

  // freopen("log.txt", "w", stdout);


  cpu.executeStartUpSequence();

  bool quit{false};
  uint64_t lastTotalCycle{cpu.totalCycle};
  SDL_Event e;
//
//  while (!quit) {
//    while (SDL_WaitEvent(&e)) {
//      if (e.type == SDL_QUIT) {
//        quit = true;
//
//        // Destroy SDL Stuff
//        SDL_DestroyWindow(window);
//        SDL_DestroyRenderer(renderer);
//        SDL_DestroyTexture(texture);
//        window = nullptr;
//        SDL_Quit();
//      }
//    }
//  }

  while (!quit) {
    auto start{ std::chrono::system_clock::now() };

    inputHandler.resetRead();

    debugDisplay.updateScreen();

    // Handle Event
    while (SDL_PollEvent(&e)) {
      inputHandler.handleEvent(e);

      if (e.key.keysym.sym == SDLK_0 && e.type == SDL_KEYDOWN) {
        for (int i{0x2000}; i < 0x23C0; i++) {
          if (i % 0x20 == 0)
            printf("\n");
          printf("%02X ", ppu.memory[i]);
        }

        printf("\n");

        for (int i{0x23C0}; i < 0x2400; i++) {
          if (i % 0x08 == 0)
            printf("\n");
          printf("%02X ", ppu.memory[i]);
        }

        printf("\n");
//
//        for (int i{}; i < 256; i++) {
//          if (i % 0x10 == 0)
//            printf("\n");
//          printf("%02X ", ppu.oam[i]);
//        }
      }

      if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
        quit = true;

        // Destroy SDL Stuff
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyTexture(texture);

        SDL_DestroyWindow(debugWindow);
        SDL_DestroyRenderer(debugRenderer);
        SDL_DestroyTexture(debugTexture);

        window = nullptr;
        SDL_Quit();
      }
    }

    // Handle Keyboard State
    inputHandler.handleKeyboardState();

    // Execute CPU
    while (true) {
      cpu.executeNextClock();
      if ((cpu.totalCycle - lastTotalCycle) > 29833) {
        lastTotalCycle = cpu.totalCycle;
        break;
      }
    }

    using namespace std::chrono_literals;

    // std::this_thread::sleep_until(start + 15.6ms);

    auto end{ std::chrono::system_clock::now() };
    // printf("%lld\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
  }

  return 0;
}
