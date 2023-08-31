# Project
cmake_minimum_required(VERSION 3.13)
project(CAT)

# Dependences
include($ENV{NOELLE_DIR}src/scripts/DependencesCMake.txt)

# Pass
add_subdirectory(src)
