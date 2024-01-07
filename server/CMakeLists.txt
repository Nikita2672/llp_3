cmake_minimum_required(VERSION 3.26)
project(server C)

set(CMAKE_CXX_STANDARD 17)

find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

add_executable(server
        socket_wrapper.c
        database/src/data/comparator.c
        database/src/file/allocator.c
        database/src/file/fileApi.c
        database/src/file/iterator.c
        database/src/file/tableBlocks.c
        database/src/platformic/cutfile.c
        database/src/query/query.c
        database/src/util/testPerfomance.c
        database/src/util/unitTests.c
        database/src/util/util.c
        main.c
#        database/src/main.c
        transfer/from_xml.c
        transfer/from_xml.h
)

target_link_libraries(server ${LIBXML2_LIBRARIES})
