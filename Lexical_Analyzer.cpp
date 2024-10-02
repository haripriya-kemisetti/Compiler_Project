#include <string>
#include <iostream>
#include <map>

LexItem getNextToken(istream& in, int& linenumber) {
    enum TokState { START, INID, ININT, ICONST, INSTRING, IDENT, RCONST } lexstate = START;
    string lexeme;
    char ch;

    while (in.get(ch) && !in.eof()) {
        switch (lexstate) {
        case START:
            if (isalpha(ch)) {
                lexstate = INID;
                lexeme += ch;
            }
            else if (isdigit(ch)) {
                lexstate = ININT;
                lexeme += ch;
            }
            else if (ch == '+') {
                return LexItem(PLUS, "+", linenumber);
            }
            else if (ch == '-') {
                return LexItem(MINUS, "-", linenumber);
            }
            else if (ch == '*') {
                return LexItem(MULT, "*", linenumber);
            }
            else if (ch == '/') {
                return LexItem(DIV, "/", linenumber);
            }
            else if (ch == '=') {
                return LexItem(ASSOP, "=", linenumber);
            }
            else if (ch == '>') {
                return LexItem(GTHAN, ">", linenumber);
            }
            else if (ch == '<') {
                return LexItem(LTHAN, "<", linenumber);
            }
            else if (ch == '|') {
                return LexItem(CAT, "|", linenumber);
            }
            else if (ch == '&') {
                return LexItem(CAT, "&", linenumber);
            }
            else if (ch == ',') {
                return LexItem(COMMA, ",", linenumber);
            }
            else if (ch == '(') {
                return LexItem(LPAREN, "(", linenumber);
            }
            else if (ch == ')') {
                return LexItem(RPAREN, ")", linenumber);
            }
            else if (ch == '.' && isdigit(in.peek())) {
                return LexItem(DOT, ".", linenumber);
            }
            else if (ch == ':') {
                if (in.peek() == ':') {
                    in.get(ch);
                    return LexItem(DCOLON, "::", linenumber);
                }
                else {
                    return LexItem(DEF, ":", linenumber);
                }
            }
            else if (ch == '\'' || ch == '\"') {
                lexstate = INSTRING;
            }
            else if (isspace(ch)) {
                continue;
            }
            else {
                return LexItem(ERR, string(1, ch), linenumber);
            }
            break;
        case INID:
            if (isalnum(ch)) {
                lexeme += ch;
            }
            else {
                return id_or_kw(lexeme, linenumber);
            }
            break;
        case ININT:
            if (isdigit(ch)) {
                lexeme += ch;
            }
            else if (ch == '.') {
                lexstate = RCONST;
                lexeme += ch;
            }
            else {
                return LexItem(ERR, lexeme, linenumber);
            }
            break;
        case RCONST:
            if (isdigit(ch)) {
                lexeme += ch;
            }
            else {
                return LexItem(ERR, lexeme, linenumber);
            }
            break;
        case INSTRING:
            if ((ch == '\'' || (ch == '"'))) {
                return LexItem(SCONST, lexeme, linenumber);
            }
            else if (isalnum(ch)) {
                lexeme += ch;
            }
            else {
                return LexItem(ERR, lexeme, linenumber);
            }
            break;
        default:
            return LexItem(ERR, lexeme, linenumber);
            break;
        }
    }
    return LexItem(DONE, "", linenumber);
}

extern LexItem id_or_kw(const string& lexeme, int linenum) {
    static const map<string, Token> k_map = {
        {"IF", IF}, {"ELSE", ELSE}, {"PRINT", PRINT}, {"INTEGER", INTEGER},
        {"REAL", REAL}, {"CHARACTER", CHARACTER}, {"END", END},
        {"THEN", THEN}, {"PROGRAM", PROGRAM}, {"LEN", LEN}
    };
    auto it = k_map.find(lexeme);
    if (it != k_map.end()) {
        return LexItem(it->second, lexeme, linenum);
    }

    return LexItem(IDENT, lexeme, linenum);
}
extern ostream& operator<<(ostream& out, const LexItem& tok) {
    switch (tok.GetToken()) {
    case ICONST:
        out << tok.GetToken() << tok.GetLexeme();
    case RCONST:
        out << tok.GetToken() << tok.GetLexeme();
    case BCONST:
        out << tok.GetToken() << "(" << tok.GetLexeme() << ")";
        break;
    case IDENT:
        out << tok.GetToken() << "'" << tok.GetLexeme() << "'";
        break;
    case SCONST:
        out << tok.GetToken() << "\"" << tok.GetLexeme() << "\"";
        break;
    default:
        out << tok.GetToken();
        break;
    }
    return out;
}

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>


using namespace std;

int main(int argc, char* argv[]) {
    string f[6] = { "-all","-int","-real","-str","-id""-kw" };
    if (argc < 2) {
        cerr << "NO SPECIFIED INPUT FILE." << endl;
        exit(1);
    }

    string filename = string(argv[1]);
    vector<string> flags;
    ifstream myInfile(filename);
    if (!myInfile) {
        cerr << "CANNOT OPEN THE FILE " << filename << endl;
        exit(1);
    }

    if (myInfile.peek() == EOF) {
        cerr << "Empty File." << endl;
        exit(1);
    }

    int line = 1;
    LexItem token;
    map<Token, vector<string>> tokenDirectories;
    int M = 0;
    while (token.GetToken() != DONE) {
        token = getNextToken(myInfile, line);
        string allFlag = "-all", IntegerFlag = "-int", RealFlag = "-real", StrFlag = "-str", IdFlag = "-id", KwFlag = "-kw", word = "", line = "";
        bool listI = false, listR = false, listS = false, listID = false, listK = false;
        map<char, int> integers, reals, strings, identifiers, keyWords;
        int L = 0, N = 0, O = 0, P = 0, Q = 0;
        M++;
        for (int i = 2; i < argc; i++) {
            if (argv[i] == IntegerFlag) {
                listI = true;
            }
            if (argv[i] == RealFlag) {
                listR = true;
            }
            if (argv[i] == StrFlag) {
                listS = true;
            }
            if (argv[i] == IdFlag) {
                listID = true;
            }
            if (argv[i] == KwFlag) {
                listK = true;
            }
            if (argv[i] == allFlag) {
                listR = true;
                listI = true;
                listS = true;
                listK = true;
                listID = true;
                break;
            }
        }

        while (getline(myInfile, line)) {
            L++;
            if (token == IDENT) {
                N++;
                M++;
            }
            if (token == ICONST) {
                O++;
                M++;
            }
            if (token == RCONST) {
                P++;
                M++;
            }
            if (token == SCONST) {
                Q++;
                M++;
            }
        }
        cout << "Lines: " << L << endl;
        cout << "Total Tokens: " << M << endl;
        cout << "Identifiers: " << N << endl;
        cout << "Integers: " << O << endl;
        cout << "Reals: " << P << endl;
        cout << "Strings: " << Q << endl;

        if (listI) {
            cout << "INTEGERS:" << endl;

            for (const auto& pair : integers) {
                cout << pair.first << "(" << pair.second << ") " << endl << endl;
            }

        }
        if (listR) {
            cout << "REALS:" << endl;

            for (const auto& pair : reals) {
                cout << pair.first << "(" << pair.second << ") " << endl << endl;
            }

        }
        if (listS) {
            cout << "STRINGS:" << endl;

            for (const auto& pair : strings) {
                cout << pair.first << "(" << pair.second << ") " << endl << endl;
            }

        }
        if (listID) {
            cout << "IDENTIFIERS:" << endl;

            for (const auto& pair : integers) {
                cout << pair.first << "(" << pair.second << ") " << endl << endl;
            }

        }
        if (listK) {
            cout << "KEYWORDS:" << endl;
            for (const auto& pair : integers) {
                cout << pair.first << "(" << pair.second << ") " << endl << endl;
            }
        }
        myInfile.close();

        return 0;
    }
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "NO SPECIFIED INPUT FILE." << endl;
        exit(1);
    }

    string filename = string(argv[1]);
    ifstream myInfile(filename);
    if (!myInfile) {
        cerr << "CANNOT OPEN THE FILE " << filename << endl;
        exit(1);
    }

    if (myInfile.peek() == EOF) {
        cerr << "Empty File." << endl;
        exit(1);
    }

    int line = 1;
    LexItem token;
    map<Token, int> tokenCounts;
    map<Token, vector<string>> tokenDirectories;
    map<string, int> identifiers;
    map<int, int> integers;
    map<double, int> reals;
    map<string, int> strings;

    while ((token = getNextToken(myInfile, line)).GetToken() != DONE) {
        tokenCounts[token.GetToken()]++;
        tokenDirectories[token.GetToken()].push_back(token.GetLexeme());

        if (token.GetToken() == IDENT) {
            identifiers[token.GetLexeme()]++;
        }
        else if (token.GetToken() == ICONST) {
            int value = stoi(token.GetLexeme());
            integers[value]++;
        }
        else if (token.GetToken() == RCONST) {
            double value = stod(token.GetLexeme());
            reals[value]++;
        }
        else if (token.GetToken() == SCONST) {
            strings[token.GetLexeme()]++;
        }
    }

    // Print token counts if corresponding flag is true
    
        cout << "Token Counts:" << endl;
        if (/* Your flag condition here */) {
            for (const auto& entry : tokenCounts) {
            cout << "Token: " << entry.first << ", Count: " << entry.second << endl;
        }
    }

    if (/* Your flag condition here */) {
        cout << "Token Directories:" << endl;
        for (const auto& entry : tokenDirectories) {
            cout << "Token: " << entry.first << endl;
            cout << "  Directories: ";
            for (const auto& lexeme : entry.second) {
                cout << lexeme << " ";
            }
            cout << endl;
        }
    }
    if (/* Your flag condition here */) {
        cout << "Identifier Counts:" << endl;
        for (const auto& entry : identifiers) {
            cout << "Identifier: " << entry.first << ", Count: " << entry.second << endl;
        }
    }

        cout << "Integer Counts:" << endl;
        if (IntegerFlag) {
        for (const auto& entry : integers) {
            cout << "Integer: " << entry.first << ", Count: " << entry.second << endl;
        }
    }

    if (/* Your flag condition here */) {
        cout << "Real Counts:" << endl;
        for (const auto& entry : reals) {
            cout << "Real: " << entry.first << ", Count: " << entry.second << endl;
        }
    }

    // Print string counts if corresponding flag is true
    if (/* Your flag condition here */) {
        cout << "String Counts:" << endl;
        for (const auto& entry : strings) {
            cout << "String: " << entry.first << ", Count: " << entry.second << endl;
        }
    }

    myInfile.close();
    return 0;
}
