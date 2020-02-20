project(thermaloberver)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

#find_package(jpeg REQUIRED)
#find_package(libpng12-dev REQUIRED)
#find_package(z REQUIRED) 
#find_package(graphviz REQUIRED) 
#set(CMAKE_CXX_COMPILER gcc)
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} lib/LibTIM/ . ${CMAKE_SOURCE_DIR})

#add_subdirectory(lib)
#add_subdirectory(ui)
#add_subdirectory(algo)
#add_subdirectory(test)
#add_subdirectory(learning)

AUX_SOURCE_DIRECTORY(lib/LibTIM/Common all_SRCS)
AUX_SOURCE_DIRECTORY(lib/wavelet all_SRCS)
AUX_SOURCE_DIRECTORY(lib/LibSVM all_SRCS)
AUX_SOURCE_DIRECTORY(lib/vl all_SRCS)
AUX_SOURCE_DIRECTORY(algo all_SRCS)
AUX_SOURCE_DIRECTORY(ui all_SRCS)
AUX_SOURCE_DIRECTORY(learning all_SRCS)
AUX_SOURCE_DIRECTORY(config  all_SRCS)
AUX_SOURCE_DIRECTORY(utility  all_SRCS)


FILE(GLOB all_HEADERS "ui/*.h")
FILE(GLOB all_FORMS "ui/*.ui")
SET(IMAGE_LIBS jpeg png z stdc++)

QT4_WRAP_CPP(all_HEADERS_MOC ${all_HEADERS})
QT4_WRAP_UI(all_FORMS_HEADERS ${all_FORMS})
#QT4_ADD_RESOURCES(helloworld_RESOURCES_RCC ${helloworld_RESOURCES})
SET(GV_LIBS gvc pthread cgraph)

add_executable(bptSegment  ${all_SRCS} ${all_HEADERS_MOC} ${all_FORMS_HEADERS} bptSegmentFormApp.cpp)
target_link_libraries(bptSegment ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENCV_LIBS} ${IMAGE_LIBS} ${GV_LIBS})

#add_executable(thermocmd bptSegmentFormApp.cpp ${all_SRCS} ${all_HEADERS_MOC} ${all_FORMS_HEADERS} )
#target_link_libraries(thermocmd ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENCV_LIBS} ${IMAGE_LIBS})

#add_executable(testwavelet test/testwavelet.cpp ${all_SRCS} ${all_HEADERS_MOC} ${all_FORMS_HEADERS} )
#target_link_libraries(testwavelet ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENCV_LIBS} ${IMAGE_LIBS})

add_executable(testtexture test/testtexture.cpp ${all_SRCS} ${all_HEADERS_MOC} ${all_FORMS_HEADERS} )
target_link_libraries(testtexture ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENCV_LIBS} ${IMAGE_LIBS} ${GV_LIBS})

#SET(ML_LIBS shark)
#add_executable(svmSharkTest test/svmSharkTest.cpp ${all_SRCS} ${all_HEADERS_MOC} ${all_FORMS_HEADERS} )
#target_link_libraries(svmSharkTest ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENCV_LIBS} ${IMAGE_LIBS} ${ML_LIBS})


#add_executable(dot test/treeshow.cpp ${all_SRCS} ${all_HEADERS_MOC} ${all_FORMS_HEADERS} )
#target_link_libraries(dot ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENCV_LIBS} ${IMAGE_LIBS} ${GV_LIBS})


add_executable(treeqtw test/treeqtw.cpp ${all_SRCS} ${all_HEADERS_MOC} ${all_FORMS_HEADERS} )
target_link_libraries(treeqtw ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENCV_LIBS} ${IMAGE_LIBS} ${GV_LIBS})

add_executable(desgen test/desgen.cpp ${all_SRCS} ${all_HEADERS_MOC} ${all_FORMS_HEADERS} )
target_link_libraries(desgen ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENCV_LIBS} ${IMAGE_LIBS} ${GV_LIBS})

#add_executable(treeGraphicTest test/treeGraphicTest.cpp ${all_SRCS} ${all_HEADERS_MOC} ${all_FORMS_HEADERS} )
#target_link_libraries(treeGraphicTest ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENCV_LIBS} ${IMAGE_LIBS} ${GV_LIBS})

add_executable(testTexton test/testTexton.cpp ${all_SRCS} ${all_HEADERS_MOC} ${all_FORMS_HEADERS} )
target_link_libraries(testTexton ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENCV_LIBS} ${IMAGE_LIBS} ${GV_LIBS})

add_executable(changeImageToText test/changeImageToText.cpp ${all_SRCS} ${all_HEADERS_MOC} ${all_FORMS_HEADERS} )
target_link_libraries(changeImageToText ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENCV_LIBS} ${IMAGE_LIBS} ${GV_LIBS})

add_executable(mainBPT test/mainBPT.cpp ${all_SRCS} ${all_HEADERS_MOC} ${all_FORMS_HEADERS} )
target_link_libraries(mainBPT ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENCV_LIBS} ${IMAGE_LIBS} ${GV_LIBS})

add_executable(sowerbyExtract test/SowerbyExtractGroundTruth.cpp ${all_SRCS} ${all_HEADERS_MOC} ${all_FORMS_HEADERS} )
target_link_libraries(sowerbyExtract ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENCV_LIBS} ${IMAGE_LIBS} ${GV_LIBS})

add_executable(testSVM test/testSVM.cpp ${all_SRCS} ${all_HEADERS_MOC} ${all_FORMS_HEADERS} )
target_link_libraries(testSVM ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENCV_LIBS} ${IMAGE_LIBS} ${GV_LIBS})

add_executable(testTreeReconstruction test/testTreeReconstruction.cpp ${all_SRCS} ${all_HEADERS_MOC} ${all_FORMS_HEADERS} )
target_link_libraries(testTreeReconstruction ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENCV_LIBS} ${IMAGE_LIBS} ${GV_LIBS})

add_executable(allbpt test/allbpt.cpp ${all_SRCS} ${all_HEADERS_MOC} ${all_FORMS_HEADERS} )
target_link_libraries(allbpt ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENCV_LIBS} ${IMAGE_LIBS} ${GV_LIBS})

add_executable(testSIFT test/testSIFT.cpp ${all_SRCS} ${all_HEADERS_MOC} ${all_FORMS_HEADERS} )
target_link_libraries(testSIFT ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENCV_LIBS} ${IMAGE_LIBS} ${GV_LIBS})

add_executable(genAllSIFT test/genAllSIFT.cpp ${all_SRCS} ${all_HEADERS_MOC} ${all_FORMS_HEADERS} )
target_link_libraries(genAllSIFT ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENCV_LIBS} ${IMAGE_LIBS} ${GV_LIBS})
