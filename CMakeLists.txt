cmake_minimum_required(VERSION 3.10)

project(WebCrawler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Find CURL library
find_package(CURL REQUIRED)

add_executable(WebCrawler main.cpp)

target_link_libraries(WebCrawler ${CURL_LIBRARIES})