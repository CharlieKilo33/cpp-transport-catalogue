cmake_minimum_required(VERSION 3.26)
project(cpp-search-server)

set(CMAKE_CXX_STANDARD 17)

add_executable(main transport-catalogue/main.cpp transport-catalogue/geo.h transport-catalogue/input_reader.h transport-catalogue/input_reader.cpp transport-catalogue/stat_reader.h transport-catalogue/stat_reader.cpp transport-catalogue/transport_catalogue.h transport-catalogue/transport_catalogue.cpp)