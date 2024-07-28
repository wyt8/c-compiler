#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <vector>

struct IdentifierItem
{
    std::string name;
    ssize_t offset;
};

class IdentifierTable
{
private:
    std::vector<IdentifierItem> items;
    ssize_t ebpOffset = 0;

public:
    // 向table中添加元素，如果元素已经存在，则直接返回其下标
    size_t add(const std::string &name)
    {
        auto index = find(name);
        if (index == -1)
        {
            IdentifierItem item;
            item.name = std::string(name);
            ebpOffset -= 4;
            item.offset = ebpOffset;
            items.push_back(item);
        }

        return index;
    }

    // 向table中添加元素，如果元素已经存在，则直接返回其下标
    size_t add(const std::string &name, int offset)
    {
        auto index = find(name);
        if (index == -1)
        {
            IdentifierItem item;
            item.name = std::string(name);
            item.offset = offset;
            items.push_back(item);
        }

        return index;
    }

    ssize_t find(const std::string &name)
    {
        for (auto it = items.begin(); it != items.end(); it++)
        {
            if (it->name == name)
            {
                return std::distance(items.begin(), it);
            }
        }

        return -1;
    }

    IdentifierItem get(size_t index)
    {
        return items.at(index);
    }

    size_t size()
    {
        return items.size();
    }

    void clear()
    {
        items.clear();
        ebpOffset = 0;
    }
};

// C语言中的变量类型
enum CType
{
    INT,
    VOID
};

// C语言中的运算符
enum COperator
{
    ASIGN,  // =
    NEG,    // -
    NOT,    // !
    BITNOT, // ~
    CEQ,    // ==
    CNE,    // !=
    CLT,    // <
    CLE,    // <=
    CGT,    // >
    CGE,    // >=
    PLUS,   // +
    MINUS,  // -
    MUL,    // *
    DIV,    // /
    MOD,    // %
    AND,    // &&
    OR,     // ||
    BITAND, // &
    BITOR,  // |
    BITXOR, // ^
    RSHIFT, // >>
    LSHIFT, // <<
};

#endif