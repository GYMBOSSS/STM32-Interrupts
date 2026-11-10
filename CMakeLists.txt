cmake_minimum_required(VERSION 3.22)
 
# --- Toolchain (must be set before project()) -------------------------------
set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
 
set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY      arm-none-eabi-objcopy)
set(CMAKE_SIZE         arm-none-eabi-size)
 
# The compiler check must not try to link a full executable (no startup code yet).
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
 
project(stm32-baremetal-driver C ASM)
 
# --- Flags -------------------------------------------------------------------
set(CPU_PARAMETERS -mcpu=cortex-m3 -mthumb -mfloat-abi=soft)
 
add_compile_options(
    ${CPU_PARAMETERS}
    -Wall -Wextra
    -ffunction-sections -fdata-sections
    $<$<CONFIG:Debug>:-Og>
    $<$<CONFIG:Debug>:-g3>
    $<$<CONFIG:Release>:-Os>
)
add_compile_options($<$<COMPILE_LANGUAGE:C>:-std=gnu11>)
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)
 
add_link_options(
    ${CPU_PARAMETERS}
    -T${CMAKE_SOURCE_DIR}/linker/STM32F103RBTx_FLASH.ld
    --specs=nano.specs
    --specs=nosys.specs
    -Wl,--gc-sections
    -Wl,-Map=${PROJECT_NAME}.map
    -Wl,--print-memory-usage
)
 
# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()
 
# --- Target ------------------------------------------------------------------
add_executable(${PROJECT_NAME}
    src/main.c
    src/system_stm32f1xx.c
    src/peripherals/clock/clock.c
    src/peripherals/gpio/gpio.c
    src/peripherals/led/led.c
    src/peripherals/ring_buffer/ring_buffer.c
    src/peripherals/systick/systick.c
    src/peripherals/usart/usart.c
    startup/startup_stm32f103xb.s
)
 
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")
 
target_include_directories(${PROJECT_NAME} PRIVATE
    CMSIS/Core/Include
    CMSIS/Device/ST/STM32F1xx/Include
    src
    src/peripherals/clock
    src/peripherals/gpio
    src/peripherals/led
    src/peripherals/ring_buffer
    src/peripherals/systick
    src/peripherals/usart
)
 
target_compile_definitions(${PROJECT_NAME} PRIVATE STM32F103xB)
 
# --- Post-build: hex, bin, size ---------------------------------------------
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
)
 
# --- Flash target: `cmake --build build --target flash` -----------------------
# Assumes flash.cfg selects the interface and target only (e.g. stlink + stm32f1x).
find_program(OPENOCD openocd)
if(OPENOCD)
    add_custom_target(flash
        COMMAND ${OPENOCD} -f ${CMAKE_SOURCE_DIR}/flash.cfg
                -c "program $<TARGET_FILE:${PROJECT_NAME}> verify reset exit"
        DEPENDS ${PROJECT_NAME}
        COMMENT "Flashing ${PROJECT_NAME}.elf with OpenOCD"
    )
endif()