cmake_minimum_required(VERSION 2.8)

project(dcontrf)

# Расположение заголовочных файлов
include_directories(.)
include_directories(../Matrix.041)
include_directories(../NeuArch)

if(WIN32)
set(LIST_LIBS pnna matrix)
else(UNIX)
set(LIST_LIBS pnna matrix dl)
endif(WIN32)

set(SOURCE_EXE dcontrf.cpp NaNNOCL.cpp)

add_executable(dcontrf ${SOURCE_EXE})
target_link_libraries(dcontrf ${LIST_LIBS})

install(TARGETS dcontrf RUNTIME DESTINATION bin)
