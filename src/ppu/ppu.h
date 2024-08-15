#ifndef NESEMULATOR_PPU_H
#define NESEMULATOR_PPU_H

#include "../display/display.h"
#include <vector>
#include <cstdint>
#include <queue>

using byte = uint8_t;
using word = uint16_t;

class Initializer;

class PPU {
  friend class Initializer;

public:
  explicit PPU(Display& display);

  byte readPPUStatus();
  [[nodiscard]] byte readOAMData() const;
  byte readPPUData();

  void writePPUCtrl(byte val);
  void writePPUMask(byte val);
  void writeOAMAddr(byte val);
  void writeOAMData(byte val);
  void writePPUScroll(byte val);
  void writePPUAddr(byte val);
  void writePPUData(byte val);
  void writeOAMDma(std::vector<byte>& cpuMem, int input);

  void executeNextClock();

  // TODO move back to private once done testing
  std::vector<byte> memory;
  std::vector<byte> oam;
  std::vector<byte> secondaryOam;

private:
  Display& display;

  // For keep track
  int cycle;
  int scanline;
  bool isEvenFrame;

  // Memory

  // Register
  byte ppuCtrl;
  byte ppuMask;
  byte ppuStatus;
  byte oamAddr;

  // Internal Register
  word v;
  word t;
  word x;
  word w;

  // For background rendering
  std::queue<byte> attrQueue; // Because attr data is also delayed by 16pixel
  word lowBGShiftRegister;
  word highBGShiftRegister;

  // Check whether it is the first frame
  bool first;

  // Store the 256 pixel data for the sprite in next scanline
  // first 5 bit == palette index, 5th bit == sprite 0, 6th bit == priority, 7th bit == hasBeenWrittenTo
  std::vector<byte> spritePixelData;

  void handleVisibleScanline();
  void handleBackgroundFetching();
  void handlePreRenderScanline();
  void handleSpriteEvaluation();
  void evaluateSpriteForNextClock();
  void renderNextScanlineSprite(int index);
  void handleDraw(bool render);
  bool isRendering() const;
};

#endif
