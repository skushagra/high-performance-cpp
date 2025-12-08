cmake_minimum_required(VERSION 3.10)
project(LoggerApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add all source files
add_executable(logger
    main.cpp
    Mather.cpp
    LinkedList.cpp
)

# Include header files directory
target_include_directories(logger PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

