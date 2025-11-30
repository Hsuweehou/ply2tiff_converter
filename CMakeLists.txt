cmake_minimum_required(VERSION 3.18)
project(ply2tiff_converter)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt5组件
# 注意：如果CMake找不到Qt5，可以通过以下方式指定Qt路径：
# 1. 在命令行中使用 -DCMAKE_PREFIX_PATH="D:/Qt/5.x.x/mingw81_64" 或 -DQt5_DIR="D:/Qt/5.x.x/mingw81_64/lib/cmake/Qt5"
# 2. 在CMake GUI中设置相应的变量
# set(Qt5_DIR "C:/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5")
# ##########################################################
# # Setting path for 3rd party libraries
# ##########################################################
set(QT_LIBRARY_DIR "D:/Qt" CACHE PATH "The path to third party library")  # set to the relative path to current CmakeLists.txt
set(THIRD_PARTY_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/../3rdParty CACHE PATH "The path of third party libraries.")  # set to the relative path to current CmakeLists.txt

if (NOT DEFINED CMAKE_BUILD_TYPE)
    message("CMAKE_BUILD_TYPE is not defined! Set it as Release by default.")
    set(CMAKE_BUILD_TYPE "Release")
endif()
# Qt5
set(Qt5_DIR ${QT_LIBRARY_DIR}/Qt5.12.8/5.12.8/msvc2017_64/lib/cmake/Qt5)
message(STATUS "Qt5_DIR = " ${Qt5_DIR})
# find_package(Qt5 REQUIRED PATHS ${Qt5_DIR})
find_package(Qt5 COMPONENTS Widgets REQUIRED)
if(Qt5_FOUND)
    message("Qt5 ${Qt5_VERSION} found!")
else()
    message(FATAL_ERROR "Can not find Qt5!")
endif()

# 设置包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 添加可执行文件
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/mainwindow.cpp
    include/mainwindow.h
    src/ply_processor.cpp
    include/ply_processor.h
    src/advanced_ply_processor.cpp
    include/advanced_ply_processor.h
    src/a_ply2tiff_converter.cpp
    include/a_ply2tiff_converter.h
    src/ply2tiff_converter.cpp
    resources/resources.qrc
    resources/app.rc
)

# 链接Qt库
target_link_libraries(${PROJECT_NAME} 
    Qt5::Core 
    Qt5::Widgets
)

# 查找OpenCV
# #OpenCV
set(OpenCV_DIR ${THIRD_PARTY_LIBRARY_DIR}/OpenCV/opencv4/build/x64/vc15/lib)
message(STATUS "OpenCV_DIR = ${OpenCV_DIR}")
find_package(OpenCV 4.5.1 REQUIRED)
if(OpenCV_FOUND)
    message ("opencv found")
else()
    message (FATAL_ERROR "Cannot find opencv")
endif()

include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

# 如果happly.h在include目录中
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

#注释掉可以保留运行时的终端
Set_Target_Properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

# 设置MSVC编译器选项，确保正确处理UTF-8源文件
# /utf-8 选项等同于 /source-charset:utf-8 /execution-charset:utf-8
# 这确保源文件和执行字符集都使用UTF-8编码，解决中文常量编译错误
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
    # 同时设置全局编译选项作为备用
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
endif()