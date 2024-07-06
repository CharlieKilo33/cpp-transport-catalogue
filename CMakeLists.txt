cmake_minimum_required(VERSION 3.26)
project(cpp-search-server)

set(CMAKE_CXX_STANDARD 17)

add_executable(main transport-catalogue/map_renderer.cpp transport-catalogue/map_renderer.h transport-catalogue/domain.cpp transport-catalogue/domain.h transport-catalogue/geo.cpp transport-catalogue/geo.h transport-catalogue/json.cpp transport-catalogue/json.h transport-catalogue/json_reader.cpp transport-catalogue/json_reader.h transport-catalogue/main.cpp  transport-catalogue/request_handler.cpp transport-catalogue/request_handler.h transport-catalogue/svg.cpp transport-catalogue/svg.h transport-catalogue/transport_catalogue.cpp transport-catalogue/transport_catalogue.h)