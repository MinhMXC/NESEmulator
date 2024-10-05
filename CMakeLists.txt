cmake_minimum_required(VERSION 3.27)
project(NESEmulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

SET(SDL2_PATH "C:/Users/MinhNguyen/CLionProjects/NESEmulator/SDL2")
SET(SDL2_IMAGE_PATH "C:/Users/MinhNguyen/CLionProjects/NESEmulator/SDL2_image")
SET(SDL2_TTF_PATH "C:/Users/MinhNguyen/CLionProjects/NESEmulator/SDL2_ttf")
SET(Catch2_PATH "C:/Users/MinhNguyen/CLionProjects/NESEmulator/Catch2")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(Catch2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
include_directories(${SDL2_TTF_INCLUDE_DIRS})

add_executable(NESEmulator src/main.cpp
        src/constants.h
        src/initializer/initializer.h
        src/display/display.h
        src/ppu/ppu.h
        src/ppu/ppu.cpp
        src/display/display.cpp
        src/utils.h
        src/utils.cpp
        src/cpu/cpu.h
        src/cpu/cpu.cpp
        src/cpu/addressingMode.cpp
        src/input_handler/input_handler.cpp
        src/display/debug_display.h
        src/display/debug_display.cpp
)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${SDL2_IMAGE_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${SDL2_TTF_LIBRARY})

add_executable(PPUTest src/test/PPUTest.cpp
        src/display/display.h
        src/display/display.cpp
        src/ppu/ppu.h
        src/ppu/ppu.cpp
        src/utils.h
        src/utils.cpp
)

target_link_libraries(PPUTest Catch2::Catch2WithMain)
target_link_libraries(PPUTest ${SDL2_LIBRARY})

add_executable(LoadStoreTest src/test/cpu/loadStoreTest.cpp
        src/constants.h
        src/initializer/initializer.h
        src/display/display.h
        src/ppu/ppu.h
        src/ppu/ppu.cpp
        src/display/display.cpp
        src/utils.h
        src/utils.cpp
        src/cpu/cpu.h
        src/cpu/cpu.cpp
        src/cpu/addressingMode.cpp
)
target_link_libraries(LoadStoreTest Catch2::Catch2WithMain)
target_link_libraries(LoadStoreTest ${SDL2_LIBRARY})


add_executable(RegisterTransferTest src/test/cpu/registerTransferTest.cpp
        src/constants.h
        src/initializer/initializer.h
        src/display/display.h
        src/ppu/ppu.h
        src/ppu/ppu.cpp
        src/display/display.cpp
        src/utils.h
        src/utils.cpp
        src/cpu/cpu.h
        src/cpu/cpu.cpp
        src/cpu/addressingMode.cpp
)
target_link_libraries(RegisterTransferTest Catch2::Catch2WithMain)
target_link_libraries(RegisterTransferTest ${SDL2_LIBRARY})


add_executable(StackOperationTest src/test/cpu/stackOperationTest.cpp
        src/constants.h
        src/initializer/initializer.h
        src/display/display.h
        src/ppu/ppu.h
        src/ppu/ppu.cpp
        src/display/display.cpp
        src/utils.h
        src/utils.cpp
        src/cpu/cpu.h
        src/cpu/cpu.cpp
        src/cpu/addressingMode.cpp
)
target_link_libraries(StackOperationTest Catch2::Catch2WithMain)
target_link_libraries(StackOperationTest ${SDL2_LIBRARY})


add_executable(LogicalTest src/test/cpu/logicalTest.cpp
        src/constants.h
        src/initializer/initializer.h
        src/display/display.h
        src/ppu/ppu.h
        src/ppu/ppu.cpp
        src/display/display.cpp
        src/utils.h
        src/utils.cpp
        src/cpu/cpu.h
        src/cpu/cpu.cpp
        src/cpu/addressingMode.cpp
)
target_link_libraries(LogicalTest Catch2::Catch2WithMain)
target_link_libraries(LogicalTest ${SDL2_LIBRARY})


add_executable(ArithmeticTest src/test/cpu/arithmeticTest.cpp
        src/constants.h
        src/initializer/initializer.h
        src/display/display.h
        src/ppu/ppu.h
        src/ppu/ppu.cpp
        src/display/display.cpp
        src/utils.h
        src/utils.cpp
        src/cpu/cpu.h
        src/cpu/cpu.cpp
        src/cpu/addressingMode.cpp
)
target_link_libraries(ArithmeticTest Catch2::Catch2WithMain)
target_link_libraries(ArithmeticTest ${SDL2_LIBRARY})


add_executable(IncrementDecrementTest src/test/cpu/incrementDecrementTest.cpp
        src/constants.h
        src/initializer/initializer.h
        src/display/display.h
        src/ppu/ppu.h
        src/ppu/ppu.cpp
        src/display/display.cpp
        src/utils.h
        src/utils.cpp
        src/cpu/cpu.h
        src/cpu/cpu.cpp
        src/cpu/addressingMode.cpp
)
target_link_libraries(IncrementDecrementTest Catch2::Catch2WithMain)
target_link_libraries(IncrementDecrementTest ${SDL2_LIBRARY})


add_executable(ShiftTest src/test/cpu/shiftTest.cpp
        src/constants.h
        src/initializer/initializer.h
        src/display/display.h
        src/ppu/ppu.h
        src/ppu/ppu.cpp
        src/display/display.cpp
        src/utils.h
        src/utils.cpp
        src/cpu/cpu.h
        src/cpu/cpu.cpp
        src/cpu/addressingMode.cpp
)
target_link_libraries(ShiftTest Catch2::Catch2WithMain)
target_link_libraries(ShiftTest ${SDL2_LIBRARY})


add_executable(JumpCallTest src/test/cpu/jumpCallTest.cpp
        src/constants.h
        src/initializer/initializer.h
        src/display/display.h
        src/ppu/ppu.h
        src/ppu/ppu.cpp
        src/display/display.cpp
        src/utils.h
        src/utils.cpp
        src/cpu/cpu.h
        src/cpu/cpu.cpp
        src/cpu/addressingMode.cpp
)
target_link_libraries(JumpCallTest Catch2::Catch2WithMain)
target_link_libraries(JumpCallTest ${SDL2_LIBRARY})


add_executable(BranchTest src/test/cpu/branchTest.cpp
        src/constants.h
        src/initializer/initializer.h
        src/display/display.h
        src/ppu/ppu.h
        src/ppu/ppu.cpp
        src/display/display.cpp
        src/utils.h
        src/utils.cpp
        src/cpu/cpu.h
        src/cpu/cpu.cpp
        src/cpu/addressingMode.cpp
)
target_link_libraries(BranchTest Catch2::Catch2WithMain)
target_link_libraries(BranchTest ${SDL2_LIBRARY})


add_executable(StatusFlagTest src/test/cpu/statusFlagTest.cpp
        src/constants.h
        src/initializer/initializer.h
        src/display/display.h
        src/ppu/ppu.h
        src/ppu/ppu.cpp
        src/display/display.cpp
        src/utils.h
        src/utils.cpp
        src/cpu/cpu.h
        src/cpu/cpu.cpp
        src/cpu/addressingMode.cpp
)
target_link_libraries(StatusFlagTest Catch2::Catch2WithMain)
target_link_libraries(StatusFlagTest ${SDL2_LIBRARY})