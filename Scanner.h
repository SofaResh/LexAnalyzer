#pragma once

#include "LexicalAnalyser.h"
#include <iostream>
#include <string>

class Token {
private:
    LexemType _type;
    int _value;
    std::string _str;

public:
    Token(LexemType type);

    Token(LexemType type, int value);

    Token(LexemType type, const std::string &str);

    int value();

    std::string str();

    LexemType type();

    void print(std::ostream &stream);

};

class Scanner {
private:
    char _currentChar;
    std::istream &_lexemStream;
    int _state;
    int _digitValue;
    std::string _stringValue;

public:
    Scanner(std::istream &stream);

    Token getNextToken();
};
