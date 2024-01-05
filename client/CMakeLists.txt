cmake_minimum_required(VERSION 3.26)
project(client)

set(CMAKE_CXX_STANDARD 17)


find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(LibXml2 REQUIRED)


set(BISON_INPUT_FILE parser/parser/parser.y)
set(FLEX_INPUT_FILE parser/lexer/lexer.l)

BISON_TARGET(MyParser ${BISON_INPUT_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/parser/parser/parser.c)
FLEX_TARGET(MyLexer ${FLEX_INPUT_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/parser/lexer/lexer.c)
ADD_FLEX_BISON_DEPENDENCY(MyLexer MyParser)

include_directories(./parser)
include_directories(${LIBXML2_INCLUDE_DIR})

add_executable(client main.c ${BISON_MyParser_OUTPUTS} ${FLEX_MyLexer_OUTPUTS}
        parser/ast/ast.c parser/input.c net/socket_wrapper.c
        transfer/ast_to_xml.c
        transfer/ast_to_xml.h)

target_link_libraries(client ${LIBXML2_LIBRARIES})
