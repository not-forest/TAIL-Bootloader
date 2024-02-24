cmake_minimum_required(VERSION 3.15)
project(tail_bootloader C ASM)

# This defines if the output binary is for QEMU simulation or release.
option(RELEASE "Compiles the release binary" OFF)

# Project paths
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)

# Custom glob for c files
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${SRC_DIR}/*.c)

# Custom glob for assembly files
file(GLOB_RECURSE ASSEMBLY_SOURCES CONFIGURE_DEPENDS ${SRC_DIR}/*.asm)

# Compile assembly files to object files
add_custom_target(assembly_lib
    COMMAND mkdir -p build
    COMMENT "Assembling.."
)

add_custom_target(compile
    COMMAND mkdir -p build
    COMMENT "Compiling TAIL.."
)

# Compiling
add_custom_target(compile_bin
    COMMAND cp ${SRC_DIR}/scripts/*.ld .
    COMMAND ld.bfd -m elf_i386 -T linker.ld -nostdlib --nmagic -o tail_bootloader.elf
    COMMAND objcopy -O binary tail_bootloader.elf tail_bootloader.bin
    COMMAND dd if=/dev/zero of=tail_bootloader.img bs=1024 count=2
    COMMAND dd if=tail_bootloader.bin of=tail_bootloader.img bs=512 conv=notrunc
    WORKING_DIRECTORY cd ${BUILD_DIR}
    DEPENDS compile
    DEPENDS assembly_lib
)

# Run TAIL bootloader in QEMU with showed interrupts.
add_custom_target(run_int
    COMMAND qemu-system-i386 -d int -boot c ${BUILD_DIR}/tail_bootloader.img --no-reboot --no-shutdown
    DEPENDS compile_bin
    COMMENT "Running QEMU.."
)

# Run TAIL bootloader in QEMU in debug mode with GDB debugger.
add_custom_target(run
    COMMAND qemu-system-i386 -m 4096 -drive format=raw,file=${BUILD_DIR}/tail_bootloader.img -s -S -no-reboot -no-shutdown &
    COMMAND echo "Waiting for QEMU to start..."
    COMMAND sleep 2
    COMMAND gdb -ex "target remote :1234" -ex symbol-file ${BUILD_DIR}/tail_bootloader.elf -ex "layout asm"
    DEPENDS compile_bin
    COMMENT "Running QEMU.."
)

add_custom_target(clear
    COMMAND rm -rf ${BUILD_DIR}
    COMMENT "Clearing out the files."
)

foreach(ASSEMBLY_FILE IN LISTS ASSEMBLY_SOURCES)
    get_filename_component(FILENAME ${ASSEMBLY_FILE} NAME_WE)
    add_custom_command(TARGET assembly_lib
        COMMAND nasm -f elf32 -g -o ${BUILD_DIR}/${FILENAME}.o ${ASSEMBLY_FILE}
        COMMENT "Assembling: ${FILENAME}.asm"
        WORKING_DIRECTORY ${SRC_DIR}
    )
endforeach()

foreach(C_FILE IN LISTS SRC_FILES)
    get_filename_component(FILENAME ${C_FILE} NAME_WE)
    add_custom_command(TARGET compile
        COMMAND gcc -RELEASE -mgeneral-regs-only -c -g -Os -m32 -D__KERNEL__ -march=i386 -ffreestanding -fomit-frame-pointer -fno-exceptions -fno-pic -fno-asynchronous-unwind-tables -fno-unwind-tables -Wall -Werror ${C_FILE} -o ${BUILD_DIR}/${FILENAME}.o 
        COMMENT "Compiling: ${FILENAME}"
        WORKING_DIRECTORY ${SRC_DIR}
    )
endforeach()
