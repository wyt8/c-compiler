#pragma once
#include <vector>
#include <algorithm>
#include <string>

// ������
enum wordType {
    KEYWORD_int,                // int�ؼ���
    KEYWORD_return,             // return�ؼ���
    IDENTIFIER_main,            // main��ʶ��
    IDENTIFIER_println_int,     // ��ʶ��println_int  
    IDENTIFIER,                 // ��ʶ��
    CONST_INT,                  // ���ͳ���
    OPERATOR1_left_parentheses, // ���ȼ�Ϊ1�������    (
    OPERATOR1_right_parentheses,// ���ȼ�Ϊ1�������    )
    OPERATOR3_div,              // ���ȼ�Ϊ3�������    /
    OPERATOR3_mul,              // ���ȼ�Ϊ3�������    *
    OPERATOR3_mod,              // ���ȼ�Ϊ3�������    %
    OPERATOR4_add,              // ���ȼ�Ϊ4�������    +
    OPERATOR4_sub,              // ���ȼ�Ϊ4�������    -
    OPERATOR5_left_shift,       // ���ȼ�Ϊ5�������    <<
    OPERATOR5_right_shift,      // ���ȼ�Ϊ5�������    >>
    OPERATOR6_less,             // ���ȼ�Ϊ6�������    <
    OPERATOR6_less_equal,       // ���ȼ�Ϊ6�������    <=
    OPERATOR6_greater,          // ���ȼ�Ϊ6�������    >
    OPERATOR6_greater_equal,    // ���ȼ�Ϊ6�������    >=
    OPERATOR7_equal,            // ���ȼ�Ϊ7�������    ==
    OPERATOR7_not_equal,        // ���ȼ�Ϊ7�������    !=
    OPERATOR8_logical_and,      // ���ȼ�Ϊ8�������    &
    OPERATOR9_logical_xor,      // ���ȼ�Ϊ9�������    ^
    OPERATOR10_logical_or,      // ���ȼ�Ϊ10�������   |
    OPERATOR14_assignment,      // ���ȼ�Ϊ14�������   =
    SEPARATOR,                  // �ָ���               ;
    BRACE_LEFT,                 // �������             {
    BRACE_RIGHT                 // �Ҵ�����             }
};

// ������
struct Token {
    wordType kind;
    int code;
};

template<typename T>
class Table {
public:
    // ��table�����Ԫ�أ����Ԫ���Ѿ����ڣ���ֱ�ӷ������±�
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
