cmake_minimum_required(VERSION 2.8)

project(test_mimo)

# Расположение заголовочных файлов
include_directories(.)
include_directories(../Matrix.041)
include_directories(../NeuArch)

if(WIN32)
set(LIST_LIBS pnna matrix)
else(UNIX)
set(LIST_LIBS pnna matrix dl)
endif(WIN32)

set(SOURCE_EXE test_mimo.cpp)

add_executable(test_mimo ${SOURCE_EXE})
target_link_libraries(test_mimo ${LIST_LIBS})

install(TARGETS test_mimo RUNTIME DESTINATION bin)
