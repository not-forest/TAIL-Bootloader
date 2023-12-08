cmake_minimum_required(VERSION 3.15)
project(tail_bootloader C ASM)

# Project paths
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)

# Custom glob for assembly files
file(GLOB ASSEMBLY_SOURCES ${SRC_DIR}/*.asm)

# Custom glob for assembly files in src/MBR
file(GLOB_RECURSE ASSEMBLY_MBR_SOURCES ${SRC_DIR}/MBR/*.asm)

# Compile assembly files to object files
add_custom_target(assembly_lib
    COMMAND mkdir -p ${BUILD_DIR}/boot_sector
    COMMENT "Assembling.."
)

foreach(ASSEMBLY_FILE IN LISTS ASSEMBLY_SOURCES)
    get_filename_component(FILENAME ${ASSEMBLY_FILE} NAME_WE)
    add_custom_command(TARGET assembly_lib
        COMMAND nasm -f elf32 -g -o ${BUILD_DIR}/${FILENAME}.o ${ASSEMBLY_FILE}
        COMMENT "Assembling: ${FILENAME}.asm"
    )
endforeach()

foreach(ASSEMBLY_MBR_FILE IN LISTS ASSEMBLY_MBR_SOURCES)
    get_filename_component(FILENAME ${ASSEMBLY_MBR_FILE} NAME_WE)
    add_custom_command(TARGET assembly_lib
        COMMAND nasm -f elf32 -g -o ${BUILD_DIR}/boot_sector/${FILENAME}.o ${ASSEMBLY_MBR_FILE}
        COMMENT "Assembling MBR: ${FILENAME}.asm"
    )
endforeach()

# Compiling
add_custom_target(compile
    COMMAND gcc -c -g -Os -m16 -march=i386 -ffreestanding -fomit-frame-pointer -fno-exceptions -fno-asynchronous-unwind-tables -fno-unwind-tables -Wall -Werror ${SRC_DIR}/main.c -o ${BUILD_DIR}/main.o
    COMMAND ld.bfd -m elf_i386 -T ${SRC_DIR}/scripts/linker.ld -nostdlib --nmagic -o ${BUILD_DIR}/tail_bootloader.elf ${BUILD_DIR}/boot_sector/*.o ${BUILD_DIR}/*.o
    DEPENDS assembly_lib
    COMMAND objcopy -O binary ${BUILD_DIR}/tail_bootloader.elf ${BUILD_DIR}/tail_bootloader.bin
    COMMAND dd if=${BUILD_DIR}/tail_bootloader.bin of=${BUILD_DIR}/tail_bootloader.img
    COMMENT "Compiling TAIL.."
)

# Run TAIL bootloader in QEMU with showed interrupts.
add_custom_target(run_int
    COMMAND qemu-system-i386 -d int -boot c ${BUILD_DIR}/tail_bootloader.img
    DEPENDS compile
    COMMENT "Running QEMU.."
)

# Run TAIL bootloader in QEMU in debug mode with GDB debugger.
add_custom_target(run
    COMMAND qemu-system-i386 -m 4096 -drive format=raw,file=${BUILD_DIR}/tail_bootloader.img -s -S -no-reboot -no-shutdown &
    COMMAND echo "Waiting for QEMU to start..."
    COMMAND sleep 2
    COMMAND gdb -ex "target remote :1234" -ex symbol-file ${BUILD_DIR}/tail_bootloader.elf -ex "layout asm"
    DEPENDS compile
    COMMENT "Running QEMU.."
)

add_custom_target(clear
    COMMAND rm -rf ${BUILD_DIR}
    COMMENT "Clearing out the files."
)
