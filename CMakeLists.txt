cmake_minimum_required(VERSION 3.10)

# 项目名称（随意取）
project(MixedCpp VERSION 1.0 LANGUAGES C CXX)

# 设置 C 和 C++ 标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 收集当前目录下所有 .c 和 .cpp 文件（递归）
file(GLOB_RECURSE SOURCES *.c *.cpp)

# 【关键】排除 build 目录下的文件，防止重复定义报错
list(FILTER SOURCES EXCLUDE REGEX "[\\\\/]build[\\\\/]")

# 为每个源文件单独生成一个可执行目标
foreach(SRC ${SOURCES})
    get_filename_component(EXE_NAME ${SRC} NAME_WE)
    # 获取文件后缀（c 或 cpp）
    get_filename_component(EXT ${SRC} EXT)
    # 将后缀去掉点号，拼接到目标名上
    string(REPLACE "." "" EXT_CLEAN ${EXT})
    add_executable(${EXE_NAME}_${EXT_CLEAN} ${SRC})
endforeach()