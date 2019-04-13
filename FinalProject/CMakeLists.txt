cmake_minimum_required(VERSION 3.13)
project(FinalProject)

set(CMAKE_CXX_STANDARD 14)

include_directories(Code)
include_directories(Code/Level1)

add_executable(FinalProject
        Code/Level1/cd_command.c
        Code/Level1/creat_command.c
        Code/Level1/Level1.h
        Code/Level1/link_command.c
        Code/Level1/ls_command.c
        Code/Level1/mkdir_command.c
        Code/Level1/pwd_command.c
        Code/Level1/readlink_command.c
        Code/Level1/rmdir_command.c
        Code/Level1/symlink_command.c
        Code/Level1/unlink_command.c
        Code/Header.h
        Code/main.c
        Code/Type.h
        Code/utility_functions.c
        disk
        mk
        ReadMe.md
        Code/Level1/stat_command.c
        Code/Level1/chmod_command.c
        Code/Level1/touch_command.c)
