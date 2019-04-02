cmake_minimum_required(VERSION 3.13)
project(FinalProject C)

set(CMAKE_C_STANDARD 99)

add_executable(FinalProject Code/main.c Code/Level1/ls_command.c  Code/Level1/cd_command.c  Code/Level1/pwd_command.c Code/utility_functions.c Code/Level1/link_command.c Code/Level1/creat_command.c Code/Level1/mkdir_command.c)