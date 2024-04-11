cmake_minimum_required(VERSION 3.5)

set(TARGET_LIB mcp3914)

# Set Sources and Includes
set(MCP3914_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(MCP3914_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mcp3914.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/spi.cpp
)


# Create lib Target
add_library(${TARGET_LIB} ${MCP3914_SOURCES})

target_include_directories(${TARGET_LIB} PUBLIC ${MCP3914_INCLUDE_DIRECTORIES})
