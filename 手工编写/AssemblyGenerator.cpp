#include "AssemblyGenerator.h"

void AssemblyGenerator::writeLine(const std::string& line, int tabNum) {
    for (int i = 0; i < tabNum; i++) {
        this->assemblyCode += "\t";
    }
    this->assemblyCode += line + "\n";
}

void AssemblyGenerator::genAssemblyFrame() {
    this->writeLine(".intel_syntax noprefix");
    this->writeLine(".global main");
    this->writeLine(".extern printf");
    this->writeLine(".data");
    this->writeLine("format_str:");
    this->writeLine(".asciz \"%d\\n\"", 1);
    this->writeLine(".text");
}

int AssemblyGenerator::getEbpOffset(Token& variable) {
    return (variable.code + 1) * 4;
}

void AssemblyGenerator::parseVarDeclareStatement(size_t start, size_t len) {
    std::string line = "mov DWORD PTR [ebp-" + std::to_string(this->getEbpOffset(tokens[start + 1])) + "], 0";
    this->writeLine(line, 1);
}

void AssemblyGenerator::parseAssignStatement(size_t start, size_t len) {
    std::string line;
    if (len == 3 && tokens[start + 2].kind == wordType::CONST_INT) { // 常量赋值
        line = "mov DWORD PTR [ebp-" + std::to_string(this->getEbpOffset(tokens[start])) + "], " + std::to_string(tokens[start + 2].code);
        this->writeLine(line, 1);
    }
    else if (len == 3 && tokens[start + 2].kind == wordType::IDENTIFIER) { // 变量赋值
        line = "mov eax, DWORD PTR [ebp-" + std::to_string(this->getEbpOffset(tokens[start + 2])) + "]";
        this->writeLine(line, 1);
        line = "mov DWORD PTR [ebp-" + std::to_string(getEbpOffset(tokens[start])) + "], eax";
        this->writeLine(line, 1);
    }
    else { // 表达式赋值
        parseExpressionStatement(start + 2, len - 2);
        line = "mov DWORD PTR [ebp-" + std::to_string(getEbpOffset(tokens[start])) + "], eax\n";
        this->writeLine(line, 1);
    }
}

void AssemblyGenerator::parseReturnStatement(size_t start, size_t len) {
    if (len == 2 && tokens[start + 1].kind == wordType::CONST_INT) {
        std::string line = "mov eax, " + std::to_string(tokens[start + 1].code);
        this->writeLine(line, 1);
    }
    else if (len == 2 && tokens[start + 1].kind == wordType::IDENTIFIER) {
        std::string line = "mov eax, DWORD PTR [ebp-" + std::to_string(this->getEbpOffset(tokens[start + 1])) + "]";
        this->writeLine(line, 1);
    }
    std::string line = "leave";
    this->writeLine(line, 1);
    line = "ret";
    this->writeLine(line, 1);
}

std::vector<Token> AssemblyGenerator::getPostfixExpression(size_t start, size_t len)
{
    std::stack<Token> op;
    std::vector<Token> res;

    for (size_t i = start; i < start + len; i++)
    {
        if (tokens[i].kind == wordType::IDENTIFIER || tokens[i].kind == wordType::CONST_INT)
        {
            res.push_back(tokens[i]);
        }
        else
        {
            switch (tokens[i].code)
            {
            case 1:
                if (tokens[i].kind == wordType::OPERATOR1_left_parentheses) {
                    op.push(tokens[i]);
                }
                else if (tokens[i].kind == wordType::OPERATOR1_right_parentheses) { // 遇到")"则弹出符号栈直到"("弹出
                    if (op.empty())
                        break;
                    Token temp = op.top();

                    while (temp.kind != wordType::OPERATOR1_left_parentheses) {
                        res.push_back(temp);
                        op.pop();
                        if (op.empty())
                            break;
                        temp = op.top();
                    }
                    op.pop();
                }
                break;
            case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 14:
                if (!op.empty()) {
                    Token temp = op.top();
                    while (temp.code <= tokens[i].code && temp.code != 1) {
                        res.push_back(temp);
                        op.pop();
                        if (op.empty())
                            break;
                        temp = op.top();
                    }
                }
                op.push(tokens[i]);
                break;
            }
        }
    }

    while (!op.empty()) {
        res.push_back(op.top());
        op.pop();
    }
    return res;
}

int AssemblyGenerator::constBinaryCalculate(Token& left, Token& right, Token& op) {
    int res;
    int leftNum = left.code;
    int rightNum = left.code;
    switch (op.kind) {
    case wordType::OPERATOR3_div:
        res = leftNum / rightNum;
        break;
    case wordType::OPERATOR3_mul:
        res = leftNum * rightNum;
        break;
    case wordType::OPERATOR3_mod:
        res = leftNum % rightNum;
        break;
    case wordType::OPERATOR4_add:
        res = leftNum + rightNum;
        break;
    case wordType::OPERATOR4_sub:
        res = leftNum - rightNum;
        break;
    case wordType::OPERATOR5_left_shift:
        res = leftNum << rightNum;
        break;
    case wordType::OPERATOR5_right_shift:
        res = leftNum >> rightNum;
        break;
    case wordType::OPERATOR6_less:
        res = leftNum < rightNum;
        break;
    case wordType::OPERATOR6_less_equal:
        res = leftNum <= rightNum;
        break;
    case wordType::OPERATOR6_greater:
        res = leftNum > rightNum;
        break;
    case wordType::OPERATOR6_greater_equal:
        res = leftNum >= rightNum;
        break;
    case wordType::OPERATOR7_equal:
        res = leftNum == rightNum;
        break;
    case wordType::OPERATOR7_not_equal:
        res = leftNum != rightNum;
        break;
    case wordType::OPERATOR8_logical_and:
        res = leftNum & rightNum;
        break;
    case wordType::OPERATOR9_logical_xor:
        res = leftNum ^ rightNum;
        break;
    case wordType::OPERATOR10_logical_or:
        res = leftNum | rightNum;
        break;
    }
    return res;
}

void AssemblyGenerator::varBinaryCalculate(Token& op) {
    std::string line;
    switch (op.kind) {
    case wordType::OPERATOR3_div:
        line = "cdq";
        writeLine(line, 1);
        line = "idiv ebx";
        writeLine(line, 1);
        break;
    case wordType::OPERATOR3_mul:
        line = "imul eax, ebx";
        writeLine(line, 1);
        break;
    case wordType::OPERATOR3_mod:
        line = "cdq";
        writeLine(line, 1);
        line = "idiv ebx";
        writeLine(line, 1);
        line = "mov eax, edx";
        writeLine(line, 1);
        break;
    case wordType::OPERATOR4_add:
        line = "add eax, ebx";
        writeLine(line, 1);
        break;
    case wordType::OPERATOR4_sub:
        line = "sub eax, ebx";
        writeLine(line, 1);
        break;
    case wordType::OPERATOR5_left_shift:
        line = "shl eax, ebx";
        writeLine(line, 1);
        break;
    case wordType::OPERATOR5_right_shift:
        line = "shr eax, ebx";
        writeLine(line, 1);
        break;
    case wordType::OPERATOR6_less:
        line = "cmp eax, ebx";
        writeLine(line, 1);
        line = "mov eax, 0";
        writeLine(line, 1);
        line = "setl al";
        writeLine(line, 1);
        break;
    case wordType::OPERATOR6_less_equal:
        line = "cmp eax, ebx";
        writeLine(line, 1);
        line = "mov eax, 0";
        writeLine(line, 1);
        line = "setle al";
        writeLine(line, 1);
        break;
    case wordType::OPERATOR6_greater:
        line = "cmp eax, ebx";
        writeLine(line, 1);
        line = "mov eax, 0";
        writeLine(line, 1);
        line = "setg al";
        writeLine(line, 1);
        break;
    case wordType::OPERATOR6_greater_equal:
        line = "cmp eax, ebx";
        writeLine(line, 1);
        line = "mov eax, 0";
        writeLine(line, 1);
        line = "setge al";
        writeLine(line, 1);
        break;
    case wordType::OPERATOR7_equal:
        line = "cmp eax, ebx";
        writeLine(line, 1);
        line = "mov eax, 0";
        writeLine(line, 1);
        line = "sete al";
        writeLine(line, 1);
        break;
    case wordType::OPERATOR7_not_equal:
        line = "cmp eax, ebx";
        writeLine(line, 1);
        line = "mov eax, 0";
        writeLine(line, 1);
        line = "setne al";
        writeLine(line, 1);
        break;
    case wordType::OPERATOR8_logical_and:
        line = "and eax, ebx";
        writeLine(line, 1);
        break;
    case wordType::OPERATOR9_logical_xor:
        line = "xor eax, ebx";
        writeLine(line, 1);
        break;
    case wordType::OPERATOR10_logical_or:
        line = "or eax, ebx";
        writeLine(line, 1);
        break;
    }
}

void AssemblyGenerator::binaryCalculate(Token& left, Token& right, Token& op) {
    if (left.kind == wordType::CONST_INT && right.kind == wordType::CONST_INT) // 两者都是常数
    {
        std::string line = "mov eax, " + std::to_string(constBinaryCalculate(left, right, op));
        this->writeLine(line, 1);
    }
    else { // 其中有一个为标识符
        if (left.kind == wordType::IDENTIFIER && right.kind == wordType::IDENTIFIER)
        {
            std::string line = "mov eax, DWORD PTR [ebp-" + std::to_string(getEbpOffset(left)) + "]";
            this->writeLine(line, 1);
            line = "mov ebx, DWORD PTR [ebp-" + std::to_string(getEbpOffset(right)) + "]";
            this->writeLine(line, 1);
        }
        else if (left.kind == wordType::CONST_INT) {
            std::string line = "mov eax, " + std::to_string(left.code);
            this->writeLine(line, 1);
            line = "mov ebx, DWORD PTR [ebp-" + std::to_string(getEbpOffset(right)) + "]";
            this->writeLine(line, 1);
        }
        else {
            std::string line = "mov eax, DWORD PTR [ebp-" + std::to_string(getEbpOffset(left)) + "]";
            this->writeLine(line, 1);
            line = "mov ebx, " + std::to_string(right.code);
            this->writeLine(line, 1);
        }
        varBinaryCalculate(op);
    }
}