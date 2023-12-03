cmake_minimum_required(VERSION 3.15)
project(tail_bootloader C ASM)

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

# Compiling
add_custom_target(compile
    COMMAND gcc -c -g -Os -march=x86-64 -ffreestanding -Wall -Werror ${SRC_DIR}/main.c -o ${BUILD_DIR}/main.o
    COMMAND ld -static -T ${SRC_DIR}/scripts/linker.ld -nostdlib --nmagic -o ${BUILD_DIR}/tail_bootloader.elf ${ASSEMBLY_OBJECTS}
    COMMAND objcopy -O binary ${BUILD_DIR}/tail_bootloader.elf ${BUILD_DIR}/tail_bootloader.bin
    COMMAND dd if=${BUILD_DIR}/tail_bootloader.bin of=${BUILD_DIR}/tail_bootloader.img
    DEPENDS assembly_lib
    COMMENT "Compiling TAIL.."
)

# Run TAIL bootloader in QEMU with showed interrupts.
add_custom_target(run_int
    COMMAND qemu-system-x86_64 -d int -boot c ${BUILD_DIR}/tail_bootloader.img
    DEPENDS compile
    COMMENT "Running QEMU.."
)

# Run TAIL bootloader in QEMU in debug mode with GDB debugger.
add_custom_target(run
    COMMAND qemu-system-x86_64 -boot c ${BUILD_DIR}/tail_bootloader.img -s -S -no-reboot -no-shutdown &
    COMMAND echo "Waiting for QEMU to start..."
    COMMAND sleep 2
    COMMAND gdb -ex "target remote :1234" -ex symbol-file ${BUILD_DIR}/tail_bootloader.bin -ex "layout asm"
    DEPENDS compile
    COMMENT "Running QEMU.."
)