cmake_minimum_required(VERSION 2.8)

project(dplantid)

# Расположение заголовочных файлов
include_directories(.)
include_directories(../Matrix.041)
include_directories(../NeuArch)

if(WIN32)
set(LIST_LIBS pnna matrix)
else(UNIX)
set(LIST_LIBS pnna matrix dl)
endif(WIN32)

set(SOURCE_dplantid dplantid.cpp NaNNRPL.cpp)

add_executable(dplantid ${SOURCE_dplantid})
target_link_libraries(dplantid ${LIST_LIBS})

set(SOURCE_dnnplant dnnplant.cpp NaNNRPL.cpp)

add_executable(dnnplant ${SOURCE_dnnplant})
target_link_libraries(dnnplant ${LIST_LIBS})

install(TARGETS dplantid dnnplant RUNTIME DESTINATION bin)
