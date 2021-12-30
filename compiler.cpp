#include <iostream>
#include <fstream>
#include "Scanner.h"

int main() {

    std::ifstream ifile(R"(myprog.minic)");
    Scanner scanner(ifile);

    for (;;) {
        Token currentLexem = scanner.getNextToken();
        currentLexem.print(std::cout);

        if (currentLexem.type() == LexemType::error || currentLexem.type() == LexemType::eof) {
            break;
        }
    }
};