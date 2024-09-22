#ifndef NESEMULATOR_PPU_H
#define NESEMULATOR_PPU_H

#include "../display/display.h"
#include <vector>
#include <cstdint>
#include <queue>

/**
 * \brief First 5 bit = paletteIndex, bit 5 = isSprite0, bit 6 = priority, bit 7 = hasBeenWrittenTo
 */
using SpriteData = uint8_t;
using Byte = uint8_t;
using Word = uint16_t;

class Initializer;
class PPU;

/**
 * \brief Class to handle <a href="https://www.nesdev.org/wiki/PPU_OAM">OAM</a> Operations
 */
class OAM {
public:
  /**
   * \brief Initialise OAM
   * \param ppu Reference to the PPU that will use this OAM
   */
  explicit OAM(PPU& ppu);

  /**
   * \brief Get the pixelData at the specified x-position
   * \param x the x-position
   * \note The y-position is based on the current scanline of the PPU. If PPU current scanline is 1 then y-position is 0,
   * i.e y-position = scanline - 1. This is because of how the Sprite Evaluation work in NES.
   * \warning This must be used in conjunction with tick(). This will clear the spritePixelData at that location.
   */
  Byte getPixelData(int x);

  /**
   * \brief Execute the OAM operation at the current cycle and scanline of PPU
   * \note Call this function from scanline 0 to scanline 239 (inclusive) of PPU
   */
  void tick();

  /**
   * \brief Set oamAddr to input
   * \param input the Byte to be written
   */
  void writeOAMAddr(Byte input);

  /**
   * \brief Return the OAM Memory at the current oamAddr
   * \note If read when PPU is rendering between cycle 1 and 64 during visible scanline, 0xFF will be returned
   */
  Byte readOAMData();

  /**
   * \brief Write a Byte to OAM Memory at the current oamAddr
   * \param input the Byte to be written
   * \note This will increment oamAddr by 1 after writing.
   * Write to oamAddr % 4 == 2 will discard bit 2 to bit 4 as NES OAM Attribute Memory does
   * not have bit 2 to bit 4
   */
  void writeOAMData(Byte input);

  /**
   * \brief <a href="https://www.nesdev.org/wiki/DMA#:~:text=Examples%20%2D%20General%20behavior-,OAM%20DMA,-OAM%20DMA%20copies">OAM DMA</a> Process
   * \param cpuMem reference to CPU Memory Array
   * \param input If input is XX, data is from CPU page $XX00–$XXFF
   */
  void dma(std::vector<Byte>& cpuMem, Byte input);

  /**
   * \brief The 256 Byte SpriteData to store the result of the OAM process executed in PPU scanline - 1
   */
  std::vector<SpriteData> spritePixelData;

private:
  /**
   * \brief Execute the OAM sprite evaluation process at the current cycle and scanline of PPU
   * \note Call this function from cycle 65 to 256 from scanline 0 to scanline 239 of PPU.
   * This will not be cycle accurate as doing so will be quite complicated and
   * potentially less performant
   */
  void evaluateOAM();

  /**
   * \brief Convert SecondaryOAM content to SpriteData
   * \note Call this function from cycle 257 to 320 from scanline 0 to scanline 239 of PPU when
   * scanline % 8 == 0 (This function will be called exactly 8 times)
   */
  void evaluateSpriteData(int index);

  /**
   * \brief Reference to the PPU that own this OAM
   */
  PPU& ppu;

  /**
   * \brief The 256 Byte OAM memory
   */
  std::vector<Byte> oam;

  /**
   * \brief The 32 Byte
   * <a href="https://www.nesdev.org/wiki/PPU_OAM#:~:text=8%20sprites)%20of-,secondary%20OAM,-memory%20that%20is">
   * secondary OAM memory
   * </a> for internal operations
   */
  std::vector<Byte> secondaryOam;

  /**
   * \brief
   * <a href="https://www.nesdev.org/wiki/PPU_registers#OAMADDR:~:text=Common%20name%3A-,OAMADDR,-Description%3A%20OAM%20address">
   * OAMADDR
   * </a> register
   */
  Byte oamAddr;

  /**
   * \brief Address of the next free byte in secondary OAM
   */
  int secondaryOamAddr;

  /**
   * \brief Set if Sprite Evaluation has reached the end
   */
  bool spriteEvaluationEnd;

  /**
   * \brief Memory read offset during weird sprite overflow process
   */
  int readOffset;

  /**
   * \brief Store the OAM Data read on odd cycle
   */
   Byte isSecondaryOamClearing;
};



class PPU {
  friend class Initializer;
  friend class OAM;

public:
  explicit PPU(Display& display);

  Byte readPPUStatus();
  [[nodiscard]] Byte readOAMData();
  Byte readPPUData();

  void writePPUCtrl(Byte val);
  void writePPUMask(Byte val);
  void writeOAMAddr(Byte val);
  void writeOAMData(Byte val);
  void writePPUScroll(Byte val);
  void writePPUAddr(Byte val);
  void writePPUData(Byte val);
  void writeOAMDma(std::vector<Byte>& cpuMem, Byte input);

  // Emulation
  Byte readPPUStatusNoSideEffect() const;
  Byte readPPUCtrlNoSideEffect() const;

  void executeNextClock();

  // TODO move back to private once done testing
  std::vector<Byte> memory;

  // Emulation
  int cycle;
  int scanline;
  bool isEvenFrame;
  int frame;
  bool disableNextNMI;
  bool nametableArrangement; // 0 = horizontal, 1 = vertical
private:
  Display& display;
  OAM oam;

  // Memory

  // Register
  Byte ppuCtrl;
  Byte ppuMask;
  Byte ppuStatus;

  // Internal Register
  Word v;
  Word t;
  Word x;
  Word w;

  // For background rendering
  std::queue<Byte> attrQueue; // Because attr data is also delayed by 16pixel
  Word lowBGShiftRegister;
  Word highBGShiftRegister;

  // Check whether it is the first frame
  bool first;

  // Store the 256 pixel data for the sprite in next scanline
  // first 5 bit == palette index, 5th bit == sprite 0, 6th bit == priority, 7th bit == hasBeenWrittenTo
  std::vector<Byte> spritePixelData;

  // Memory Mapping
  Byte readMemory(Word addr);
  void writeMemory(Word addr, Byte input);
  Word mapMemory(Word addr) const;

  void handleVisibleScanline();
  void handleBackgroundFetching();
  void handlePreRenderScanline();
  void handleDraw(bool render);
  [[nodiscard]] bool isRendering() const;
};

#endif
