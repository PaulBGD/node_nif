cmake_minimum_required(VERSION 3.16)
project(node_nif)
cmake_policy(SET CMP0042 NEW)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_JS_INC})
add_subdirectory(lib/NIF)

file(GLOB SOURCE_FILES "library.cpp")
add_library(node_nif SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
set_target_properties(node_nif PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(node_nif PRIVATE nif ${CMAKE_JS_LIB})

execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
target_include_directories(node_nif PRIVATE ${NODE_ADDON_API_DIR})

add_definitions(-DNAPI_VERSION=3)
