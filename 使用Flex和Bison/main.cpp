#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <cstdlib>
#include <cstdio>

#include "global.h"
#include "ASTNodes.h"

using namespace std;

extern shared_ptr<NBlock> programBlock;

extern int yyparse();
extern FILE *yyin;

int main(int argc, char *argv[])
{
    string sourceFileName = argv[1];
    // string sourceFileName = "./input.txt";
    if ((yyin = fopen(sourceFileName.c_str(), "r")) == nullptr)
    {
        cerr << "源文件无法打开" << endl;
        return 1;
    }

    // 构建语法树
    if (yyparse()==1){
        cerr << "Parser Error!" << endl;
        return -1;
    }

    cout << ".intel_syntax noprefix" << endl;
    cout << ".global main" << endl;
    cout << ".extern printf" << endl;
    cout << ".data" << endl;
    cout << "format_str:" << endl;
    cout << "\t.asciz \"%d\\n\"" << endl;
    cout << ".text" << endl;

    ostream &out = cout;
    string prefix = "";
    programBlock->genAsmCode(out, prefix);

    fclose(yyin);
    system("pause");
    return 0;
}
