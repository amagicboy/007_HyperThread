cmake_minimum_required (VERSION 2.8)

project (demo)

include_directories (include)

# set (LIB_SRC ${PROJECT_SOURCE_DIR}/src/testFunc)
# add_library(testFunc_shared SHARED ${LIB_SRC})
# set_target_properties(testFunc_shared PROPERTIES OUTPUT_NAME "testFunc" )

# 设置库文件的输出路径
set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

add_subdirectory ("src")

add_library(numap SHARED
    ${PROJECT_SOURCE_DIR}/src/numap.c
    ${PROJECT_SOURCE_DIR}/src/numap_analyse.c
    ${PROJECT_SOURCE_DIR}/src/handle.c
)

target_link_libraries(numap LINK_PUBLIC numa pfm)

add_compile_options(-lpthread -lncurses)

# 设置构建类型和相应的编译选项
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb") 
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
