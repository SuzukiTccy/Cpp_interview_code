# CMake file


# cmake minimum version
cmake_minimum_required(VERSION 3.10)

# project name
project(Interview)

# set C++ version
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED) # 关键：启用线程支持

add_executable(exe_Interview)

add_subdirectory(./src ../bin) 
# 指明本项目包含一个子目录 src，这样 src 目录下的 CMakeLists.txt 文件和源代码也会被处理
# 并将构建输出放在bin目录，bin目录的位置是 相对于执行cmake命令时所在的位置 来说的，在这里就是build目录

# 给exe_Interview添加源文件
target_sources(exe_Interview
    PRIVATE
    ./main.cpp
)

# 给exe_Interview添加头文件搜索路径
target_include_directories(exe_Interview
    PUBLIC
        ./include
)

# 给Advance添加依赖库
target_link_libraries(exe_Interview PRIVATE Lib Threads::Threads)

# 针对不同平台的编译选项
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")  # macOS
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # Windows 通常不需要额外标志
endif()

# ========== 修改：添加run目标 ==========
# 创建run目标，只有在显式执行make run时才会运行程序
add_custom_target(run
    DEPENDS exe_Interview
    COMMAND echo "=== 运行程序 ==="
    COMMAND $<TARGET_FILE:exe_Interview> # 生成器表达式，获取Advance可执行文件的路径
    COMMAND echo "=== 程序执行完成 ==="
)