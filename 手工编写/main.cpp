#include <iostream>
#include <fstream>
#include <string>
#include <stack>

#include "global.h"
#include "LexicalAnalyzer.h"
#include "AssemblyGenerator.h"

using namespace std;

int main(int argc, char* argv[])
{
    string sourceFileName = argv[1];
    // string sourceFileName = "./input.txt";
    ifstream sourceFile(sourceFileName, ifstream::in);
    if (!sourceFile.is_open()) {
        cerr << "Դ�ļ��޷���" << endl;
        return 1;
    }
    
    LexicalAnalyzer la;
    // ��ȡ�ļ�����
    string line;
    while (getline(sourceFile, line)) {
        la.lexicalAnalysis(line);
    }
    sourceFile.close();

    //for (auto a : la.tokens) {
    //    cout << a.kind << "*" << a.code << endl;
    //}

    AssemblyGenerator ag(la.tokens, la.identifierTable);
    cout << ag.generate() << endl;

    system("pause");
    return 0;
}