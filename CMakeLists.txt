cmake_minimum_required(VERSION 2.8)	 # Проверка версии CMake.
                                     # Если версия установленой программы
                                     # старее указаной, произайдёт аварийный выход.

project(NNACS)			# Название проекта

set(CMAKE_BUILD_TYPE Release)

add_subdirectory(Matrix.041)
add_subdirectory(NeuArch)
add_subdirectory(NaTools)
add_subdirectory(dtools)
add_subdirectory(dcsloop)
add_subdirectory(dplantid)
add_subdirectory(dcontrp)
add_subdirectory(dcontrf)
add_subdirectory(noc_labs)

install(DIRECTORY noc_etalon DESTINATION .
        PATTERN ".svn" EXCLUDE)
