#include <iostream>

#include "meverifLexer.h"
#include "meverifParser.h"
#include "CodeGenVisitor.h"

using namespace antlr4;
using namespace std;

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        std::cerr << "[error] Input sourcefile required:" << argv[0] << " <sourcefile>\n";
        return 1;
    }
    ifstream sourceFile;
    sourceFile.open(argv[1]);

    // lexer & parser part of antlr4
    ANTLRInputStream input(sourceFile);
    mdverifLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    mdverifParser parser(&tokens);
    parser.setErrorHandler(make_shared<BailErrorStrategy>());

    // customized pass: allocator, typer, codegen and etc.

    mdverifParser::ProgramContext* root = parser.program();
    VerifVisitor visitor;
    visitor.visitProgram(root);
    
    cout << "Verified.\n";
    return 0;
}

