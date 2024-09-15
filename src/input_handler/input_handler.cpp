#include "input_handler.h"
#include <cstdint>
#include <cstdio>

constexpr byte A        = 0b0000'0001;
constexpr byte B        = 0b0000'0010;
constexpr byte SELECT   = 0b0000'0100;
constexpr byte START    = 0b0000'1000;
constexpr byte UP       = 0b0001'0000;
constexpr byte DOWN     = 0b0010'0000;
constexpr byte LEFT     = 0b0100'0000;
constexpr byte RIGHT    = 0b1000'0000;

InputHandler::InputHandler() : input{}, poll{}, readState{} {}

void InputHandler::handleEvent(SDL_Event& event) {
  // TODO: Implement polling input

  // Only handle keydown event
  if (event.type == SDL_KEYUP)
    return;

  switch (event.key.keysym.sym) {
    case SDLK_a:
      input |= A;
      break;
    case SDLK_b:
      input |= B;
      break;
    case SDLK_q:
      input |= SELECT;
      break;
    case SDLK_e:
      input |= START;
      break;
    case SDLK_UP:
      input |= UP;
      break;
    case SDLK_DOWN:
      input |= DOWN;
      break;
    case SDLK_LEFT:
      input |= LEFT;
      break;
    case SDLK_RIGHT:
      input |= RIGHT;
      break;
    default:
      break;
  }
}

void InputHandler::handleKeyboardState() {
  const byte* keyboardState{ SDL_GetKeyboardState(nullptr) };

  if (keyboardState[SDL_SCANCODE_Q])
    input |= SELECT;

  if (keyboardState[SDL_SCANCODE_E])
    input |= START;

  if (keyboardState[SDL_SCANCODE_A])
    input |= A;

  if (keyboardState[SDL_SCANCODE_B])
    input |= B;

  if (keyboardState[SDL_SCANCODE_LEFT])
    input |= LEFT;

  if (keyboardState[SDL_SCANCODE_RIGHT])
    input |= RIGHT;

  if (keyboardState[SDL_SCANCODE_UP])
    input |= UP;

  if (keyboardState[SDL_SCANCODE_DOWN])
    input |= DOWN;
}

bool InputHandler::readInput() {
  bool res{};

  switch (readState % 8) {
    case 0:
      res = input & A;
      break;
    case 1:
      res = input & B;
      break;
    case 2:
      res = input & SELECT;
      break;
    case 3:
      res = input & START;
      break;
    case 4:
      res = input & UP;
      break;
    case 5:
      res = input & DOWN;
      break;
    case 6:
      res = input & LEFT;
      break;
    case 7:
      res = input & RIGHT;
      break;
  }

  readState++;
  return res;
}

void InputHandler::resetRead() {
  // readState = 0;
  input = 0;
}

void InputHandler::startPollInput() {
  poll = true;
}

void InputHandler::endPollInput() {
  poll = false;
}