cmake_minimum_required(VERSION 2.8)

project(dcontrp)

# Расположение заголовочных файлов
include_directories(.)
include_directories(../Matrix.041)
include_directories(../NeuArch)

if(WIN32)
set(LIST_LIBS pnna matrix)
else(UNIX)
set(LIST_LIBS pnna matrix dl)
endif(WIN32)

set(SOURCE_EXE dcontrp.cpp NaNNCPL.cpp)

add_executable(dcontrp ${SOURCE_EXE})
target_link_libraries(dcontrp ${LIST_LIBS})

install(TARGETS dcontrp RUNTIME DESTINATION bin)
