#include "LexicalAnalyzer.h"


std::vector<Token> LexicalAnalyzer::lexicalAnalysis(const std::string& line)
{
    std::regex wordRegex("([a-zA-Z_]\\w*)|\\d+|(<=)|(>=)|(==)|(!=)|(>>)|(<<)|[-=+*/%();{}!&|^<>]");
    std::sregex_iterator wordsBegin = std::sregex_iterator(line.begin(), line.end(), wordRegex);
    std::sregex_iterator wordsEnd = std::sregex_iterator();

    std::vector<Token> lineTokens;
    for (std::sregex_iterator i = wordsBegin; i != wordsEnd; i++)
    {
        std::smatch match = *i;
        std::string word = match.str();
        Token token;
        for (auto it = wordTypeRegexMap.begin(); it != wordTypeRegexMap.end(); ++it) {
            if (regex_match(word, it->second)) {
                token.kind = it->first;
                switch (it->first) {
                case wordType::IDENTIFIER:
                    token.code = identifierTable.add(word);
                    break;
                case wordType::CONST_INT:
                    token.code = stoi(word);
                    break;
                case wordType::OPERATOR1_left_parentheses: case wordType::OPERATOR1_right_parentheses:
                    token.code = 1;
                    break;
                case wordType::OPERATOR3_div: case wordType::OPERATOR3_mul: case wordType::OPERATOR3_mod:
                    token.code = 3;
                    break;
                case wordType::OPERATOR4_add: case wordType::OPERATOR4_sub:
                    token.code = 4;
                    break;
                case wordType::OPERATOR5_left_shift: case wordType::OPERATOR5_right_shift:
                    token.code = 5;
                    break;
                case wordType::OPERATOR6_less: case wordType::OPERATOR6_less_equal: case wordType::OPERATOR6_greater: case wordType::OPERATOR6_greater_equal:
                    token.code = 6;
                    break;
                case wordType::OPERATOR7_equal: case wordType::OPERATOR7_not_equal:
                    token.code = 7;
                    break;
                case wordType::OPERATOR8_logical_and:
                    token.code = 8;
                    break;
                case wordType::OPERATOR9_logical_xor:
                    token.code = 9;
                    break;
                case wordType::OPERATOR10_logical_or:
                    token.code = 10;
                    break;
                case wordType::OPERATOR14_assignment:
                    token.code = 14;
                    break;
                default:
                    token.code = 0;
                }
                break;
            }
        }
        lineTokens.push_back(token);
    }
    tokens.insert(tokens.end(), lineTokens.begin(), lineTokens.end());
    return lineTokens;
}