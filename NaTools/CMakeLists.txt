cmake_minimum_required(VERSION 2.8)

project(NaTools)

# Расположение заголовочных файлов
include_directories(.)
include_directories(../Matrix.041)
include_directories(../NeuArch)

set(LIST_LIBS pnna matrix)

set(SOURCE_StatAn StatAn.cpp)
add_executable(StatAn ${SOURCE_StatAn})
target_link_libraries(StatAn ${LIST_LIBS})

set(SOURCE_Distr2D Distr2D.cpp)
add_executable(Distr2D ${SOURCE_Distr2D})
target_link_libraries(Distr2D ${LIST_LIBS})

set(SOURCE_Distr1D Distr1D.cpp)
add_executable(Distr1D ${SOURCE_Distr1D})
target_link_libraries(Distr1D ${LIST_LIBS})

set(SOURCE_MakeNN MakeNN.cpp)
add_executable(MakeNN ${SOURCE_MakeNN})
target_link_libraries(MakeNN ${LIST_LIBS})

set(SOURCE_ResetNN ResetNN.cpp)
add_executable(ResetNN ${SOURCE_ResetNN})
target_link_libraries(ResetNN ${LIST_LIBS})

set(SOURCE_EvalNN EvalNN.cpp)
add_executable(EvalNN ${SOURCE_EvalNN})
target_link_libraries(EvalNN ${LIST_LIBS})

set(SOURCE_TrainNN TrainNN.cpp)
add_executable(TrainNN ${SOURCE_TrainNN})
target_link_libraries(TrainNN ${LIST_LIBS})

set(SOURCE_UNGen UNGen.cpp)
add_executable(UNGen ${SOURCE_UNGen})
target_link_libraries(UNGen ${LIST_LIBS})

set(SOURCE_FileCvt FileCvt.cpp)
add_executable(FileCvt ${SOURCE_FileCvt})
target_link_libraries(FileCvt ${LIST_LIBS})

#set(SOURCE_% %.cpp)
#add_executable(% ${SOURCE_%})
#target_link_libraries(% ${LIST_LIBS})

install(TARGETS StatAn Distr2D Distr1D MakeNN ResetNN EvalNN TrainNN UNGen FileCvt RUNTIME DESTINATION bin)
