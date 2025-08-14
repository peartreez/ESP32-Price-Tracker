# The following lines of boilerplate have to be in your project's
# CMakeLists.txt in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# Include the ESP-IDF build system
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Set the project name
project(bitcoin_price_fetcher) 