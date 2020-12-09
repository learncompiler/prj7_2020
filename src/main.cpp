#include <iostream>

#include "mdverifLexer.h"
#include "mdverifParser.h"
#include "verif_visitor.h"

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

    mdverifParser::ProgramContext* root = parser.program();
    VerifVisitor visitor;
    visitor.visitProgram(root);
    
    cout << "Verified.\n";
    return 0;
}

