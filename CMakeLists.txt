cmake_minimum_required(VERSION 3.22)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

set(CPU_PARAMETERS "-mcpu=cortex-m3 -mthumb -mfloat-abi=soft")

set(CMAKE_C_FLAGS "${CPU_PARAMETERS} -std=gnu11 -Wall -Wextra -Og -g3 -ggdb")

set(CMAKE_EXE_LINKER_FLAGS
    "${CPU_PARAMETERS} \
     -T${CMAKE_SOURCE_DIR}/linker/STM32F103RBTx_FLASH.ld \
     --specs=nosys.specs \
     -Wl,-Map=${CMAKE_PROJECT_NAME}.map \
     -Wl,--print-memory-usage"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    SUFFIX ".elf"
)

project(blink_registers C ASM)

add_executable(${PROJECT_NAME}
    src/main.c
    src/system_stm32f1xx.c
    startup/startup_stm32f103xb.s
)

target_include_directories(${PROJECT_NAME} PRIVATE
    CMSIS/Core/Include
    CMSIS/Device/ST/STM32F1xx/Include
    src
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    STM32F103xB
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
)