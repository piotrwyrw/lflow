cmake_minimum_required(VERSION 3.24)
project(lflow C)

set(CMAKE_C_STANDARD 11)

add_executable(lflow main.c src/include/token.h src/token.c src/include/tokenizer.h src/tokenizer.c src/include/xstring.h src/xstring.c src/include/status.h src/include/io.h src/io.c src/include/ast.h src/include/arr.h src/arr.c src/include/bool.h src/ast.c src/include/parse.h src/parse.c)
