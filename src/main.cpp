#include <iostream>

#include "mdverifLexer.h"
#include "mdverifParser.h"
#include "verif_visitor.h"
#include "utils.h"

using namespace antlr4;
using namespace std;

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        cerr << "[error] Input sourcefile required:" << argv[0] << " <sourcefile>\n";
        return 1;
    }
    ifstream sourceFile;
    sourceFile.open(argv[1]);
    if (!sourceFile.good())
    {
        cout << "fail to open source file.";
        return 0;
    }

    // lexer & parser part of antlr4
    ANTLRInputStream input(sourceFile);
    mdverifLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    mdverifParser parser(&tokens);
    parser.setErrorHandler(make_shared<BailErrorStrategy>());

    mdverifParser::ProgramContext* root = parser.program();
    VerifVisitor visitor;
    try
    {
        visitor.visitProgram(root);
    } catch (VerificationError &e)
    {
        cout << e.what() << '\n';
        cout << "Verification failed.\n";
        return 0;
    } catch (SyntaxError &e)
    {
        cout << e.what() << '\n';
        cout << "Syntax error.\n";
        return 0;
    }
    cout << "Verified.\n";
    return 0;
}

