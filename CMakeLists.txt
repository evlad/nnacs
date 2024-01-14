cmake_minimum_required(VERSION 2.8)	 # Проверка версии CMake.
                                     # Если версия установленой программы
                                     # старее указаной, произайдёт аварийный выход.

project(NNACS)			# Название проекта

#set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_INSTALL_PREFIX _debug_)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_INSTALL_PREFIX _release_)

# mkdir -p _build_ _release_
# cd _build_
# cmake -DCMAKE_INSTALL_PREFIX=../_release_ ..
# make
# make install

add_subdirectory(Matrix.041)
add_subdirectory(NeuArch)
add_subdirectory(NaTools)
add_subdirectory(dtools)
add_subdirectory(dcsloop)
add_subdirectory(dplantid)
add_subdirectory(dcontrp)
add_subdirectory(dcontrf)
add_subdirectory(noc_labs)
add_subdirectory(test_mimo)

install(DIRECTORY noc_etalon DESTINATION .
        PATTERN ".svn" EXCLUDE)
