# 简单C语言编译器

该编译器将一种语法类似C语言的语句序列翻译为等价的汇编程序，所输出的汇编程序符合X86汇编语言格式要求，可以在Linux环境下正常运行。

🔗[其他资源](https://github.com/wyt8/bit-cs)

## 👍最终支持的功能

* 关键字：`int`、`return`、`main`、`void`、`if`、`else`、`while`、`continue`、`break`
* 标识符：符合C89标准的标识符（`[A-Za-z_][0-9A-Za-z_]*`）
* 常量：十进制整型，如1、223、10等
* 赋值操作符：`=`
* 一元运算符：`-`、`!`、`~`
* 二元运算符：`+`、`-`、`*`、`/`、`%`、`<`、`<=`、`>`、`>=`、`==`、`!=`、`&`、`|`、`^`、`&&`、`||`
* 标点符号：`;`、`{`、`}`、`(`、`)`
* 语句
  * 变量声明：`int a, b=111, c=1+3;`
  * 表达式赋值语句：`a = (d+b&1)/(e!=3^b/c&d);`、`a = b+c;`
  * return语句：`return a+b;`、`return func(a);`
  * 函数调用：`println_int(a+b);`
  * 条件语句：`if (condition) {...}`、`if (condition) {...} else {...}`
  * 循环语句：`while (condition) {...}`
  * 循环控制语句：`continue;`、`break;`
* 函数定义：`int func() {...}`、`void func() {...}`、`int func(int a, int b) {...}`、`void func(int a, int b) {...}`
* 预置函数：在自定义函数外，还需支持对预置函数的调用
  * `println_int(int a)`：与C语言中`printf("%d\n", a)`有相同输出

## 🔍目录结构说明

* `使用Flex和Bison/`：使用Flex进行分词，使用Bison进行语法分析的版本，该版本支持lab1~lab4。

* `手工编写/`：手工编写的版本，该版本支持lab1~lab2。

* `实验需求/`：lab1~lab4的需求文档。

* `测试用例/`：lab2~lab4的公开测试用例。

## 🖼️结果展示

原始C语言代码：

```c
int factorial(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

int main() {
    int i = 1;
    while (i <= 5) {
        println_int(factorial(i));
        i = i + 1;
    }
    return 0;
}
```

生成的汇编代码：

```asm
.intel_syntax noprefix
.global main
.extern printf
.data
format_str:
    .asciz "%d\n"
.text
__func_factorial:
    push ebp
    mov ebp, esp
    sub esp, 0x200

    _L_ifcon_1:
        mov eax, DWORD PTR [ebp+8]
        push eax
        push 1
        pop ebx
        pop eax
        cmp eax, ebx
        setle al
        movzx eax, al
        push eax
        pop eax
        cmp eax, 0
        je _L_else_1

    _L_if_1:
        push 1
        pop eax
        leave
        ret

        jmp _L_ifend_1

    _L_else_1:
        mov eax, DWORD PTR [ebp+8]
        push eax
        mov eax, DWORD PTR [ebp+8]
        push eax
        push 1
        pop ebx
        pop eax
        sub eax, ebx
        push eax
        call __func_factorial
        add esp, 4
        push eax
        pop ebx
        pop eax
        imul eax, ebx
        push eax
        pop eax
        leave
        ret

    _L_ifend_1:

    leave
    ret


main:
    push ebp
    mov ebp, esp
    sub esp, 0x200

    push 1
    pop eax
    mov DWORD PTR [ebp-4], eax


    _L_whilecon_2:
        mov eax, DWORD PTR [ebp-4]
        push eax
        push 5
        pop ebx
        pop eax
        cmp eax, ebx
        setle al
        movzx eax, al
        push eax
        pop eax
        cmp eax, 0
        je _L_whileend_2

    _L_while_2:
        mov eax, DWORD PTR [ebp-4]
        push eax
        call __func_factorial
        add esp, 4
        push eax
        push offset format_str
        call printf
        add esp, 8
        push eax

        mov eax, DWORD PTR [ebp-4]
        push eax
        push 1
        pop ebx
        pop eax
        add eax, ebx
        push eax
        pop eax
        mov DWORD PTR [ebp-4], eax
        push eax

        jmp _L_whilecon_2

    _L_whileend_2:

    push 0
    pop eax
    leave
    ret

    leave
    ret
```

## 📒说明

对于Flex和Bison，这里提供一些参考资料：

* [使用Flex、Bison和LLVM编写自己的Toy Compiler](https://zhuanlan.zhihu.com/p/626085010)
* [自己动手写编译器](https://pandolia.net/tinyc/index.html)
