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
target_link_libraries(cstr ${LINK_WITH_LIBS})

add_library(deadzone SHARED deadzone.cpp)
target_link_libraries(deadzone ${LINK_WITH_LIBS})

add_library(dummy SHARED dummy.cpp)
target_link_libraries(dummy ${LINK_WITH_LIBS})

add_library(luft SHARED luft.cpp)
target_link_libraries(luft ${LINK_WITH_LIBS})

add_library(polynomial SHARED polynomial.cpp)
target_link_libraries(polynomial ${LINK_WITH_LIBS})

add_library(randmeander SHARED randmeander.cpp)
target_link_libraries(randmeander ${LINK_WITH_LIBS})

add_library(randunified SHARED randunified.cpp)
target_link_libraries(randunified ${LINK_WITH_LIBS})

add_library(saturat SHARED saturat.cpp)
target_link_libraries(saturat ${LINK_WITH_LIBS})

add_library(sine SHARED sine.cpp)
target_link_libraries(sine ${LINK_WITH_LIBS})

add_library(watertank SHARED watertank.cpp)
target_link_libraries(watertank ${LINK_WITH_LIBS})

# Build without prefix lib
set_target_properties(${EXFUNC_LIST} PROPERTIES PREFIX "")

# Install rule
install(TARGETS ${EXFUNC_LIST} LIBRARY DESTINATION ${LIB_DEST_DIR} RUNTIME DESTINATION ${LIB_DEST_DIR})
