cmake_minimum_required(VERSION 3.12)
project(Semestralna_praca C)

set(CMAKE_C_STANDARD 99)

add_executable(Semestralna_praca main.c http.c http.h defines.h json.c json.h)
