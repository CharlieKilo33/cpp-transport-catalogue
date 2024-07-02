cmake_minimum_required(VERSION 3.26)
project(cpp-search-server)

set(CMAKE_CXX_STANDARD 17)

add_executable(main transport-catalogue/main.cpp transport-catalogue/geo.h transport-catalogue/input_reader.h transport-catalogue/input_reader.cpp transport-catalogue/stat_reader.h transport-catalogue/stat_reader.cpp transport-catalogue/transport_catalogue.h transport-catalogue/transport_catalogue.cpp)
add_executable(tst test/domain.cpp test/domain.h test/geo.cpp test/geo.h test/json.cpp test/json.h test/json_reader.cpp test/json_reader.h test/main.cpp test/map_renderer.cpp test/map_renderer.h test/request_handler.cpp test/request_handler.h test/svg.cpp test/svg.h test/transport_catalogue.cpp test/transport_catalogue.h)