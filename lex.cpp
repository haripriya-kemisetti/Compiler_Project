/*
 * lex.cpp
 * Lexical Analyzer for SPL Language
 * CS280 - Fall 2023
 */

#include <cctype>
#include <map>

using std::map;
using namespace std;

#include "lex.h"
//Keywords or reserved words mapping
LexItem id_or_kw(const string& lexeme, int linenum)
{
	string strlexeme = lexeme;
	map<string, Token> kwmap = {
		{ "print", PRINT},
		{ "if", IF },
		{ "else", ELSE },
		{ "then", THEN },
		{ "integer", INTEGER },
		{ "real", REAL },
		{ "character", CHARACTER },
		{ "end", END },
		{ "program", PROGRAM },
		{ "len", LEN},
	};
	Token tt;
	//convert the lexeme to all lower case letters

	//cout << "inside id_or_kw " << lexeme << endl;
	for (int i = 0; i < lexeme.length(); i++)
	{
		strlexeme[i] = tolower(strlexeme[i]);
	}
	tt = IDENT;
	auto kIt = kwmap.find(strlexeme);
	if (kIt != kwmap.end())
	{
		tt = kIt->second;
	}
	return LexItem(tt, lexeme, linenum);
}

map<Token, string> tokenPrint = {
		{PROGRAM, "PROGRAM" },
		{PRINT, "PRINT"},
		{ IF, "IF" },
		{ ELSE, "ELSE" },
		{ IDENT, "IDENT" },
		{ END, "END" },
		{ LEN, "LEN" },
		{ INTEGER, "INTEGER" },
		{ REAL, "REAL" },
		{ CHARACTER, "CHARACTER" },
		{ ICONST, "ICONST" },
		{ RCONST, "RCONST" },
		{ SCONST, "SCONST" },
		{ THEN, "THEN" },


		{ PLUS, "PLUS" },
		{ MINUS, "MINUS" },
		{ MULT, "MULT" },
		{ DIV, "DIV" },
		{ POW, "POW" },
		{ ASSOP, "ASSOP" },
		{ EQ, "EQ" },
		{ GTHAN, "GTHAN" },
		{ LTHAN, "LTHAN" },

		{ COMMA, "COMMA" },
		{ LPAREN, "LPAREN" },
		{ RPAREN, "RPAREN" },
		{ DOT, "DOT" },
		{ DCOLON, "DCOLON" },
		{ CAT, "CAT" },
		{ DEF, "DEF" },
		{ ERR, "ERR" },

		{ DONE, "DONE" },
};

ostream& operator<<(ostream& out, const LexItem& tok) {

	Token tt = tok.GetToken();
	out << tokenPrint[tt];
	if (tt == ICONST || tt == RCONST || tt == BCONST) {
		out << ": (" << tok.GetLexeme() << ")";
	}
	else if (tt == IDENT) {
		out << ": '" << tok.GetLexeme() << "'";
	}
	else if (tt == SCONST) {
		out << ": \"" << tok.GetLexeme() << "\"";
	}
	else if (tt == ERR) {
		out << ": {" << tok.GetLexeme() << "}";
	}
	return out;
}

LexItem getNextToken(istream& in, int& linenum)
{
	enum TokState { START, INID, INSTRI, INSTRII, ININT, INREAL, INCOMMENT, INLOGIC } lexstate = START;
	string lexeme;
	char ch, nextchar, nextch;
	Token tt;
	bool dec = false;

	//cout << "in getNestToken" << endl;
	while (in.get(ch)) {
		//cout << "in while " << ch << endl;
		switch (lexstate) {
		case START:
			if (ch == '\n')
			{
				linenum++;
				//cout << linenum << endl;
			}

			if (isspace(ch))
				continue;

			lexeme = ch;

			if (isalpha(ch)) {
				lexeme = ch;
				lexstate = INID;
				//cout << "in ID " << endl;
			}
			else if (ch == '\'') {
				lexstate = INSTRI;

			}
			else if (ch == '\"') {
				lexstate = INSTRII;
			}

			else if (isdigit(ch)) {
				lexstate = ININT;
			}
			else if (ch == '!') {
				//lexeme += ch;
				lexstate = INCOMMENT;
				//in.get(ch);
			}

			else {
				tt = ERR;
				switch (ch) {
				case '+':
					tt = PLUS;
					break;

				case '-':
					tt = MINUS;
					break;

				case '*':
					tt = MULT;
					nextchar = in.peek();
					if (nextchar == '*') {
						in.get(ch);
						lexeme += ch;
						tt = POW;
						break;
					}
					else if (nextchar == ',') {
						tt = DEF;
						break;
					}
					break;

				case '/':
					tt = DIV;
					nextchar = in.peek();
					if (nextchar == '/') {
						in.get(ch);
						lexeme += ch;
						tt = CAT;
						break;
					}
					break;

				case ':':
					//tt = COLON;
					nextchar = in.peek();
					if (nextchar == ':') {
						in.get(ch);
						lexeme += ch;
						tt = DCOLON;
						break;
					}
					//error
					break;

				case '=':
					tt = ASSOP;
					nextchar = in.peek();
					if (nextchar == '=') {
						in.get(ch);
						lexeme += ch;
						tt = EQ;
						break;
					}
					break;
				case '(':
					tt = LPAREN;
					break;
				case ')':
					tt = RPAREN;
					break;

				case ',':
					tt = COMMA;
					break;

				case '>':
					tt = GTHAN;
					break;

				case '<':
					tt = LTHAN;
					break;

				case '.':
					tt = DOT;
					nextchar = in.peek();
					if (isdigit(nextchar))
					{
						in.putback(ch);
						lexeme = "";
						lexstate = INREAL;
						continue;
					}
					else if (isalpha(nextchar))
					{
						lexstate = INLOGIC;
						continue;
					}
					break;

				}//end of inner switch
				return LexItem(tt, lexeme, linenum);
			}//end of else

			break;	//break out of START case

		case INID:
			if (isalpha(ch) || isdigit(ch) || ch == '_') {

				//cout << "in id continued" << ch << endl;
				lexeme += ch;
			}
			else {
				in.putback(ch);
				//cout << lexeme << endl;
				return id_or_kw(lexeme, linenum);

			}
			break;

		case INSTRI:

			if (ch == '\n') {
				return LexItem(ERR, lexeme, linenum);
			}
			lexeme += ch;
			if (ch == '\'') {
				lexeme = lexeme.substr(1, lexeme.length() - 2);
				return LexItem(SCONST, lexeme, linenum);
			}
			break;

		case INSTRII:

			if (ch == '\n') {
				return LexItem(ERR, lexeme, linenum);
			}
			lexeme += ch;
			if (ch == '\"') {
				lexeme = lexeme.substr(1, lexeme.length() - 2);
				return LexItem(SCONST, lexeme, linenum);
			}
			break;

		case ININT:
			if (isdigit(ch)) {
				lexeme += ch;
			}
			else if (ch == '.') {
				nextch = in.peek();
				if (isdigit(nextch))
				{
					in.putback(ch);//put back decimal point
					lexstate = INREAL;
				}
				else
				{
					in.putback(ch);
					return LexItem(ICONST, lexeme, linenum);
				}
				//in.putback(ch);
			}
			else {
				in.putback(ch);
				return LexItem(ICONST, lexeme, linenum);
			}
			break;

		case INREAL:

			if (ch == '.' && isdigit(in.peek()) && !dec) {
				lexeme += ch;
				dec = true;
			}
			else if (isdigit(ch) && dec) {
				lexeme += ch;
			}

			else if ((ch == '.') && dec && isdigit(in.peek())) {
				lexeme += ch;
				//cout << "error " << lexeme << endl;
				return LexItem(ERR, lexeme, linenum);
			}
			else if ((ch == '.') && dec && isalpha(in.peek())) {
				in.putback(ch);
				return LexItem(RCONST, lexeme, linenum);
			}
			else {
				in.putback(ch);
				//cout << "not error "  << lexeme << endl;
				//decimal = false;
				return LexItem(RCONST, lexeme, linenum);
			}

			break;

		case INLOGIC:
			if (isalpha(ch)) {

				//cout << "in id continued" << ch << endl;
				lexeme += ch;
			}
			else if (ch == '.') {
				//cout << lexeme << endl;
				lexeme += ch;
				return id_or_kw(lexeme, linenum);
			}
			else
			{
				return LexItem(ERR, "Error: Illegal Logical operator or constant.", linenum);
			}
			break;

		case INCOMMENT:
			if (ch == '\n') {
				lexstate = START;
				in.putback(ch);
			}

			break;
		}//end of switch
	}//end of while loop

	if (in.eof())
		return LexItem(DONE, "", linenum);

	return LexItem(ERR, "Error: Some strange symbol", linenum);
}





