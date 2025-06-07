#ifndef __ASTNODES_H__
#define __ASTNODES_H__

#include <iostream>
#include <vector>
#include <stack>

#include <memory>
#include <string>

#include "global.h"

class NBlock;
class NStatement;
class NExpression;
class NVariableDeclaration;

typedef std::vector<std::shared_ptr<NStatement>> StatementList;
typedef std::vector<std::shared_ptr<NExpression>> ExpressionList;

// 所有节点的基类
class Node
{
protected:
    std::string funcNamePrefix = "__func_";
    std::string indent = "\t";
    std::string labelPrefix = "_L_";
    static int labelNo;
    static IdentifierTable identifierTable; // 标识符表
    static std::stack<int> labelStack;      // 标签栈，用于continue和break

    static ssize_t getEbpOffset(const std::string &name)
    {
        return identifierTable.get(identifierTable.find(name)).offset;
    }

public:
    Node() {}
    virtual ~Node() {}
    virtual std::string getTypeName() const = 0;
    virtual void genAsmCode(std::ostream &out, std::string &prefix) const {};
};

// 类型
class NType : public Node
{
public:
    CType type;

    NType() : type(CType::INT) {}

    NType(CType type) : type(type) {}

    std::string getTypeName() const override
    {
        return "NType";
    }
};

// 运算符
class NOperator : public Node
{
public:
    COperator op;

    NOperator(COperator op) : op(op) {}

    std::string getTypeName() const override
    {
        return "NOperator";
    }
};

// 表达式，整体有值，最后一条语句都是“push eax”
class NExpression : public Node
{
public:
    NExpression() {}

    std::string getTypeName() const override
    {
        return "NExpression";
    }
};

// 语句，整体没有值
class NStatement : public Node
{
public:
    NStatement() {}

    std::string getTypeName() const override
    {
        return "NStatement";
    }
};

// 整数
class NInteger : public NExpression
{
public:
    int value;

    NInteger() : value(0) {}

    NInteger(int value) : value(value) {}

    std::string getTypeName() const override
    {
        return "NInteger";
    }

    void genAsmCode(std::ostream &out, std::string &prefix) const override
    {
        out << prefix << "push " << value << std::endl;
    }
};

// 标识符
class NIdentifier : public NExpression
{
public:
    const std::string name;

    NIdentifier(const std::string &name) : name(name) {}

    std::string getTypeName() const override
    {
        return "NIdentifier";
    }

    void genAsmCode(std::ostream &out, std::string &prefix) const override
    {
        if (getEbpOffset(name) > 0)
            out << prefix << "mov eax, DWORD PTR [ebp+" << getEbpOffset(name) << "]" << std::endl;
        else
            out << prefix << "mov eax, DWORD PTR [ebp-" << -getEbpOffset(name) << "]" << std::endl;
        out << prefix << "push eax" << std::endl;
    }
};

// 函数调用表达式
class NMethodCall : public NExpression
{
public:
    const std::shared_ptr<NIdentifier> id;
    std::shared_ptr<ExpressionList> arguments = std::make_shared<ExpressionList>();

    NMethodCall(const std::shared_ptr<NIdentifier> id, std::shared_ptr<ExpressionList> arguments)
        : id(id), arguments(arguments) {}

    NMethodCall(const std::shared_ptr<NIdentifier> id)
        : id(id) {}

    std::string getTypeName() const override
    {
        return "NMethodCall";
    }

    void genAsmCode(std::ostream &out, std::string &prefix) const override
    {
        // 函数参数倒着入栈
        for (auto it = arguments->rbegin(); it != arguments->rend(); it++)
        {
            (*it)->genAsmCode(out, prefix);
        }
        if (id->name == "println_int")
        {
            out << prefix << "push offset format_str" << std::endl;
            out << prefix << "call printf" << std::endl;
            out << prefix << "add esp, 8" << std::endl;
        }
        else
        {
            out << prefix << "call " << funcNamePrefix << id->name << std::endl;
            out << prefix << "add esp, " << arguments->size() * 4 << std::endl;
        }
        out << prefix << "push eax" << std::endl;
    }
};

// 二元运算符
class NBinaryOperatorExpression : public NExpression
{
public:
    std::shared_ptr<NOperator> op;
    std::shared_ptr<NExpression> left;
    std::shared_ptr<NExpression> right;

    NBinaryOperatorExpression(std::shared_ptr<NExpression> left, std::shared_ptr<NOperator> op, std::shared_ptr<NExpression> right)
        : left(left), right(right), op(op) {}

    std::string getTypeName() const override
    {
        return "NBinaryOperator";
    }

    void genAsmCode(std::ostream &out, std::string &prefix) const override
    {
        left->genAsmCode(out, prefix);
        right->genAsmCode(out, prefix);
        out << prefix << "pop ebx" << std::endl;
        out << prefix << "pop eax" << std::endl;

        switch (op->op)
        {
        case COperator::PLUS:
            out << prefix << "add eax, ebx" << std::endl;
            break;
        case COperator::MINUS:
            out << prefix << "sub eax, ebx" << std::endl;
            break;
        case COperator::MUL:
            out << prefix << "imul eax, ebx" << std::endl;
            break;
        case COperator::DIV:
            out << prefix << "cdq" << std::endl;
            out << prefix << "idiv ebx" << std::endl;
            break;
        case COperator::MOD:
            out << prefix << "cdq" << std::endl;
            out << prefix << "idiv ebx" << std::endl;
            out << prefix << "mov eax, edx" << std::endl;
            break;
        case COperator::CEQ:
            out << prefix << "cmp eax, ebx" << std::endl;
            out << prefix << "sete al" << std::endl;
            out << prefix << "movzx eax, al" << std::endl;
            break;
        case COperator::CNE:
            out << prefix << "cmp eax, ebx" << std::endl;
            out << prefix << "setne al" << std::endl;
            out << prefix << "movzx eax, al" << std::endl;
            break;
        case COperator::CGE:
            out << prefix << "cmp eax, ebx" << std::endl;
            out << prefix << "setge al" << std::endl;
            out << prefix << "movzx eax, al" << std::endl;
            break;
        case COperator::CGT:
            out << prefix << "cmp eax, ebx" << std::endl;
            out << prefix << "setg al" << std::endl;
            out << prefix << "movzx eax, al" << std::endl;
            break;
        case COperator::CLT:
            out << prefix << "cmp eax, ebx" << std::endl;
            out << prefix << "setl al" << std::endl;
            out << prefix << "movzx eax, al" << std::endl;
            break;
        case COperator::CLE:
            out << prefix << "cmp eax, ebx" << std::endl;
            out << prefix << "setle al" << std::endl;
            out << prefix << "movzx eax, al" << std::endl;
            break;
        case COperator::BITAND:
            out << prefix << "and eax, ebx" << std::endl;
            break;
        case COperator::BITOR:
            out << prefix << "or eax, ebx" << std::endl;
            break;
        case COperator::BITXOR:
            out << prefix << "xor eax, ebx" << std::endl;
            break;
        case COperator::AND:
            out << prefix << "test eax, eax" << std::endl;
            out << prefix << "setne al" << std::endl;
            out << prefix << "movzx eax, al" << std::endl;
            out << prefix << "test ebx, ebx" << std::endl;
            out << prefix << "setne bl" << std::endl;
            out << prefix << "movzx ebx, bl" << std::endl;
            out << prefix << "and eax, ebx" << std::endl;
            break;
        case COperator::OR:
            out << prefix << "test eax, eax" << std::endl;
            out << prefix << "setne al" << std::endl;
            out << prefix << "movzx eax, al" << std::endl;
            out << prefix << "test ebx, ebx" << std::endl;
            out << prefix << "setne bl" << std::endl;
            out << prefix << "movzx ebx, bl" << std::endl;
            out << prefix << "or eax, ebx" << std::endl;
            break;
        default:
            std::cerr << "[ERROR] Unknown operator: " << op->op << std::endl;
            break;
        }
        out << prefix << "push eax" << std::endl;
    }
};

// 一元运算符
class NUnaryOperatorExpression : public NExpression
{
public:
    std::shared_ptr<NOperator> op;
    std::shared_ptr<NExpression> operand;

    NUnaryOperatorExpression(std::shared_ptr<NExpression> operand, std::shared_ptr<NOperator> op)
        : operand(operand), op(op) {}

    std::string getTypeName() const override
    {
        return "NUnaryOperator";
    }

    void genAsmCode(std::ostream &out, std::string &prefix) const override
    {
        operand->genAsmCode(out, prefix);
        out << prefix << "pop eax" << std::endl;
        switch (op->op)
        {
        case COperator::NEG:
            out << prefix << "neg eax" << std::endl;
            break;
        case COperator::BITNOT:
            out << prefix << "not eax" << std::endl;
            break;
        case COperator::NOT:
            out << prefix << "cmp eax, 0" << std::endl;
            out << prefix << "sete al" << std::endl;
            out << prefix << "movzx eax, al" << std::endl;
            break;
        default:
            std::cerr << "[ERROR] Unknown operator: " << op->op << std::endl;
            break;
        }
        out << prefix << "push eax" << std::endl;
    }
};

// 赋值表达式
class NAssignment : public NExpression
{
public:
    std::shared_ptr<NIdentifier> left;
    std::shared_ptr<NExpression> right;

    NAssignment(std::shared_ptr<NIdentifier> left, std::shared_ptr<NExpression> right)
        : left(left), right(right) {}

    std::string getTypeName() const override
    {
        return "NAssignment";
    }

    void genAsmCode(std::ostream &out, std::string &prefix) const override
    {
        right->genAsmCode(out, prefix);
        out << prefix << "pop eax" << std::endl;
        if (getEbpOffset(left->name) > 0)
            out << prefix << "mov DWORD PTR [ebp+" << getEbpOffset(left->name) << "], eax" << std::endl;
        else
            out << prefix << "mov DWORD PTR [ebp-" << -getEbpOffset(left->name) << "], eax" << std::endl;
        out << prefix << "push eax" << std::endl;
    }
};

// 语句块
class NBlock : public NStatement
{
public:
    std::shared_ptr<StatementList> statements = std::make_shared<StatementList>();

    NBlock() {}

    std::string getTypeName() const override
    {
        return "NBlock";
    }

    void genAsmCode(std::ostream &out, std::string &prefix) const override
    {
        for (auto it = statements->begin(); it != statements->end(); it++)
        {
            (*it)->genAsmCode(out, prefix);
            out << std::endl;
        }
    }
};

// 表达式语句
class NExpressionStatement : public NStatement
{
public:
    std::shared_ptr<NExpression> expression;

    NExpressionStatement() {}

    NExpressionStatement(std::shared_ptr<NExpression> expression)
        : expression(expression) {}

    std::string getTypeName() const override
    {
        return "NExpressionStatement";
    }

    void genAsmCode(std::ostream &out, std::string &prefix) const override
    {
        expression->genAsmCode(out, prefix);
    }
};

// 单个变量声明语句
class NVariableDeclarationInner : public NStatement
{
public:
    const std::shared_ptr<NType> type;
    std::shared_ptr<NIdentifier> id;
    std::shared_ptr<NExpression> assignmentExpr;

    NVariableDeclarationInner() {}

    NVariableDeclarationInner(const std::shared_ptr<NType> type, std::shared_ptr<NIdentifier> id, std::shared_ptr<NExpression> assignmentExpr = nullptr)
        : type(type), id(id), assignmentExpr(assignmentExpr) {}

    std::string getTypeName() const override
    {
        return "NVariableDeclarationInner";
    }

    void genAsmCode(std::ostream &out, std::string &prefix) const override
    {
        identifierTable.add(id->name);

        if (assignmentExpr != nullptr)
        {
            assignmentExpr->genAsmCode(out, prefix);
            out << prefix << "pop eax" << std::endl;
            if (getEbpOffset(id->name) > 0)
                out << prefix << "mov DWORD PTR [ebp+" << getEbpOffset(id->name) << "], eax" << std::endl;
            else
                out << prefix << "mov DWORD PTR [ebp-" << -getEbpOffset(id->name) << "], eax" << std::endl;
        }
    }
};

// 变量声明语句，其中可能包含多个变量声明
class NVariableDeclaration : public NStatement
{
public:
    std::shared_ptr<NType> defaultType = std::shared_ptr<NType>(new NType(CType::INT));
    std::vector<std::shared_ptr<NVariableDeclarationInner>> variableDeclarationList;

    NVariableDeclaration() {}

    NVariableDeclaration(std::shared_ptr<NType> defaultType) : defaultType(defaultType) {}

    void addItem(std::shared_ptr<NIdentifier> id, const std::shared_ptr<NType> type = nullptr, std::shared_ptr<NExpression> assignmentExpr = nullptr)
    {
        if (type == nullptr)
        {
            auto item = std::shared_ptr<NVariableDeclarationInner>(new NVariableDeclarationInner(defaultType, id, assignmentExpr));
            variableDeclarationList.push_back(item);
        }
        else
        {
            auto item = std::shared_ptr<NVariableDeclarationInner>(new NVariableDeclarationInner(type, id, assignmentExpr));
            variableDeclarationList.push_back(item);
        }
    }

    std::string getTypeName() const override
    {
        return "NVariableDeclaration";
    }

    void genAsmCode(std::ostream &out, std::string &prefix) const override
    {
        for (auto it = variableDeclarationList.begin(); it != variableDeclarationList.end(); it++)
        {
            (*it)->genAsmCode(out, prefix);
        }
        out << std::endl;
    }
};

// 函数定义语句
class NFunctionDefine : public NStatement
{
public:
    std::shared_ptr<NType> type;
    std::shared_ptr<NIdentifier> id;
    std::shared_ptr<NVariableDeclaration> arguments;
    std::shared_ptr<NBlock> block;

    NFunctionDefine(std::shared_ptr<NType> type, std::shared_ptr<NIdentifier> id, std::shared_ptr<NVariableDeclaration> arguments, std::shared_ptr<NBlock> block)
        : type(type), id(id), arguments(arguments), block(block) {}

    std::string getTypeName() const override
    {
        return "NFunctionDefine";
    }

    void genAsmCode(std::ostream &out, std::string &prefix) const override
    {
        identifierTable.clear();

        if (id->name == "main")
        {
            out << prefix << id->name << ":" << std::endl;
        }
        else
        {
            out << prefix << funcNamePrefix << id->name << ":" << std::endl;
        }

        auto nextPrefix = prefix + indent;

        out << nextPrefix << "push ebp" << std::endl;
        out << nextPrefix << "mov ebp, esp" << std::endl;
        out << nextPrefix << "sub esp, 0x200" << std::endl; // 预留变量声明和定义所存放的空间

        // 函数参数存储在标识符表中
        for (auto i = 0; i < arguments->variableDeclarationList.size(); i++)
        {
            identifierTable.add(arguments->variableDeclarationList[i]->id->name, (i + 2) * 4);
            // out << nextPrefix << "mov eax, DWORD PTR [ebp+" << (i + 2) * 4 << "]" << std::endl;
            // out << nextPrefix << "mov DWORD PTR [ebp-" << getEbpOffset((arguments->variableDeclarationList[i])->id->name) << "], eax" << std::endl;
        }
        out << std::endl;

        block->genAsmCode(out, nextPrefix);
        out << nextPrefix << "leave" << std::endl;
        out << nextPrefix << "ret" << std::endl;
        out << std::endl;
    }
};

// 返回语句
class NReturnStatement : public NStatement
{
public:
    std::shared_ptr<NExpression> expression;

    NReturnStatement(std::shared_ptr<NExpression> expression) : expression(expression) {}

    std::string getTypeName() const override
    {
        return "NReturnStatement";
    }

    void genAsmCode(std::ostream &out, std::string &prefix) const override
    {
        expression->genAsmCode(out, prefix);
        out << prefix << "pop eax" << std::endl;
        out << prefix << "leave" << std::endl;
        out << prefix << "ret" << std::endl;
    }
};

// continue语句
class NContinueStatement : public NStatement
{
public:
    NContinueStatement() {}

    std::string getTypeName() const override
    {
        return "NContinueStatement";
    }

    void genAsmCode(std::ostream &out, std::string &prefix) const override
    {
        out << prefix << "jmp " << labelPrefix << "whilecon_" << labelStack.top() << std::endl;
    }
};

// break语句
class NBreakStatement : public NStatement
{
public:
    NBreakStatement() {}

    std::string getTypeName() const override
    {
        return "NBreakStatement";
    }

    void genAsmCode(std::ostream &out, std::string &prefix) const override
    {
        out << prefix << "jmp " << labelPrefix << "whileend_" << labelStack.top() << std::endl;
    }
};

// if语句
class NIfStatement : public NStatement
{
public:
    std::shared_ptr<NExpression> condition;
    std::shared_ptr<NBlock> ifBlock;
    std::shared_ptr<NBlock> elseBlock;

    NIfStatement(std::shared_ptr<NExpression> condition, std::shared_ptr<NBlock> ifBlock, std::shared_ptr<NBlock> elseBlock = nullptr)
        : condition(condition), ifBlock(ifBlock), elseBlock(elseBlock) {}

    std::string getTypeName() const override
    {
        return "NIfStatement";
    }

    void genAsmCode(std::ostream &out, std::string &prefix) const override
    {
        labelNo++;
        int tempLabelNo = labelNo;
        auto nextPrefix = prefix + indent;
        out << prefix << labelPrefix << "ifcon_" << tempLabelNo << ":" << std::endl;
        condition->genAsmCode(out, nextPrefix);
        out << nextPrefix << "pop eax" << std::endl;
        out << nextPrefix << "cmp eax, 0" << std::endl;
        if (elseBlock != nullptr)
        {
            out << nextPrefix << "je " << labelPrefix << "else_" << tempLabelNo << std::endl;
        }
        else
        {
            out << nextPrefix << "je " << labelPrefix << "ifend_" << tempLabelNo << std::endl;
        }
        out << std::endl;

        out << prefix << labelPrefix << "if_" << tempLabelNo << ":" << std::endl;
        ifBlock->genAsmCode(out, nextPrefix);
        out << nextPrefix << "jmp " << labelPrefix << "ifend_" << tempLabelNo << std::endl;
        out << std::endl;
        
        if (elseBlock != nullptr)
        {
            out << prefix << labelPrefix << "else_" << tempLabelNo << ":" << std::endl;
            elseBlock->genAsmCode(out, nextPrefix);
        }
        out << prefix << labelPrefix << "ifend_" << tempLabelNo << ":" << std::endl;
    }
};

// while语句
class NWhileStatement : public NStatement
{
public:
    std::shared_ptr<NExpression> condition;
    std::shared_ptr<NBlock> block;

    NWhileStatement(std::shared_ptr<NExpression> condition, std::shared_ptr<NBlock> block)
        : condition(condition), block(block) {}

    std::string getTypeName() const override
    {
        return "NWhileStatement";
    }

    void genAsmCode(std::ostream &out, std::string &prefix) const override
    {
        labelNo++;
        labelStack.push(labelNo);
        auto nextPrefix = prefix + indent;
        out << prefix << labelPrefix << "whilecon_" << labelStack.top() << ":" << std::endl;
        condition->genAsmCode(out, nextPrefix);
        out << nextPrefix << "pop eax" << std::endl;
        out << nextPrefix << "cmp eax, 0" << std::endl;
        out << nextPrefix << "je " << labelPrefix << "whileend_" << labelStack.top() << std::endl;
        out << std::endl;
        out << prefix << labelPrefix << "while_" << labelStack.top() << ":" << std::endl;
        block->genAsmCode(out, nextPrefix);
        out << nextPrefix << "jmp " << labelPrefix << "whilecon_" << labelStack.top() << std::endl;
        out << std::endl;
        out << prefix << labelPrefix << "whileend_" << labelStack.top() << ":" << std::endl;
        labelStack.pop();
    }
};

#endif
