cmake_minimum_required(VERSION 3.15)
project(XNFSMusicPlayer)

# Force C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create the mod file (Shared Library / DLL)
add_library(XNFSMusicPlayer SHARED dllmain.cpp) # Add all your .cpp files here

# Tell the compiler exactly where the BASS headers are
target_include_directories(XNFSMusicPlayer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/header/BASS/c")