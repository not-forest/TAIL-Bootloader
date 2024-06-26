# TAIL-Bootloader

A simple bootloader, written in C, is currently in its early stages of development. List that outlines the planned features and milestones for the bootloader:

### 1. Two Staged Bootloader
   - [✅] Implement the first stage of the bootloader.
   - [❌] Develop the second stage to load the main bootloader functionality.

### 2. Bring the Kernel
   - [❌] Implement the code to locate and load the kernel into memory.

### 3. Go to Protected Mode
   - [✅] Transition the system into protected mode.

### 4. Provide Kernel Information
   - [❌] Develop mechanisms to provide essential information to the kernel for correct operation.

### 5. Transfer Control to Kernel
   - [❌] Establish the process of transferring control from the bootloader to the loaded kernel.

### 6. Become Multibootable
   - [❌] Implement support for multiboot functionality, allowing the bootloader to load different kernels.

## Current Project Status

Interrupts for bootloader application is set up and can perform some basic tasks. For non exceptions, only the 
PIC support is added for now. The TAIL part of the bootloader runs if primitively configured protected mode. 

## Run

1. To compile: make compile
2. Run in QEMU with gdb debugging: make run
3. Run in QEMU without gdb (interrupts are shown): make run DEBUG=0

