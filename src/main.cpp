#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <stdio.h>
#include <stdlib.h>

#include "lexer/lexer.h"
#include "parser/parser.h"

std::string read_file(std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("selected file doesn't exist");
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    std::string filename(argv[1]);
    std::string source = read_file(filename);
    
    try {
        Lexer lexer(filename, source.c_str());
        Parser parser(lexer);
        
        for (auto &stmt : parser.parse()) {
            std::cout << *stmt << std::endl;
        }
    } catch (LexerError &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
