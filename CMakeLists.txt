# This is a ARM Embedded CMakeLists.txt templete.
#
# You need to change "ARM_COMPILE_PREFIX" to adapt your development environment.
#

cmake_minimum_required(VERSION 3.13)

include(FetchContent)

get_filename_component(CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} NAME)
SET(PRJ_NAME ${CURRENT_FOLDER})

# 开启详细输出
# set(CMAKE_VERBOSE_MAKEFILE ON)

# 编译标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

SET (CMAKE_C_COMPILER_WORKS 1)
SET (CMAKE_CXX_COMPILER_WORKS 1)


#Cmake 交叉编译配置
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_VERBOSE_MAKEFILE ON)

# 设置交叉编译器
set(SHADERC_ENABLE_TESTS ON)
set(ARM_COMPILE_PREFIX "$ENV{HOME}/Library/xPacks/@xpack-dev-tools/arm-none-eabi-gcc/11.3.1-1.1.2/.content/bin")
set(CMAKE_C_COMPILER   "${ARM_COMPILE_PREFIX}/arm-none-eabi-gcc")
set(CMAKE_ASM_COMPILER "${ARM_COMPILE_PREFIX}/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "${ARM_COMPILE_PREFIX}/arm-none-eabi-g++")
set(CMAKE_OBJCOPY      "${ARM_COMPILE_PREFIX}/arm-none-eabi-objcopy")
set(CMAKE_SIZE         "${ARM_COMPILE_PREFIX}/arm-none-eabi-size")
set(CMAKE_OBJDUMP      "${ARM_COMPILE_PREFIX}/bin/arm-none-eabi-objdump")
set(CMAKE_DEBUGER      "${ARM_COMPILE_PREFIX}/arm-none-eabi-gdb")
set(CMAKE_CPPFILT      "${ARM_COMPILE_PREFIX}/bin/arm-none-eabi-c++filt")

# 生成 compile_commands.json 文件
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${PRJ_NAME}.elf VERSION 1.0.1)

# 使能汇编语言
ENABLE_LANGUAGE(ASM)

# set(CMAKE_BUILD_TYPE "DEBUG")

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RELEASE")
endif ()

#设置调试选项
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g2 -ggdb")
SET(CMAKE_C_FLAGS_RELEASE "-Og -g")

add_definitions(
    -DUSE_HAL_DRIVER
    -DSTM32F103xB
)

# cpu
set(CPU  "-mcpu=cortex-m3")
# fpu
set(FPU  "")
# float-abi
set(FLOAT-ABI  "")
# mcu
set(MCU  "${CPU} -mthumb ${FPU} ${FLOAT-ABI}")

set(CMAKE_C_FLAGS   "${MCU} -fno-builtin -Wall -fdata-sections -ffunction-sections")
set(CMAKE_CXX_FLAGS "${MCU} -fno-builtin -Wall -fdata-sections -ffunction-sections")
set(CMAKE_ASM_FLAGS "${MCU} -x assembler-with-cpp")

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

# #设置启动文件变量
SET(START_UP_ASM ${PROJECT_SOURCE_DIR}/startup_stm32f103xb.s)
# #设置启动文件 C 属性
set_property(SOURCE ${START_UP_ASM} PROPERTY LANGUAGE C)

#设置链接文件相关路径和变量
SET(LDSCRIPT ${PROJECT_SOURCE_DIR}/STM32F103C8Tx_FLASH.ld)

#设置链接选项
SET(CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs -u_printf_float -u_sprintf_float -T${LDSCRIPT} -lc -lm -lnosys  -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map,--cref -Wl,--gc-sections")

# #设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin)

include_directories(
  ${PROJECT_SOURCE_DIR}/Core/Inc
  ${PROJECT_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Inc
  ${PROJECT_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
  ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
  ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include
)

add_executable(${PRJ_NAME}.elf
    Core/Src/main.c
)

# file(GLOB SRC_DRIVER SRC/DRIVER/*.c)
# add_library(${PROJECT_NAME}
#   ${SRC_DRIVER}
# )

add_library(STM32_Library
  Core/Src/stm32f1xx_it.c
  Core/Src/stm32f1xx_hal_msp.c
  Core/Src/system_stm32f1xx.c  
  ${START_UP_ASM}
)

add_library(HAL_Library
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c
)

target_link_libraries(${PRJ_NAME}.elf STM32_Library HAL_Library)

option(printf "This is a option for printf library" ON)
if(printf)
  FetchContent_Declare(printf
      GIT_REPOSITORY https://github.com/eyalroz/printf.git
      GIT_TAG v6.1.0 # Replace this with a real available version
      SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Core/Lib/printf
  )
  FetchContent_MakeAvailable(printf)
  target_link_libraries(${PRJ_NAME}.elf printf)
endif()

# Convert ELF File to HEX and BIN File.
add_custom_command(TARGET "${PRJ_NAME}.elf" POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE_OUTPUT_PATH}/${PRJ_NAME}.elf ${EXECUTABLE_OUTPUT_PATH}/${PRJ_NAME}.bin
  COMMAND ${CMAKE_OBJCOPY} -O ihex   ${EXECUTABLE_OUTPUT_PATH}/${PRJ_NAME}.elf ${EXECUTABLE_OUTPUT_PATH}/${PRJ_NAME}.hex
  COMMENT "Building ${PRJ_NAME}.bin and ${PRJ_NAME}.hex"
  COMMAND ${CMAKE_COMMAND} -E copy ${EXECUTABLE_OUTPUT_PATH}/${PRJ_NAME}.hex "${CMAKE_CURRENT_BINARY_DIR}/${PRJ_NAME}.hex"
  COMMAND ${CMAKE_COMMAND} -E copy ${EXECUTABLE_OUTPUT_PATH}/${PRJ_NAME}.bin "${CMAKE_CURRENT_BINARY_DIR}/${PRJ_NAME}.bin"
  # Show code section size
  COMMAND ${CMAKE_SIZE} --format=berkeley ${EXECUTABLE_OUTPUT_PATH}/${PRJ_NAME}.elf ${EXECUTABLE_OUTPUT_PATH}/${PRJ_NAME}.hex
)
