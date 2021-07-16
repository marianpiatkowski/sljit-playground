cmake_minimum_required(VERSION 3.9)

project(sljit-playground C CXX)

set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package(SLJIT REQUIRED)
include(AddSLJITFlags)

execute_process(COMMAND ${CMAKE_COMMAND} "-E" "create_symlink" "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/src_dir")

set(MOVE_FILES hello_sljit.cc func_call.cc branch.cc fibonacci.cc ackermann.cc)
foreach(f ${MOVE_FILES})
  execute_process(COMMAND ${CMAKE_COMMAND} "-E" "create_symlink" "${CMAKE_CURRENT_SOURCE_DIR}/${f}" "${CMAKE_CURRENT_BINARY_DIR}/${f}")
endforeach()

add_executable(hello_sljit hello_sljit.cc)
add_sljit_flags(hello_sljit)

add_executable(func_call func_call.cc)
add_sljit_flags(func_call)

add_executable(branch branch.cc)
add_sljit_flags(branch)

add_executable(fibonacci fibonacci.cc)
add_sljit_flags(fibonacci)

add_executable(ackermann ackermann.cc)
add_sljit_flags(ackermann)
