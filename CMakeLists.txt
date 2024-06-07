cmake_minimum_required(VERSION 3.7...3.22)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(BetfairStream VERSION 1.0
                      DESCRIPTION "Betfair Stream Client"
                      LANGUAGES CXX)

find_package(Boost REQUIRED COMPONENTS program_options)
find_package(nlohmann_json REQUIRED)

add_executable(betfair-stream
               src/main.cpp
               src/async_stream.cpp
               src/handler.cpp)
target_link_libraries(betfair-stream PUBLIC
                      Boost::boost
                      Boost::program_options
                      nlohmann_json::nlohmann_json
                      ssl
                      crypto
                      pthread)
target_compile_features(betfair-stream PRIVATE cxx_std_20)
install(TARGETS betfair-stream)
