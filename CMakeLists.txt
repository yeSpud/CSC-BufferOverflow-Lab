cmake_minimum_required(VERSION 3.23)
project(hash C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-fno-stack-protector -z execstack")
add_executable(hash main.c)
target_link_libraries(hash crypt)


enable_language(ASM_NASM)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
    <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_ASM_NASM_FLAGS "${ASM_NASM_FLAGS} -g -f elf64")
else()
    set(CMAKE_ASM_NASM_FLAGS "${ASM_NASM_FLAGS}")
endif()
add_executable(shellcode shell.asm)
