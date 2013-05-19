cmake_minimum_required(VERSION 2.8)

project(dtools)

# Расположение заголовочных файлов
include_directories(.)
include_directories(../Matrix.041)
include_directories(../NeuArch)

if(WIN32)
set(LIST_LIBS pnna matrix)
else(UNIX)
set(LIST_LIBS pnna matrix dl)
endif(WIN32)

set(SOURCE_dtf dtf.cpp)
add_executable(dtf ${SOURCE_dtf})
target_link_libraries(dtf ${LIST_LIBS})

set(SOURCE_d2ts d2ts.cpp)
add_executable(d2ts ${SOURCE_d2ts})
target_link_libraries(d2ts ${LIST_LIBS})

set(SOURCE_drand drand.cpp)
add_executable(drand ${SOURCE_drand})
target_link_libraries(drand ${LIST_LIBS})

set(SOURCE_dwtest dwtest.cpp)
add_executable(dwtest ${SOURCE_dwtest})
target_link_libraries(dwtest ${LIST_LIBS})

set(SOURCE_dwtest2 dwtest2.cpp)
add_executable(dwtest2 ${SOURCE_dwtest2})
target_link_libraries(dwtest2 ${LIST_LIBS})

set(SOURCE_dmse dmse.cpp)
add_executable(dmse ${SOURCE_dmse})
target_link_libraries(dmse ${LIST_LIBS})

set(SOURCE_dcorr dcorr.cpp)
add_executable(dcorr ${SOURCE_dcorr})
target_link_libraries(dcorr ${LIST_LIBS})

set(SOURCE_dsum dsum.cpp)
add_executable(dsum ${SOURCE_dsum})
target_link_libraries(dsum ${LIST_LIBS})

set(SOURCE_dsub dsub.cpp)
add_executable(dsub ${SOURCE_dsub})
target_link_libraries(dsub ${LIST_LIBS})

set(SOURCE_dstat dstat.cpp)
add_executable(dstat ${SOURCE_dstat})
target_link_libraries(dstat ${LIST_LIBS})

set(SOURCE_dstatsb dstatsb.cpp)
add_executable(dstatsb ${SOURCE_dstatsb})
target_link_libraries(dstatsb ${LIST_LIBS})

set(SOURCE_djacob djacob.cpp)
add_executable(djacob ${SOURCE_djacob})
target_link_libraries(djacob ${LIST_LIBS})

set(SOURCE_dsin dsin.cpp)
add_executable(dsin ${SOURCE_dsin})
target_link_libraries(dsin ${LIST_LIBS})

set(SOURCE_dmean dmean.cpp)
add_executable(dmean ${SOURCE_dmean})
target_link_libraries(dmean ${LIST_LIBS})

set(SOURCE_dstddev dstddev.cpp)
add_executable(dstddev ${SOURCE_dstddev})
target_link_libraries(dstddev ${LIST_LIBS})

set(SOURCE_dconst dconst.cpp)
add_executable(dconst ${SOURCE_dconst})
target_link_libraries(dconst ${LIST_LIBS})

set(SOURCE_dsmooth dsmooth.cpp)
add_executable(dsmooth ${SOURCE_dsmooth})
target_link_libraries(dsmooth ${LIST_LIBS})

set(SOURCE_dmult dmult.cpp)
add_executable(dmult ${SOURCE_dmult})
target_link_libraries(dmult ${LIST_LIBS})

set(SOURCE_drandmea drandmea.cpp)
add_executable(drandmea ${SOURCE_drandmea})
target_link_libraries(drandmea ${LIST_LIBS})

set(SOURCE_dranduni dranduni.cpp)
add_executable(dranduni ${SOURCE_dranduni})
target_link_libraries(dranduni ${LIST_LIBS})

set(SOURCE_dsteps dsteps.cpp)
add_executable(dsteps ${SOURCE_dsteps})
target_link_libraries(dsteps ${LIST_LIBS})

set(SOURCE_dmeander dmeander.cpp)
add_executable(dmeander ${SOURCE_dmeander})
target_link_libraries(dmeander ${LIST_LIBS})

#set(SOURCE_% .cpp)
#add_executable(% ${SOURCE_%})
#target_link_libraries(% ${LIST_LIBS})

install(TARGETS dmeander dsteps dranduni drandmea dmult
        dsmooth dconst dstddev dmean dsin djacob dstatsb dstat dcorr
	dsum dsub dmse dwtest dwtest2 dtf d2ts drand RUNTIME DESTINATION bin)
