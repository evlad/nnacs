cmake_minimum_required(VERSION 2.8)

project(dcsloop)

# Расположение заголовочных файлов
include_directories(.)
include_directories(../Matrix.041)
include_directories(../NeuArch)

if(WIN32)
set(LIST_LIBS pnna matrix)
else(UNIX)
set(LIST_LIBS pnna matrix dl)
endif(WIN32)

set(SOURCE_EXE dcsloop.cpp NaCSM.cpp)

add_executable(dcsloop ${SOURCE_EXE})
target_link_libraries(dcsloop ${LIST_LIBS})

install(TARGETS dcsloop RUNTIME DESTINATION bin)
