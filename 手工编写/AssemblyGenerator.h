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
    int varNum; // �ֲ���������Ŀ

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
    // ���������д��һ��
    void writeLine(const std::string& line, int tabNum = 0);

    // ���ɻ�����Ŀ��
    void genAssemblyFrame();

    // ��ȡ������ջ֡�е�ƫ����
    int getEbpOffset(Token& variable);

    // ��������������
    void parseVarDeclareStatement(size_t start, size_t len);

    // ���������ֵ���
    void parseAssignStatement(size_t start, size_t len);

    // ���������
    void parseReturnStatement(size_t start, size_t len);

    // ��ȡָ��λ��
    size_t getLen(size_t start) {
        size_t len = 0;
        while (tokens[start + len].kind != wordType::SEPARATOR) {
            len++;
        }
        return len;
    }

    // �ҵ���start��ʼ�ض����͵��ʵĵ�һ�γ���λ��
    size_t findSpecialTypeWordIndex(size_t start, wordType type = wordType::SEPARATOR) {
        size_t res = start;
        while (tokens[start++].kind != type) {
            res++;
        }
        return res;
    }

    // ����������
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

    // �������������
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

    // ����׺���ʽת��Ϊ��׺���ʽ
    std::vector<Token> getPostfixExpression(size_t start, size_t len);

    // ��eax�Ĵ����е�ֵ�洢��ջ��
    Token storeTemp() {
        Token token;
        token.kind = wordType::IDENTIFIER;
        token.code = this->varNum;
        this->varNum += 1;
        std::string line = "mov DWORD PTR [ebp-" + std::to_string(getEbpOffset(token)) + "], eax";
        writeLine(line, 1);
        return token;
    }

    // ����varNum������
    void releaseTemp() {
        this->varNum -= 1;
    }

    // ���������ʽ�����ʽ��ֵ�洢��eax�Ĵ�����
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
            default: // ˫Ŀ�����
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

    // ���߶��ǳ�����˫Ŀ�������������̣�������ֱ�Ӽ���
    int constBinaryCalculate(Token& left, Token& right, Token& op);

    // ��������洢��eax���Ҳ������洢��ebx������洢��eax��
    void varBinaryCalculate(Token& op);

    // ˫Ŀ�������������̣�����洢��eax��
    void binaryCalculate(Token& left, Token& right, Token& op);
};
