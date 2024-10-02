#include "parser.h"
#include <map>
#include "lex.h"

map<string, bool> defVar;
map<string, Token> SymTable;
static string char_len = "";
static int nestingLevel = 0;

namespace Parser {
    bool pushed_back = false;
    LexItem	pushed_token;

    static LexItem GetNextToken(istream& in, int& line) {
        if (pushed_back) {
            pushed_back = false;
            return pushed_token;
        }
        return getNextToken(in, line);
    }

    static void PushBackToken(LexItem& t) {
        if (pushed_back) {
            abort();
        }
        pushed_back = true;
        pushed_token = t;
    }

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)

{
    ++error_count;
    cout << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line);

//PrintStmt:= PRINT *, ExpreList 
bool PrintStmt(istream& in, int& line) {
    LexItem t;
    t = Parser::GetNextToken(in, line);
    if (t != DEF) {
        ParseError(line, "Print statement syntax error.");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t != COMMA) {
        ParseError(line, "Missing Comma.");
        return false;
    }
    bool e = ExprList(in, line);
    if (!e) {
        ParseError(line, "Missing expression after Print Statement");
        return false;
    }
    return e;
}//End of PrintStmt


//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
    bool status = false;
    status = Expr(in, line);
    if (!status) {
        ParseError(line, "Missing Expression");
        return false;
    }
    LexItem t = Parser::GetNextToken(in, line);

    if (t == COMMA) {
        status = ExprList(in, line);
    }
    else if (t.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << t.GetLexeme() << ")" << endl;
        return false;
    }
    else {
        Parser::PushBackToken(t);
        return true;
    }
    return status;
}//End of ExprList

bool Prog(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);
    if (t != PROGRAM) {
        ParseError(line, "Missing PROGRAM");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t != IDENT) {
        ParseError(line, "Missing IDENTIFIER");
        return false;
    }
    LexItem tok = Parser::GetNextToken(in, line);
    Parser::PushBackToken(tok);
    while (tok == INTEGER || tok == REAL || tok == CHARACTER) {
        Decl(in, line);
        if (tok == CHARACTER && char_len != "") {
            cout << "Definition of Strings with length of " << char_len << " in declaration statement." << endl;
            char_len = "";
        }
        tok = Parser::GetNextToken(in, line);
        Parser::PushBackToken(tok);
    }
    while (Stmt(in, line));

    t = Parser::GetNextToken(in, line);


    if (t != END) {
        ParseError(line, "Missing END of program");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != PROGRAM) {
        ParseError(line, "Missing end PROGRAM");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t != IDENT) {
        ParseError(line, "Missing end IDENT");
        return false;
    }
    cout << "(DONE)" << endl;
    return true;
}
bool Decl(istream& in, int& line) {
    if (!Type(in, line)) {
        return false;
    }
    LexItem t = Parser::GetNextToken(in, line);
    if (t != DCOLON) {
        Parser::PushBackToken(t);
        ParseError(line, "Missing DCOLON");
        return false;
    }
    return VarList(in, line);
}
bool Type(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);
    if (t != INTEGER && t != REAL && t != CHARACTER) {
        Parser::PushBackToken(t);
        return false;
    }
    if (t == CHARACTER) {
        t = Parser::GetNextToken(in, line);
        if (t == LPAREN) {
            t = Parser::GetNextToken(in, line);
            if (t != LEN) {
                return false;
            }
            t = Parser::GetNextToken(in, line);
            if (t != ASSOP) {
                return false;
            }
            t = Parser::GetNextToken(in, line);
            if (t != ICONST) {
                return false;
            }
            char_len = t.GetLexeme();
            t = Parser::GetNextToken(in, line);
            if (t != RPAREN) {
                return false;
            }
        }
        else {
            Parser::PushBackToken(t);
        }
    }
    return true;
}

bool VarList(istream& in, int& line) {
    if (!Var(in, line)) {
        return false;
    }
    LexItem v = Parser::GetNextToken(in, line);
    if (defVar.find(v.GetLexeme()) == defVar.end()) {
        defVar[v.GetLexeme()] = true;
    }
    else {
        ParseError(line, "Variable already declared");
        return false;
    }
    LexItem t = Parser::GetNextToken(in, line);
    if (t == ASSOP) {
        if (!Expr(in, line)) {
            return false;
        }
        cout << "Initialization of the variable " << v.GetLexeme() << " in the declaration statement." << endl;
        t = Parser::GetNextToken(in, line);
    }
    while (t == COMMA) {
        if (!Var(in, line)) {
            return false;
        }
        v = Parser::GetNextToken(in, line);
        if (defVar.find(v.GetLexeme()) == defVar.end()) {
            defVar[v.GetLexeme()] = true;
        }
        else {
            ParseError(line, "Variable already declared");
            return false;
        }
        t = Parser::GetNextToken(in, line);
        if (t == ASSOP) {
            if (!Expr(in, line)) {
                return false;
            }
            cout << "Initialization of the variable " << v.GetLexeme() << " in the declaration statement." << endl;
            t = Parser::GetNextToken(in, line);
        }
    }

    if (t != END && t != PROGRAM) {
        Parser::PushBackToken(t);
        return true;
    }
    else {
        Parser::PushBackToken(t);
        return false;
    }
}

bool Stmt(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);
    if (t == DCOLON) {
        t = Parser::GetNextToken(in, line);
    }
    if (t == IDENT) {
        Parser::PushBackToken(t);
        return AssignStmt(in, line);
    }
    else if (t == IF) {
        Parser::PushBackToken(t);
        return BlockIfStmt(in, line);
    }
    else if (t == PRINT) {
        return PrintStmt(in, line);
    }
    else if (t == ELSE || t == PROGRAM || t == END) {
        Parser::PushBackToken(t);
        return false;
    }
    else {
        Parser::PushBackToken(t);
        return SimpleIfStmt(in, line);
    }
}

bool SimpleStmt(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);
    if (t == IDENT) {
        Parser::PushBackToken(t);
        return AssignStmt(in, line);
    }
    else if (t == PRINT) {
        return PrintStmt(in, line);
    }
    else {
        return false;
    }
}

bool BlockIfStmt(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);
    if (t != IF) {
        Parser::PushBackToken(t);
        return false;
    }
    nestingLevel++;
    t = Parser::GetNextToken(in, line);
    if (t != LPAREN) {
        return false;
    }
    if (!RelExpr(in, line)) {
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != RPAREN) {
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != THEN) {
        if (t == PRINT) {
            cout << "Print statement in a Simple If statement." << endl;
        }
        Parser::PushBackToken(t);
        return SimpleStmt(in, line);
    }

    while (Stmt(in, line));

    t = Parser::GetNextToken(in, line);
    if (t != ELSE && t != END) {
        ParseError(line, "Missing END after IF block");
        return false;
    }
    if (t == ELSE) {
        while (Stmt(in, line));
        t = Parser::GetNextToken(in, line);
        if (t != END) {
            ParseError(line, "Missing END after IF block");
            return false;
        }

    }

    t = Parser::GetNextToken(in, line);
    if (t != IF) {
        ParseError(line, "Missing IF after END");
        return false;
    }
    cout << "End of Block If statement at nesting level " << nestingLevel << endl;
    nestingLevel--;
    return true;
}

bool SimpleIfStmt(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);
    if (t != IF) {
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != LPAREN) {
        return false;
    }

    if (!RelExpr(in, line)) {
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != RPAREN) {
        return false;
    }
    return SimpleStmt(in, line);
}

bool AssignStmt(istream& in, int& line) {
    if (!Var(in, line)) {
        return false;
    }

    LexItem t = Parser::GetNextToken(in, line);

    if (defVar.find(t.GetLexeme()) == defVar.end()) {
        ParseError(line, "Using Undefined Variable");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != ASSOP) {
        return false;
    }
    return Expr(in, line);
}

bool Var(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);
    if (t != IDENT) {
        ParseError(line, "Expected an identifier");
        return false;
    }
    Parser::PushBackToken(t);
    return true;
}

bool RelExpr(istream& in, int& line) {
    if (!Expr(in, line)) {
        ParseError(line, "Missing expression in relational expression");
        return false;
    }
    LexItem t = Parser::GetNextToken(in, line);
    if (t == EQ || t == LTHAN || t == GTHAN) {
        return Expr(in, line);
    }
    Parser::PushBackToken(t);
    return true;
}

bool Expr(istream& in, int& line) {
    if (!MultExpr(in, line)) {
        ParseError(line, "Invalid expression");
        return false;
    }

    LexItem t = Parser::GetNextToken(in, line);
    while (t == PLUS || t == MINUS || t == CAT) {
        if (!MultExpr(in, line)) {
            ParseError(line, "Invalid expression after arithmetic operator");
            return false;
        }
        t = Parser::GetNextToken(in, line);
    }
    Parser::PushBackToken(t);
    return true;
}

bool MultExpr(istream& in, int& line) {
    if (!TermExpr(in, line)) {
        ParseError(line, "Invalid term in multiplication/division expression");
        return false;
    }

    LexItem t = Parser::GetNextToken(in, line);
    while (t == MULT || t == DIV) {
        if (!TermExpr(in, line)) {
            ParseError(line, "Invalid term after multiplication/division operator");
            return false;
        }
        t = Parser::GetNextToken(in, line);
    }
    Parser::PushBackToken(t);
    return true;
}

bool TermExpr(istream& in, int& line) {
    if (!SFactor(in, line)) {
        ParseError(line, "Invalid factor in term expression");
        return false;
    }
    LexItem t = Parser::GetNextToken(in, line);
    while (t == POW) {
        if (!SFactor(in, line)) {
            ParseError(line, "Invalid factor after power operator");
            return false;
        }
        t = Parser::GetNextToken(in, line);
    }
    Parser::PushBackToken(t);
    return true;
}

bool SFactor(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);
    if (t == PLUS || t == MINUS) {
        return Factor(in, line, 1);
    }
    else {
        Parser::PushBackToken(t);
        return Factor(in, line, 1);
    }
}

bool Factor(istream& in, int& line, int sign) {
    LexItem t = Parser::GetNextToken(in, line);
    if (t == IDENT || t == ICONST || t == RCONST || t == SCONST) {
        if (t == IDENT) {
            if (!(defVar.find(t.GetLexeme()) == defVar.end())) {
                return true;
            }
            else {
                ParseError(line, "Using undefined variable");
                return false;
            }
        }
        else {
            return true;
        }

    }
    else if (t == LPAREN) {
        if (!Expr(in, line)) {
            return false;
        }
        t = Parser::GetNextToken(in, line);
        if (t != RPAREN) {
            cout << t << endl;
            ParseError(line, "Missing closing parenthesis ')'");
            return false;
        }
        return true;
    }
    else {
        cout << "in factor: " << t << endl;
        Parser::PushBackToken(t);
        ParseError(line, "Invalid factor");
        return false;
    }
}