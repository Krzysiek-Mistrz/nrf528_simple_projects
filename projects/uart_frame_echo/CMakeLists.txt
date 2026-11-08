cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(uart_frame_echo)

set(EMBEDKIT ${CMAKE_CURRENT_SOURCE_DIR}/../../embedkit/lib)

target_sources(app PRIVATE
    src/main.c
    ${EMBEDKIT}/bits.c
    ${EMBEDKIT}/ringbuf.c
    ${EMBEDKIT}/fsm.c
    ${EMBEDKIT}/debounce.c
    ${EMBEDKIT}/frame_parser.c
)

target_include_directories(app PRIVATE ${EMBEDKIT})