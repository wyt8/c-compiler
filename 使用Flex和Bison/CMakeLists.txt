cmake_minimum_required(VERSION 3.16)  
project(lab04)  

set(CMAKE_CXX_STANDARD 14)

# 查找Bison和Flex模块  
find_package(BISON REQUIRED)  
find_package(FLEX REQUIRED)  

# 检查是否找到了Bison和Flex，并设置必要的变量  
if(NOT BISON_FOUND)  
    message(FATAL_ERROR "Bison not found!")  
endif()  

if(NOT FLEX_FOUND)  
    message(FATAL_ERROR "Flex not found!")  
endif()  

include_directories(${CMAKE_SOURCE_DIR}) 

# 假设您已经定义了BISON_TARGET和FLEX_TARGET宏  
# 注意：这些宏可能不是CMake内置的，而是来自FindBison.cmake和FindFlex.cmake模块  
BISON_TARGET(MyParser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)  
FLEX_TARGET(MyScanner token.l ${CMAKE_CURRENT_BINARY_DIR}/token.cpp)  
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

# 添加可执行文件  
add_executable(Compilerlab4
    ${BISON_MyParser_OUTPUTS}  
    ${FLEX_MyScanner_OUTPUTS}
    # global.h
    main.cpp  
)  

# 设置C++标准  
target_compile_features(Compilerlab4 PRIVATE cxx_std_14)
