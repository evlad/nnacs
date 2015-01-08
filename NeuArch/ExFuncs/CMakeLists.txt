cmake_minimum_required(VERSION 2.8)	# Проверка версии CMake.
					# Если версия установленой программы
					# старее указаной, произайдёт аварийный выход.

project(NeuArch)			# Название проекта

include_directories(..)			# Расположение заголовочных файлов
include_directories(../../Matrix.041)	# Расположение заголовочных файлов

set(CSTR_SRCS cstr.cpp)
add_library(cstr SHARED ${CSTR_SRCS})
install(TARGETS cstr LIBRARY DESTINATION bin)

set(DEADZONE_SRCS deadzone.cpp)
add_library(deadzone SHARED ${DEADZONE_SRCS})
install(TARGETS deadzone LIBRARY DESTINATION bin)

set(DUMMY_SRCS dummy.cpp)
add_library(dummy SHARED ${DUMMY_SRCS})
install(TARGETS dummy LIBRARY DESTINATION bin)

set(LUFT_SRCS luft.cpp)
add_library(luft SHARED ${LUFT_SRCS})
install(TARGETS luft LIBRARY DESTINATION bin)

set(POLYNOMIAL_SRCS polynomial.cpp)
add_library(polynomial SHARED ${POLYNOMIAL_SRCS})
install(TARGETS polynomial LIBRARY DESTINATION bin)

set(RANDMEANDER_SRCS randmeander.cpp)
add_library(randmeander SHARED ${RANDMEANDER_SRCS})
install(TARGETS randmeander LIBRARY DESTINATION bin)

set(RANDUNIFIED_SRCS randunified.cpp)
add_library(randunified SHARED ${RANDUNIFIED_SRCS})
install(TARGETS randunified LIBRARY DESTINATION bin)

set(SATURAT_SRCS saturat.cpp)
add_library(saturat SHARED ${SATURAT_SRCS})
install(TARGETS saturat LIBRARY DESTINATION bin)

set(SINE_SRCS sine.cpp)
add_library(sine SHARED ${SINE_SRCS})
install(TARGETS sine LIBRARY DESTINATION bin)

set(WATERTANK_SRCS watertank.cpp)
add_library(watertank SHARED ${WATERTANK_SRCS})
install(TARGETS watertank LIBRARY DESTINATION bin)
