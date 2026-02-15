cmake_minimum_required(VERSION 3.23)
project(csv_median_calculator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(Boost 1.74 REQUIRED COMPONENTS program_options)
find_package(tomlplusplus CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(csv_median_calculator
  src/main.cpp
  src/config_parser.hpp
  src/csv_reader.hpp
  src/median_calculator.hpp
)

target_include_directories(csv_median_calculator PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(csv_median_calculator PRIVATE Boost::program_options spdlog::spdlog tomlplusplus::tomlplusplus)

target_compile_features(csv_median_calculator PRIVATE cxx_std_23)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
