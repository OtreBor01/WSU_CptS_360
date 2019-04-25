cmake_minimum_required(VERSION 3.13)
project(FinalProject)

set(CMAKE_CXX_STANDARD 14)

include_directories(Code)
include_directories(Code/Level1)
include_directories(Code/Level2)

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
        Code/Level1/rm_command.c
        Code/Level1/symlink_command.c
        Code/Level1/unlink_command.c
        Code/Header.h
        Code/main.c
        Code/Type.h
        Code/utility_functions.c
        mk
        ReadMe.md
        Code/Level1/stat_command.c
        Code/Level1/chmod_command.c
        Code/Level1/touch_command.c
        Code/Level2/read_command.c
        Code/Level2/mv_command.c
        Code/Level2/open_command.c
        Code/Level2/pfd.c
        Code/Level2/close_command.c
        Code/Level2/write_command.c
        Code/Level2/utility_functions.c
        Code/Level1/utility_functions.c
        Code/Level2/cp_command.c Code/Level2/cat_command.c Code/Level2/lseek_command.c Code/Level3/utility_functions.c Code/Level3/Level3.h Code/Level3/mount_command.c Code/Level3/unmount_command.c)

