cmake_minimum_required(VERSION 3.5.0)
project(siliconray VERSION 0.1.0 LANGUAGES C)

# Add the include directory to the project
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(siliconray main.c universe.c math.c)

# Set DEBUG preprocessor definition
target_compile_definitions(siliconray PRIVATE DEBUG)

# Set STB_IMAGE_WRITE_IMPLEMENTATION & STB_IMAGE_IMPLEMENTATION preprocessor definition
target_compile_definitions(siliconray PRIVATE STB_IMAGE_WRITE_IMPLEMENTATION STB_IMAGE_IMPLEMENTATION)
target_compile_definitions(siliconray PRIVATE STB_IMAGE_IMPLEMENTATION STB_IMAGE_IMPLEMENTATION)

# Set the compile definitions for the release build
target_link_libraries(siliconray m)