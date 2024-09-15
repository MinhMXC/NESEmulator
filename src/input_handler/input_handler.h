#ifndef NESEMULATOR_INPUT_HANDLER_H
#define NESEMULATOR_INPUT_HANDLER_H

#include <cstdint>
#include <SDL.h>

typedef uint8_t byte;

class InputHandler {
public:
  InputHandler();

  void handleEvent(SDL_Event& event);
  void handleKeyboardState();
  bool readInput();
  void resetRead();
  void startPollInput();
  void endPollInput();

private:
  bool poll;
  byte input;
  uint64_t readState{};
};

#endif
