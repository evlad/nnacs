cmake_minimum_required(VERSION 2.8)	 # Проверка версии CMake.
                                     # Если версия установленой программы
                                     # старее указаной, произайдёт аварийный выход.

project(matrix)			# Название проекта

set(SOURCE_LIB matcreat.c
	matdurbn.c
	matinv.c
	matsolve.c
	mattoepz.c
	matadd.c
	matdet.c
	matdump.c
	materr.c
	matmul.c
	matsub.c
	matsubx.c
	mattran.c)			# Установка переменной со списком исходников для библиотеки

add_library(matrix STATIC ${SOURCE_LIB})	# Создание статической библиотеки с именем foo
#add_library(matrix SHARED ${SOURCE_LIB})	# Создание динамической библиотеки с именем foo
