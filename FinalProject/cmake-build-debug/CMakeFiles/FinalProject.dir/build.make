# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /snap/clion/67/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/67/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/austinmm/Desktop/WSU_CptS_360/FinalProject

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/austinmm/Desktop/WSU_CptS_360/FinalProject/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/FinalProject.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/FinalProject.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/FinalProject.dir/flags.make

CMakeFiles/FinalProject.dir/Code/main.c.o: CMakeFiles/FinalProject.dir/flags.make
CMakeFiles/FinalProject.dir/Code/main.c.o: ../Code/main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/austinmm/Desktop/WSU_CptS_360/FinalProject/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/FinalProject.dir/Code/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/FinalProject.dir/Code/main.c.o   -c /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/main.c

CMakeFiles/FinalProject.dir/Code/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/FinalProject.dir/Code/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/main.c > CMakeFiles/FinalProject.dir/Code/main.c.i

CMakeFiles/FinalProject.dir/Code/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/FinalProject.dir/Code/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/main.c -o CMakeFiles/FinalProject.dir/Code/main.c.s

CMakeFiles/FinalProject.dir/Code/Level1/ls_command.c.o: CMakeFiles/FinalProject.dir/flags.make
CMakeFiles/FinalProject.dir/Code/Level1/ls_command.c.o: ../Code/Level1/ls_command.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/austinmm/Desktop/WSU_CptS_360/FinalProject/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/FinalProject.dir/Code/Level1/ls_command.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/FinalProject.dir/Code/Level1/ls_command.c.o   -c /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/Level1/ls_command.c

CMakeFiles/FinalProject.dir/Code/Level1/ls_command.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/FinalProject.dir/Code/Level1/ls_command.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/Level1/ls_command.c > CMakeFiles/FinalProject.dir/Code/Level1/ls_command.c.i

CMakeFiles/FinalProject.dir/Code/Level1/ls_command.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/FinalProject.dir/Code/Level1/ls_command.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/Level1/ls_command.c -o CMakeFiles/FinalProject.dir/Code/Level1/ls_command.c.s

CMakeFiles/FinalProject.dir/Code/Level1/cd_command.c.o: CMakeFiles/FinalProject.dir/flags.make
CMakeFiles/FinalProject.dir/Code/Level1/cd_command.c.o: ../Code/Level1/cd_command.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/austinmm/Desktop/WSU_CptS_360/FinalProject/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/FinalProject.dir/Code/Level1/cd_command.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/FinalProject.dir/Code/Level1/cd_command.c.o   -c /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/Level1/cd_command.c

CMakeFiles/FinalProject.dir/Code/Level1/cd_command.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/FinalProject.dir/Code/Level1/cd_command.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/Level1/cd_command.c > CMakeFiles/FinalProject.dir/Code/Level1/cd_command.c.i

CMakeFiles/FinalProject.dir/Code/Level1/cd_command.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/FinalProject.dir/Code/Level1/cd_command.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/Level1/cd_command.c -o CMakeFiles/FinalProject.dir/Code/Level1/cd_command.c.s

CMakeFiles/FinalProject.dir/Code/Level1/pwd_command.c.o: CMakeFiles/FinalProject.dir/flags.make
CMakeFiles/FinalProject.dir/Code/Level1/pwd_command.c.o: ../Code/Level1/pwd_command.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/austinmm/Desktop/WSU_CptS_360/FinalProject/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/FinalProject.dir/Code/Level1/pwd_command.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/FinalProject.dir/Code/Level1/pwd_command.c.o   -c /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/Level1/pwd_command.c

CMakeFiles/FinalProject.dir/Code/Level1/pwd_command.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/FinalProject.dir/Code/Level1/pwd_command.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/Level1/pwd_command.c > CMakeFiles/FinalProject.dir/Code/Level1/pwd_command.c.i

CMakeFiles/FinalProject.dir/Code/Level1/pwd_command.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/FinalProject.dir/Code/Level1/pwd_command.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/Level1/pwd_command.c -o CMakeFiles/FinalProject.dir/Code/Level1/pwd_command.c.s

CMakeFiles/FinalProject.dir/Code/utility_functions.c.o: CMakeFiles/FinalProject.dir/flags.make
CMakeFiles/FinalProject.dir/Code/utility_functions.c.o: ../Code/utility_functions.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/austinmm/Desktop/WSU_CptS_360/FinalProject/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/FinalProject.dir/Code/utility_functions.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/FinalProject.dir/Code/utility_functions.c.o   -c /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/utility_functions.c

CMakeFiles/FinalProject.dir/Code/utility_functions.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/FinalProject.dir/Code/utility_functions.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/utility_functions.c > CMakeFiles/FinalProject.dir/Code/utility_functions.c.i

CMakeFiles/FinalProject.dir/Code/utility_functions.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/FinalProject.dir/Code/utility_functions.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/utility_functions.c -o CMakeFiles/FinalProject.dir/Code/utility_functions.c.s

CMakeFiles/FinalProject.dir/Code/Level1/link_command.c.o: CMakeFiles/FinalProject.dir/flags.make
CMakeFiles/FinalProject.dir/Code/Level1/link_command.c.o: ../Code/Level1/link_command.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/austinmm/Desktop/WSU_CptS_360/FinalProject/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/FinalProject.dir/Code/Level1/link_command.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/FinalProject.dir/Code/Level1/link_command.c.o   -c /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/Level1/link_command.c

CMakeFiles/FinalProject.dir/Code/Level1/link_command.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/FinalProject.dir/Code/Level1/link_command.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/Level1/link_command.c > CMakeFiles/FinalProject.dir/Code/Level1/link_command.c.i

CMakeFiles/FinalProject.dir/Code/Level1/link_command.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/FinalProject.dir/Code/Level1/link_command.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/Level1/link_command.c -o CMakeFiles/FinalProject.dir/Code/Level1/link_command.c.s

CMakeFiles/FinalProject.dir/Code/Level1/creat_command.c.o: CMakeFiles/FinalProject.dir/flags.make
CMakeFiles/FinalProject.dir/Code/Level1/creat_command.c.o: ../Code/Level1/creat_command.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/austinmm/Desktop/WSU_CptS_360/FinalProject/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/FinalProject.dir/Code/Level1/creat_command.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/FinalProject.dir/Code/Level1/creat_command.c.o   -c /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/Level1/creat_command.c

CMakeFiles/FinalProject.dir/Code/Level1/creat_command.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/FinalProject.dir/Code/Level1/creat_command.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/Level1/creat_command.c > CMakeFiles/FinalProject.dir/Code/Level1/creat_command.c.i

CMakeFiles/FinalProject.dir/Code/Level1/creat_command.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/FinalProject.dir/Code/Level1/creat_command.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/Level1/creat_command.c -o CMakeFiles/FinalProject.dir/Code/Level1/creat_command.c.s

CMakeFiles/FinalProject.dir/Code/Level1/mkdir_command.c.o: CMakeFiles/FinalProject.dir/flags.make
CMakeFiles/FinalProject.dir/Code/Level1/mkdir_command.c.o: ../Code/Level1/mkdir_command.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/austinmm/Desktop/WSU_CptS_360/FinalProject/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/FinalProject.dir/Code/Level1/mkdir_command.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/FinalProject.dir/Code/Level1/mkdir_command.c.o   -c /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/Level1/mkdir_command.c

CMakeFiles/FinalProject.dir/Code/Level1/mkdir_command.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/FinalProject.dir/Code/Level1/mkdir_command.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/Level1/mkdir_command.c > CMakeFiles/FinalProject.dir/Code/Level1/mkdir_command.c.i

CMakeFiles/FinalProject.dir/Code/Level1/mkdir_command.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/FinalProject.dir/Code/Level1/mkdir_command.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/austinmm/Desktop/WSU_CptS_360/FinalProject/Code/Level1/mkdir_command.c -o CMakeFiles/FinalProject.dir/Code/Level1/mkdir_command.c.s

# Object files for target FinalProject
FinalProject_OBJECTS = \
"CMakeFiles/FinalProject.dir/Code/main.c.o" \
"CMakeFiles/FinalProject.dir/Code/Level1/ls_command.c.o" \
"CMakeFiles/FinalProject.dir/Code/Level1/cd_command.c.o" \
"CMakeFiles/FinalProject.dir/Code/Level1/pwd_command.c.o" \
"CMakeFiles/FinalProject.dir/Code/utility_functions.c.o" \
"CMakeFiles/FinalProject.dir/Code/Level1/link_command.c.o" \
"CMakeFiles/FinalProject.dir/Code/Level1/creat_command.c.o" \
"CMakeFiles/FinalProject.dir/Code/Level1/mkdir_command.c.o"

# External object files for target FinalProject
FinalProject_EXTERNAL_OBJECTS =

FinalProject: CMakeFiles/FinalProject.dir/Code/main.c.o
FinalProject: CMakeFiles/FinalProject.dir/Code/Level1/ls_command.c.o
FinalProject: CMakeFiles/FinalProject.dir/Code/Level1/cd_command.c.o
FinalProject: CMakeFiles/FinalProject.dir/Code/Level1/pwd_command.c.o
FinalProject: CMakeFiles/FinalProject.dir/Code/utility_functions.c.o
FinalProject: CMakeFiles/FinalProject.dir/Code/Level1/link_command.c.o
FinalProject: CMakeFiles/FinalProject.dir/Code/Level1/creat_command.c.o
FinalProject: CMakeFiles/FinalProject.dir/Code/Level1/mkdir_command.c.o
FinalProject: CMakeFiles/FinalProject.dir/build.make
FinalProject: CMakeFiles/FinalProject.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/austinmm/Desktop/WSU_CptS_360/FinalProject/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Linking C executable FinalProject"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/FinalProject.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/FinalProject.dir/build: FinalProject

.PHONY : CMakeFiles/FinalProject.dir/build

CMakeFiles/FinalProject.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/FinalProject.dir/cmake_clean.cmake
.PHONY : CMakeFiles/FinalProject.dir/clean

CMakeFiles/FinalProject.dir/depend:
	cd /home/austinmm/Desktop/WSU_CptS_360/FinalProject/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/austinmm/Desktop/WSU_CptS_360/FinalProject /home/austinmm/Desktop/WSU_CptS_360/FinalProject /home/austinmm/Desktop/WSU_CptS_360/FinalProject/cmake-build-debug /home/austinmm/Desktop/WSU_CptS_360/FinalProject/cmake-build-debug /home/austinmm/Desktop/WSU_CptS_360/FinalProject/cmake-build-debug/CMakeFiles/FinalProject.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/FinalProject.dir/depend

