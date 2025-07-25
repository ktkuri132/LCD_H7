cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME LCD_H7)


# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)
#add_subdirectory(Drivers/lvgl)
# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

file(GLOB_RECURSE lvgl_Source
        Drivers/lvgl/*.c
        Drivers/lvgl/src/core/*.c
        Drivers/lvgl/src/display/*.c
        Drivers/lvgl/src/draw/*.c
        Drivers/lvgl/src/indev/*.c
        Drivers/lvgl/src/widgets/*.c
        Drivers/lvgl/src/font/*.c
        Drivers/lvgl/src/layouts/*.c
        Drivers/lvgl/src/misc/*.c
        Drivers/lvgl/src/osal/*.c
        Drivers/lvgl/src/stdlib/*.c
        Drivers/lvgl/src/tick/*.c
        Drivers/lvgl/src/others/*.c
        Drivers/lvgl/src/themes/*.c
        Drivers/lvgl/src/libs/*.c
        Drivers/lvgl/src/drivers/*.c
        Drivers/lvgl/examples/porting/*.c

        Drivers/lvgl/guider/custom/*.c
        Drivers/lvgl/guider/generated/*.c
)

#target_sources(${CMAKE_PROJECT_NAME} PRIVATE lvgl_Source)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
        Src/hw/LCD/st7789.c
        Src/hw/LCD/lcd_fonts.c
#        Src/mw/display/display.c
#        Src/mw/display/display_extlib.c
        ${lvgl_Source}
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    # Add user defined include paths
    ${CMAKE_SOURCE_DIR}/Drivers/lvgl
    ${CMAKE_SOURCE_DIR}/Drivers/lvgl/src
    ${CMAKE_SOURCE_DIR}/Drivers/lvgl/examples/porting

    ${CMAKE_SOURCE_DIR}/Drivers/lvgl/guider/custom
    ${CMAKE_SOURCE_DIR}/Drivers/lvgl/guider/generated

)

add_link_options(--diag_suppress=188,546,68,111)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
