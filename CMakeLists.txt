cmake_minimum_required(VERSION 3.28)
project(volt C)

set(CMAKE_C_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(volt
    main.c
    src/hashtable.c
    src/utf8.c
    src/lexer/lexer.c src/lexer/numeric_literals.c
    src/parser/parser.c
)
