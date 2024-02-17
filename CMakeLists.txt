cmake_minimum_required(VERSION 3.16...3.28)

project(passtables LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(passtable main.cpp)

find_package(CLI11 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(range-v3 REQUIRED)
find_package(spdlog REQUIRED)

target_link_libraries(passtable
    CLI11::CLI11
    OpenSSL::Crypto
    range-v3::concepts
    range-v3::meta
    range-v3::range-v3
    spdlog::spdlog
)

include(GNUInstallDirs)
install(TARGETS passtable
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
