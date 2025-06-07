#pragma once
#include <vector>
#include <algorithm>
#include <string>

// 词类型
enum wordType {
    KEYWORD_int,                // int关键字
    KEYWORD_return,             // return关键字
    IDENTIFIER_main,            // main标识符
    IDENTIFIER_println_int,     // 标识符println_int  
    IDENTIFIER,                 // 标识符
    CONST_INT,                  // 整型常量
    OPERATOR1_left_parentheses, // 优先级为1的运算符    (
    OPERATOR1_right_parentheses,// 优先级为1的运算符    )
    OPERATOR3_div,              // 优先级为3的运算符    /
    OPERATOR3_mul,              // 优先级为3的运算符    *
    OPERATOR3_mod,              // 优先级为3的运算符    %
    OPERATOR4_add,              // 优先级为4的运算符    +
    OPERATOR4_sub,              // 优先级为4的运算符    -
    OPERATOR5_left_shift,       // 优先级为5的运算符    <<
    OPERATOR5_right_shift,      // 优先级为5的运算符    >>
    OPERATOR6_less,             // 优先级为6的运算符    <
    OPERATOR6_less_equal,       // 优先级为6的运算符    <=
    OPERATOR6_greater,          // 优先级为6的运算符    >
    OPERATOR6_greater_equal,    // 优先级为6的运算符    >=
    OPERATOR7_equal,            // 优先级为7的运算符    ==
    OPERATOR7_not_equal,        // 优先级为7的运算符    !=
    OPERATOR8_logical_and,      // 优先级为8的运算符    &
    OPERATOR9_logical_xor,      // 优先级为9的运算符    ^
    OPERATOR10_logical_or,      // 优先级为10的运算符   |
    OPERATOR14_assignment,      // 优先级为14的运算符   =
    SEPARATOR,                  // 分隔符               ;
    BRACE_LEFT,                 // 左大括号             {
    BRACE_RIGHT                 // 右大括号             }
};

// 属性字
struct Token {
    wordType kind;
    int code;
};

template<typename T>
class Table {
public:
    // 向table中添加元素，如果元素已经存在，则直接返回其下标
    size_t add(T& item) {
        size_t index;
        auto it = std::find(table.begin(), table.end(), item);
        if (it == table.end()) {
            index = table.size();
            table.push_back(item);
        }
        else {
            index = std::distance(table.begin(), it);
        }
        return index;
    }

    T get(size_t index) {
        return table.at(index);
    }

    size_t size() {
        return table.size();
    }
private:
    std::vector<T> table;
};
