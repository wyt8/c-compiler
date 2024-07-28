#pragma once
#include <string>
#include <vector>
#include <stack>

#include "global.h"

class AssemblyGenerator {
private:
    std::vector<Token> tokens;
    Table<std::string> identifierTable;
    std::string assemblyCode;
    int varNum; // 局部变量的数目

public:
    AssemblyGenerator(std::vector<Token> &tokens, Table<std::string>& identifierTable) : tokens(tokens), identifierTable(identifierTable) {
        this->varNum = identifierTable.size();
        this->genAssemblyFrame();
    }

    std::string generate() {
        parseFuncDefineStatement(0, tokens.size());
        return assemblyCode;
    }

private:
    // 向汇编代码中写入一行
    void writeLine(const std::string& line, int tabNum = 0);

    // 生成汇编代码的框架
    void genAssemblyFrame();

    // 获取变量在栈帧中的偏移量
    int getEbpOffset(Token& variable);

    // 处理变量声明语句
    void parseVarDeclareStatement(size_t start, size_t len);

    // 处理变量赋值语句
    void parseAssignStatement(size_t start, size_t len);

    // 处理返回语句
    void parseReturnStatement(size_t start, size_t len);

    // 获取指定位置
    size_t getLen(size_t start) {
        size_t len = 0;
        while (tokens[start + len].kind != wordType::SEPARATOR) {
            len++;
        }
        return len;
    }

    // 找到从start开始特定类型单词的第一次出现位置
    size_t findSpecialTypeWordIndex(size_t start, wordType type = wordType::SEPARATOR) {
        size_t res = start;
        while (tokens[start++].kind != type) {
            res++;
        }
        return res;
    }

    // 处理函数定义
    void parseFuncDefineStatement(size_t start, size_t len) {
        std::string line = "main:";
        writeLine(line, 0);
        line = "push ebp";
        writeLine(line, 1);
        line = "mov ebp, esp";
        writeLine(line, 1);
        line = "sub esp, 0x100";
        writeLine(line, 1);
        while (tokens[start++].kind != wordType::BRACE_LEFT) {
            len--;
        }
        len--;
        for (size_t i = start; i < start + len; i++)
        {
            if (tokens[i].kind == wordType::KEYWORD_int)
            {
                size_t separatorIndex = findSpecialTypeWordIndex(i);
                int len = separatorIndex - i;
                parseVarDeclareStatement(i, len);
                i += len;
            }
            else if (tokens[i].kind == wordType::KEYWORD_return)
            {
                size_t separatorIndex = findSpecialTypeWordIndex(i);
                int len = separatorIndex - i;
                parseReturnStatement(i, len);
                i += len;
            }
            else if (tokens[i].kind == wordType::IDENTIFIER && tokens[i + 1].kind == wordType::OPERATOR14_assignment)
            {
                size_t separatorIndex = findSpecialTypeWordIndex(i);
                int len = separatorIndex - i;
                parseAssignStatement(i, len);
                i += len;
            }
            else if (tokens[i].kind == wordType::IDENTIFIER_println_int) {
                size_t separatorIndex = findSpecialTypeWordIndex(i);
                int len = separatorIndex - i;
                parseFuncCallStatement(i, len);
                i += len;
            }
        }
    }

    // 处理函数调用语句
    void parseFuncCallStatement(size_t start, size_t len) {
        std::string line = "push DWORD PTR [ebp-" + std::to_string(getEbpOffset(tokens[start + 2])) + "]";
        writeLine(line, 1);
        line = "push offset format_str";
        writeLine(line, 1);
        line = "call printf";
        writeLine(line, 1);
        line = "add esp, 8";
        writeLine(line, 1);
    }

    // 将中缀表达式转换为后缀表达式
    std::vector<Token> getPostfixExpression(size_t start, size_t len);

    // 将eax寄存器中的值存储到栈上
    Token storeTemp() {
        Token token;
        token.kind = wordType::IDENTIFIER;
        token.code = this->varNum;
        this->varNum += 1;
        std::string line = "mov DWORD PTR [ebp-" + std::to_string(getEbpOffset(token)) + "], eax";
        writeLine(line, 1);
        return token;
    }

    // 减少varNum的数量
    void releaseTemp() {
        this->varNum -= 1;
    }

    // 处理计算表达式，表达式的值存储在eax寄存器中
    void parseExpressionStatement(size_t start, size_t len)
    {
        std::vector<Token> expression = getPostfixExpression(start, len);
        std::stack<Token> num;
        bool flag = false;
        for (size_t i = 0; i < expression.size(); i++)
        {
            switch (expression[i].kind)
            {
            case wordType::IDENTIFIER: case wordType::CONST_INT:
                num.push(expression[i]);
                break;
            default: // 双目运算符
            {
                if (flag) {
                    //releaseTemp();
                    flag = false;
                }
                auto right = num.top();
                num.pop();
                auto left = num.top();
                num.pop();
                binaryCalculate(left, right, expression[i]);
                num.push(storeTemp());
                flag = true;
                break;
            }
            }
        }
        std::string line = "mov eax, DWORD PTR [ebp-" + std::to_string(getEbpOffset(num.top())) + "]";
        writeLine(line, 1);
    }

    // 两者都是常数的双目运算符的运算过程：编译器直接计算
    int constBinaryCalculate(Token& left, Token& right, Token& op);

    // 左操作数存储在eax，右操作数存储在ebx，结果存储在eax中
    void varBinaryCalculate(Token& op);

    // 双目运算符的运算过程，结果存储在eax中
    void binaryCalculate(Token& left, Token& right, Token& op);
};
