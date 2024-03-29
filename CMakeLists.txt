cmake_minimum_required(VERSION 3.2)

project(hl_server C CXX)

SET (AUTH_SERVICE "auth_service")
SET (USER_SERVICE "user_service")
SET (EVENT_LISTENER "event_listener")
SET (PRODUCT_SERVICE "product_service")

set (STD_CXX "c++17")
set (REDISCPP_FLAGS "-DREDISCPP_HEADER_ONLY=ON")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -std=${STD_CXX} ${REDISCPP_FLAGS}")
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -g0 -std=${STD_CXX} -Wall -DNDEBUG ${REDISCPP_FLAGS}")

find_package(OpenSSL)
find_package(Threads)
find_package(ZLIB)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL)
find_package(CppKafka REQUIRED)


if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})
include_directories("/usr/include/mysql")
include_directories("/usr/local/include/mysql")
link_directories("/usr/local/lib")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${AUTH_SERVICE} auth_main.cpp 
    config/config.cpp 
    database/database.cpp 
    database/user.cpp
    database/cache.cpp)
target_include_directories(${AUTH_SERVICE} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${AUTH_SERVICE} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${AUTH_SERVICE} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${AUTH_SERVICE} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${AUTH_SERVICE} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoJWT"
                             "PocoDataMySQL"
                             "mysqlclient"
                             "rdkafka++"
                             ZLIB::ZLIB
                             CppKafka::cppkafka)


add_executable(${USER_SERVICE} user_main.cpp 
    config/config.cpp 
    database/database.cpp 
    database/user.cpp
    database/cache.cpp)
target_include_directories(${USER_SERVICE} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${USER_SERVICE} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${USER_SERVICE} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${USER_SERVICE} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${USER_SERVICE} PRIVATE 
                            ${CMAKE_THREAD_LIBS_INIT} 
                            ${Poco_LIBRARIES}
                            "PocoData"
                            "PocoDataMySQL"
                            "mysqlclient"
                            "rdkafka++"
                            ZLIB::ZLIB
                            CppKafka::cppkafka)


add_executable(${EVENT_LISTENER} event_main.cpp 
    config/config.cpp 
    database/database.cpp 
    database/user.cpp
    database/cache.cpp)
target_include_directories(${EVENT_LISTENER} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${EVENT_LISTENER} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${EVENT_LISTENER} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${EVENT_LISTENER} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${EVENT_LISTENER} PRIVATE 
                            ${CMAKE_THREAD_LIBS_INIT} 
                            ${Poco_LIBRARIES}
                            "PocoData"
                            "PocoDataMySQL"
                            "mysqlclient"
                            "rdkafka++"
                            ZLIB::ZLIB
                            CppKafka::cppkafka)

add_executable(${PRODUCT_SERVICE} product_main.cpp 
    config/config.cpp 
    database/database.cpp 
    database/user.cpp
    database/product.cpp)
target_include_directories(${PRODUCT_SERVICE} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${PRODUCT_SERVICE} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${PRODUCT_SERVICE} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${PRODUCT_SERVICE} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${PRODUCT_SERVICE} PRIVATE 
                            ${CMAKE_THREAD_LIBS_INIT} 
                            ${Poco_LIBRARIES}
                            "PocoData"
                            "PocoDataMySQL"
                            "mysqlclient"
                            ZLIB::ZLIB)

