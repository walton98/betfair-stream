cmake_minimum_required(VERSION 3.7...3.22)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(BetfairStream VERSION 1.0
                      DESCRIPTION "Betfair Stream Client"
                      LANGUAGES CXX)

find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(main main.cpp async_stream.cpp handler.cpp)
target_link_libraries(main PUBLIC
                      Boost::boost
                      Boost::program_options
                      ssl
                      crypto
                      pthread)
target_compile_features(main PRIVATE cxx_std_20)
