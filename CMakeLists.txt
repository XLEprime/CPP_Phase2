cmake_minimum_required(VERSION 3.17)
project(CPP_Phase1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_PREFIX_PATH "D:/develop/Qt/5.12.2/mingw73_64")

find_package(Qt5 COMPONENTS Sql REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(main main.cpp src/user.cpp include/user.h src/database.cpp include/database.h src/item.cpp include/item.h src/time.cpp include/time.h)
target_link_libraries(main Qt5::Core Qt5::Sql)