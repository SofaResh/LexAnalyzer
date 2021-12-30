#include "Scanner.h"
#include <iostream>
#include <string>
#include <map>
#include <set>


std::set<char> ws{ ' ', '\n', '\t', '\r' };


std::map<char, LexemType> punctuation{ {'(', LexemType::lpar},
									  {')', LexemType::rpar},
									  {'{', LexemType::lbrace},
									  {'}', LexemType::rbrace},
									  {'[', LexemType::lbracket},
									  {']', LexemType::rbracket},
									  {';', LexemType::semicolon},
									  {',', LexemType::comma},
									  {':', LexemType::colon} };

std::map<std::string, LexemType> keywords{ {"int",    LexemType::kwint},
										  {"char",   LexemType::kwchar},
										  {"if",     LexemType::kwif},
										  {"else",   LexemType::kwelse},
										  {"switch", LexemType::kwswitch},
										  {"case",   LexemType::kwcase},
										  {"while",  LexemType::kwwhile},
										  {"for",    LexemType::kwfor},
										  {"return", LexemType::kwreturn},
										  {"in",     LexemType::kwin},
										  {"out",    LexemType::kwout} };


Token::Token(LexemType type) : _type(type) {};

Token::Token(LexemType type, int value) : _type(type), _value(value) {};

Token::Token(LexemType type, const std::string& str) : _type(type), _str(str) {};

int Token::value() { return _value; };

std::string Token::str() { return _str; };

LexemType Token::type() { return _type; };

void Token::print(std::ostream& stream) {
	if (_type == LexemType::eof) {
		stream << "[" + lexemToString(_type) + ']' << std::endl;
	}
	else if (_type == LexemType::error) {
		stream << '[' + lexemToString(_type) + ':' + _str + ']' << std::endl;
	}
	else if (!_str.empty()) {
		stream << '[' + lexemToString(_type) + ", " + "\"" + _str + +"\"" + ']' << std::endl;
	}
	else if (_type == LexemType::num) {
		stream << '[' + lexemToString(_type) + ", " << _value << ']' << std::endl;
	}
	else {
		stream << '[' + lexemToString(_type) + ']' << std::endl;
	}
}


Scanner::Scanner(std::istream& stream) : _lexemStream(stream), _state(0) {};

Token Scanner::getNextToken() {
	while (true) {
		_lexemStream >> std::noskipws >> _currentChar;
		switch (_state) {
		case 0:

			if (_lexemStream.eof()) { return { LexemType::eof }; }

			if (punctuation.contains(_currentChar)) { return { punctuation[_currentChar] }; }

			if (_currentChar == '>') { return { LexemType::opgt }; }
			if (_currentChar == '$') { return { LexemType::eof }; }
			if (_currentChar == '*') { return { LexemType::opmult }; }

			if (ws.contains(_currentChar)) {
				continue;
			}

			if (std::isdigit(_currentChar)) {
				_digitValue = _currentChar % 48;
				_state = 1;
				continue;
			}

			if (_currentChar == '\'') {
				_stringValue = "";
				_state = 2;
				continue;
			}

			if (_currentChar == '"') {
				_stringValue = "";
				_state = 4;
				continue;
			}

			if (std::isalpha(_currentChar)) {
				_stringValue = _currentChar;
				_state = 5;
				continue;
			}

			if (_currentChar == '-') {
				_state = 6;
				continue;
			}

			if (_currentChar == '!') {
				_state = 7;
				continue;
			}

			if (_currentChar == '<') {
				_state = 8;
				continue;
			}

			if (_currentChar == '=') {
				_state = 9;
				continue;
			}

			if (_currentChar == '+') {
				_state = 10;
				continue;
			}

			if (_currentChar == '|') {
				_state = 11;
				continue;
			}

			if (_currentChar == '&') {
				_state = 12;
				continue;
			}

			return { LexemType::error, "Wrong symbol" };

		case 1:
			while (std::isdigit(_currentChar) && !_lexemStream.eof()) {
				if (_digitValue > 0)
					_digitValue = _digitValue * 10 + (int)(_currentChar - '0');
				else
					_digitValue = _digitValue * 10 - (int)(_currentChar - '0');
				_lexemStream >> std::noskipws >> _currentChar;
			}
			_state = 0;
			if (!_lexemStream.eof()) { _lexemStream.putback(_currentChar); }
			return { LexemType::num, _digitValue };

		case 2:
			if (_currentChar == '\'') { return { LexemType::error, "Empty character constant" }; }
			_stringValue = _currentChar;
			_state = 3;
			continue;

		case 3:
			if (_currentChar == '\'') {
				_state = 0;
				return { LexemType::chr, _stringValue };
			}
			return { LexemType::error, "Char must contain only one symbol" };

		case 4:
			while (_currentChar != '$' && _currentChar != '"' && !_lexemStream.eof()) {
				_stringValue += _currentChar;
				_lexemStream >> std::noskipws >> _currentChar;
			}


			if (_lexemStream.eof()) { return { LexemType::error, "Missing close \"" }; }
			if (_currentChar == '"') {
				_state = 0;
				return { LexemType::str, _stringValue };
			}

		case 5:
			while (!_lexemStream.eof() && (std::isalpha(_currentChar) || (std::isdigit(_currentChar)))) {
				_stringValue += _currentChar;
				_lexemStream >> std::noskipws >> _currentChar;
			}

			_state = 0;
			if (!_lexemStream.eof()) { _lexemStream.putback(_currentChar); }

			if (keywords.contains(_stringValue)) { return { keywords[_stringValue] }; }

			return { LexemType::id, _stringValue };

		case 6:
			if (std::isdigit(_currentChar)) {
				_state = 1;
				_digitValue = (-1) * _currentChar % 48;
				continue;
			}
			_state = 0;
			if (!_lexemStream.eof()) { _lexemStream.putback(_currentChar); }
			return { LexemType::opminus };

		case 7:
			_state = 0;
			if (_currentChar == '=') { return { LexemType::opne }; }
			if (!_lexemStream.eof()) { _lexemStream.putback(_currentChar); }
			return { LexemType::opnot };

		case 8:
			_state = 0;
			if (_currentChar == '=') { return { LexemType::ople }; }
			if (!_lexemStream.eof()) { _lexemStream.putback(_currentChar); }
			return { LexemType::oplt };

		case 9:
			_state = 0;
			if (_currentChar == '=' && !_lexemStream.eof()) { return { LexemType::opeq }; }
			if (!_lexemStream.eof()) { _lexemStream.putback(_currentChar); }
			return { LexemType::opassign };

		case 10:
			_state = 0;
			if (_currentChar == '+' && !_lexemStream.eof()) { return { LexemType::opinc }; }
			if (!_lexemStream.eof()) { _lexemStream.putback(_currentChar); }
			return { LexemType::opplus };

		case 11:
			if (_currentChar == '|' && !_lexemStream.eof()) {
				_state = 0;
				return { LexemType::opor };
			}
			return { LexemType::error, "Incomplete OR operator" };

		case 12:
			if (_currentChar == '&' && !_lexemStream.eof()) {
				_state = 0;
				return { LexemType::opand };
			}
			return { LexemType::error, "Incomplete AND operator" };
		}
	}
}


