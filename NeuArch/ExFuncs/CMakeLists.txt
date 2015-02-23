cmake_minimum_required(VERSION 2.8)	# Проверка версии CMake.
					# Если версия установленой программы
					# старее указаной, произайдёт аварийный выход.

project(ExFuncs)			# Название проекта

include_directories(..)			# Расположение заголовочных файлов
include_directories(../../Matrix.041)	# Расположение заголовочных файлов

set(LIB_DEST_DIR lib/exfuncs)

if(WIN32)
  set(LINK_WITH_LIBS pnna matrix)
else(UNIX)
  set(LINK_WITH_LIBS pnna matrix dl)
endif(WIN32)

set(EXFUNC_LIST
    # Just for test/debug purpose
    dummy
    # Functions
    cstr
    deadzone
    luft
    polynomial
    saturat
    sine
    watertank
    # Generators (no input)
    randmeander
    randunified)

add_library(cstr SHARED cstr.cpp)
add_library(deadzone SHARED deadzone.cpp)
add_library(dummy SHARED dummy.cpp)
add_library(luft SHARED luft.cpp)
add_library(polynomial SHARED polynomial.cpp)
add_library(randmeander SHARED randmeander.cpp)
add_library(randunified SHARED randunified.cpp)
add_library(saturat SHARED saturat.cpp)
add_library(sine SHARED sine.cpp)
add_library(watertank SHARED watertank.cpp)

# Build without prefix lib
set_target_properties(${EXFUNC_LIST} PROPERTIES PREFIX "")

# Build with given library dependencies
target_link_libraries(${EXFUNC_LIST} ${LINK_WITH_LIBS})

# Install rule
install(TARGETS ${EXFUNC_LIST} LIBRARY DESTINATION ${LIB_DEST_DIR} RUNTIME DESTINATION ${LIB_DEST_DIR})
