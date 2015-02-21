cmake_minimum_required(VERSION 2.8)	# Проверка версии CMake.
					# Если версия установленой программы
					# старее указаной, произайдёт аварийный выход.

project(NeuArch)			# Название проекта

include_directories(..)			# Расположение заголовочных файлов
include_directories(../../Matrix.041)	# Расположение заголовочных файлов

set(LIB_DEST_DIR lib/exfuncs)

if(WIN32)
set(LIST_LIBS pnna matrix)
else(UNIX)
set(LIST_LIBS pnna matrix dl)
endif(WIN32)

set(CSTR_SRCS cstr.cpp)
add_library(cstr SHARED ${CSTR_SRCS})
target_link_libraries(cstr ${LIST_LIBS})
install(TARGETS cstr LIBRARY DESTINATION ${LIB_DEST_DIR} RUNTIME DESTINATION ${LIB_DEST_DIR})

set(DEADZONE_SRCS deadzone.cpp)
add_library(deadzone SHARED ${DEADZONE_SRCS})
target_link_libraries(deadzone ${LIST_LIBS})
install(TARGETS deadzone LIBRARY DESTINATION ${LIB_DEST_DIR} RUNTIME DESTINATION ${LIB_DEST_DIR})

set(DUMMY_SRCS dummy.cpp)
add_library(dummy SHARED ${DUMMY_SRCS})
target_link_libraries(dummy ${LIST_LIBS})
install(TARGETS dummy LIBRARY DESTINATION ${LIB_DEST_DIR} RUNTIME DESTINATION ${LIB_DEST_DIR})

set(LUFT_SRCS luft.cpp)
add_library(luft SHARED ${LUFT_SRCS})
target_link_libraries(luft ${LIST_LIBS})
install(TARGETS luft LIBRARY DESTINATION ${LIB_DEST_DIR} RUNTIME DESTINATION ${LIB_DEST_DIR})

set(POLYNOMIAL_SRCS polynomial.cpp)
add_library(polynomial SHARED ${POLYNOMIAL_SRCS})
target_link_libraries(polynomial ${LIST_LIBS})
install(TARGETS polynomial LIBRARY DESTINATION ${LIB_DEST_DIR} RUNTIME DESTINATION ${LIB_DEST_DIR})

set(RANDMEANDER_SRCS randmeander.cpp)
add_library(randmeander SHARED ${RANDMEANDER_SRCS})
target_link_libraries(randmeander ${LIST_LIBS})
install(TARGETS randmeander LIBRARY DESTINATION ${LIB_DEST_DIR} RUNTIME DESTINATION ${LIB_DEST_DIR})

set(RANDUNIFIED_SRCS randunified.cpp)
add_library(randunified SHARED ${RANDUNIFIED_SRCS})
target_link_libraries(randunified ${LIST_LIBS})
install(TARGETS randunified LIBRARY DESTINATION ${LIB_DEST_DIR} RUNTIME DESTINATION ${LIB_DEST_DIR})

set(SATURAT_SRCS saturat.cpp)
add_library(saturat SHARED ${SATURAT_SRCS})
target_link_libraries(saturat ${LIST_LIBS})
install(TARGETS saturat LIBRARY DESTINATION ${LIB_DEST_DIR} RUNTIME DESTINATION ${LIB_DEST_DIR})

set(SINE_SRCS sine.cpp)
add_library(sine SHARED ${SINE_SRCS})
target_link_libraries(sine ${LIST_LIBS})
install(TARGETS sine LIBRARY DESTINATION ${LIB_DEST_DIR} RUNTIME DESTINATION ${LIB_DEST_DIR})

set(WATERTANK_SRCS watertank.cpp)
add_library(watertank SHARED ${WATERTANK_SRCS})
target_link_libraries(watertank ${LIST_LIBS})
install(TARGETS watertank LIBRARY DESTINATION ${LIB_DEST_DIR} RUNTIME DESTINATION ${LIB_DEST_DIR})
