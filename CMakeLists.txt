cmake_minimum_required (VERSION 3.5)
  
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_EXE_LINKER_FLAGS "-fsanitize=address,undefined")
add_executable(ChannelsTest ChannelsCPP/Source.cpp ChannelsCPP/Channel.cpp)
target_link_libraries(ChannelsTest PUBLIC pthread)
target_compile_options(ChannelsTest PRIVATE "-fsanitize=address,undefined")
