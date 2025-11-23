cmake_minimum_required(VERSION 3.16)
project(FootballManagement LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Шлях до заголовків
include_directories(${CMAKE_SOURCE_DIR}/include)

# Усі .cpp з src/
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(FootballManagement ${SOURCES}
        src/SeedDemo.cpp)

# macOS / Linux: звичайно дод. лінкування для filesystem не потрібне у C++17,
# але якщо компілятор старий — розкоментуй наступний рядок:
# target_link_libraries(FootballManagement stdc++fs)
