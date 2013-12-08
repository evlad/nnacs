cmake_minimum_required(VERSION 2.8)

project(nninvf)

# Расположение заголовочных файлов
include_directories(.)
include_directories(../Matrix.041)
include_directories(../NeuArch)

set(LIST_LIBS pnna matrix)

set(SOURCE_nninvf nninvf.cpp)
add_executable(nninvf ${SOURCE_nninvf})
target_link_libraries(nninvf ${LIST_LIBS})

#set(SOURCE_% %.cpp)
#add_executable(% ${SOURCE_%})
#target_link_libraries(% ${LIST_LIBS})

install(TARGETS nninvf RUNTIME DESTINATION bin)
