#This is just a stm32 CMakeLists.txt templete

cmake_minimum_required(VERSION 3.10)

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
set(ARM_COMPILE_PREFIX "/opt/homebrew/bin/")
set(CMAKE_C_COMPILER "${ARM_COMPILE_PREFIX}/arm-none-eabi-gcc")
set(CMAKE_ASM_COMPILER "${ARM_COMPILE_PREFIX}/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "${ARM_COMPILE_PREFIX}/arm-none-eabi-g++")
set(CMAKE_OBJCOPY "${ARM_COMPILE_PREFIX}/arm-none-eabi-objcopy")
set(CMAKE_SIZE "${ARM_COMPILE_PREFIX}/arm-none-eabi-size")
set(CMAKE_OBJDUMP "${ARM_COMPILE_PREFIX}/bin/arm-none-eabi-objdump")
set(CMAKE_DEBUGER "${ARM_COMPILE_PREFIX}/arm-none-eabi-gdb")
set(CMAKE_CPPFILT "${ARM_COMPILE_PREFIX}/bin/arm-none-eabi-c++filt")

# 生成 compile_commands.json 文件
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${PRJ_NAME})

# 使能汇编语言
ENABLE_LANGUAGE(ASM)

# set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_BUILD_TYPE "RELEASE")

# #设置浮点选项
# SET(MCU_FLAGS "-mcpu=cortex-m3  -mthumb")
# #设置警告相关信息
# SET(CMAKE_C_FLAGS "${MCU_FLAGS} -Wno-unknown-pragmas")
# #设置调试选项
# SET(CMAKE_C_FLAGS_DEBUG "-O0 -g2 -ggdb")
# SET(CMAKE_C_FLAGS_RELEASE "-Og -g")

add_definitions(
    -DUSE_HAL_DRIVER
    -DSTM32F103xB
    #-D__SOFTFP__=1
)

# cpu
set(CPU  "-mcpu=cortex-m3")
# fpu
set(FPU  "")
# float-abi
set(FLOAT-ABI  "")
# mcu
set(MCU  "${CPU} -mthumb ${FPU} ${FLOAT-ABI}")

set(OPT  "-Og")

set(CMAKE_C_FLAGS "${MCU} -fno-builtin ${OPT}  -Wall -fdata-sections -ffunction-sections")
set(CMAKE_CXX_FLAGS "${MCU} -fno-builtin ${OPT} -Wall -fdata-sections -ffunction-sections")
set(CMAKE_ASM_FLAGS "${MCU} -x assembler-with-cpp")

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

# #设置启动文件变量
SET(START_UP_ASM ${PROJECT_SOURCE_DIR}/startup_stm32f103xb.s)
# #设置启动文件 C 属性
set_property(SOURCE ${START_UP_ASM} PROPERTY LANGUAGE C)

include_directories(
  ${PROJECT_SOURCE_DIR}/Core/Inc
  ${PROJECT_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
  ${PROJECT_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Inc
  ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include
  ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
  ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3
  ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
  ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include
)

#设置链接文件相关路径和变量
SET(LDSCRIPT ${PROJECT_SOURCE_DIR}/STM32F103C8Tx_FLASH.ld)

#设置链接选项
SET(CMAKE_EXE_LINKER_FLAGS "--specs=nano.specs -T${LDSCRIPT} -lc -lm -lnosys  -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map,--cref -Wl,--gc-sections")

add_executable(${PRJ_NAME}.elf 
    Core/Src/main.c
    Core/Src/gpio.c
    Core/Src/freertos.c
    Core/Src/usart.c
    Core/Src/stm32f1xx_it.c
    Core/Src/stm32f1xx_hal_msp.c
    Core/Src/stm32f1xx_hal_timebase_tim.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
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
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c
    Core/Src/system_stm32f1xx.c
    Middlewares/Third_Party/FreeRTOS/Source/croutine.c
    Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
    Middlewares/Third_Party/FreeRTOS/Source/list.c
    Middlewares/Third_Party/FreeRTOS/Source/queue.c
    Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
    Middlewares/Third_Party/FreeRTOS/Source/tasks.c
    Middlewares/Third_Party/FreeRTOS/Source/timers.c
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c
    Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c  
    ${START_UP_ASM}
)

# #设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin)

#设置 ELF 转换路径
SET(ELF_FILE ${PROJECT_SOURCE_DIR}/build/bin/${PRJ_NAME}.elf)
SET(HEX_FILE ${PROJECT_SOURCE_DIR}/build/bin/${PRJ_NAME}.hex)
SET(BIN_FILE ${PROJECT_SOURCE_DIR}/build/bin/${PRJ_NAME}.bin)
#添加自定义命令实现 ELF 转换 hex 和 bin 文件
add_custom_command(TARGET "${PRJ_NAME}.elf" POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O binary ${ELF_FILE} ${BIN_FILE}
  COMMAND ${CMAKE_OBJCOPY} -O ihex ${ELF_FILE} ${HEX_FILE}
  COMMENT "Building ${PRJ_NAME}.bin and ${PRJ_NAME}.hex"
  COMMAND ${CMAKE_COMMAND} -E copy ${HEX_FILE} "${CMAKE_CURRENT_BINARY_DIR}/${PRJ_NAME}.hex"
  COMMAND ${CMAKE_COMMAND} -E copy ${BIN_FILE} "${CMAKE_CURRENT_BINARY_DIR}/${PRJ_NAME}.bin"
)
# 显示代码段大小
add_custom_command(TARGET "${PRJ_NAME}.elf" POST_BUILD
  COMMAND ${CMAKE_SIZE} --format=berkeley ${ELF_FILE} ${HEX_FILE}
)

