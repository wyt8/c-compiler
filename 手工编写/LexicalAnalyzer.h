#pragma once
#include <regex>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "global.h"

class LexicalAnalyzer
{
public:
	LexicalAnalyzer() {}

	Table<std::string> identifierTable; // 标识符表
    std::vector<Token> tokens; // 存放源文件的属性字流

    // 词法分析，将字符流转换为属性字流
	std::vector<Token> lexicalAnalysis(const std::string& line);

private:
    const std::map<wordType, std::regex> wordTypeRegexMap = {
    {wordType::KEYWORD_int, std::regex("int")},
    {wordType::KEYWORD_return, std::regex("return")},
    {wordType::IDENTIFIER_main, std::regex("main")},
    {wordType::IDENTIFIER_println_int, std::regex("println_int")},
    {wordType::IDENTIFIER, std::regex("[a-zA-Z_][a-zA-Z0-9_]*")},
    {wordType::CONST_INT, std::regex("\\d+")},
    {wordType::OPERATOR1_left_parentheses, std::regex("\\(")},
    {wordType::OPERATOR1_right_parentheses, std::regex("\\)")},
    {wordType::OPERATOR3_div, std::regex("/")},
    {wordType::OPERATOR3_mul, std::regex("\\*")},
    {wordType::OPERATOR3_mod, std::regex("%")},
    {wordType::OPERATOR4_add, std::regex("\\+")},
    {wordType::OPERATOR4_sub, std::regex("\\-")},
    {wordType::OPERATOR5_left_shift, std::regex("<<")},
    {wordType::OPERATOR5_right_shift, std::regex(">>")},
    {wordType::OPERATOR6_less, std::regex("<")},
    {wordType::OPERATOR6_less_equal, std::regex("<=")},
    {wordType::OPERATOR6_greater, std::regex(">")},
    {wordType::OPERATOR6_greater_equal, std::regex(">=")},
    {wordType::OPERATOR7_equal, std::regex("==")},
    {wordType::OPERATOR7_not_equal, std::regex("!=")},
    {wordType::OPERATOR8_logical_and, std::regex("&")},
    {wordType::OPERATOR9_logical_xor, std::regex("\\^")},
    {wordType::OPERATOR10_logical_or, std::regex("\\|")},
    {wordType::OPERATOR14_assignment, std::regex("=")},
    {wordType::SEPARATOR, std::regex(";")},
    {wordType::BRACE_LEFT, std::regex("\\{")},
    {wordType::BRACE_RIGHT, std::regex("\\}")},
    };
};

