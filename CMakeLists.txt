cmake_minimum_required(VERSION 3.15)
project(test_project C ASM)

# Project paths
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)

# Custom target for assembly files
file(GLOB ASSEMBLY_SOURCES ${SRC_DIR}/*.asm)

# Custom target for objects
file(GLOB ASSEMBLY_OBJECTS ${BUILD_DIR}/*.o)

# Compile assembly files to object files
add_custom_target(assembly_lib
    COMMAND mkdir -p ${BUILD_DIR}
    COMMENT "Assembling.."
)

foreach(ASSEMBLY_FILE IN LISTS ASSEMBLY_SOURCES)
    get_filename_component(FILENAME ${ASSEMBLY_FILE} NAME_WE)
    add_custom_command(TARGET assembly_lib
        COMMAND nasm -f elf64 -o ${BUILD_DIR}/${FILENAME}.o ${ASSEMBLY_FILE}
        COMMENT "Assembling: ${FILENAME}.asm"
    )
endforeach()

# Add custom target for compiling main.c to object file
add_custom_target(compile
    COMMAND gcc -c -g -Os -march=x86-64 -ffreestanding -Wall -Werror ${SRC_DIR}/main.c -o ${BUILD_DIR}/main.o
    COMMAND ld -T ${SRC_DIR}/scripts/linker.ld -nostdlib --nmagic -o ${BUILD_DIR}/tail_bootloader.elf ${ASSEMBLY_OBJECTS}
    COMMAND objcopy -O binary ${BUILD_DIR}/tail_bootloader.elf ${BUILD_DIR}/tail_bootloader.bin
    COMMAND dd if=${BUILD_DIR}/tail_bootloader.bin of=tail_bootloader.img
    DEPENDS assembly_lib
    COMMENT "Compiling TAIL.."
)

# Link assembly objects with main.c and generate tail_bootloader.img
add_custom_target(run
    COMMAND qemu-system-x86_64 -d int -boot c ${BUILD_DIR}/tail_bootloader.img
    DEPENDS compile
    COMMENT "Running QEMU.."
)

add_compile_options("-c -g -Os -march=x86-64 -ffreestanding -Wall -Werror")
add_link_options("-static -nostdlib --nmagic")