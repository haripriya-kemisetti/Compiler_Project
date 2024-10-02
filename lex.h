/*
 * lex.h
 *
 * CS280
 * Spring 2024
*/

#ifndef LEX_H_
#define LEX_H_

#include <string>
#include <iostream>
#include <map>
using namespace std;


//Definition of all the possible token types
enum Token {
	// keywords OR RESERVED WORDS
	IF, ELSE, PRINT, INTEGER, REAL,
	CHARACTER, END, THEN, PROGRAM,
	TRUE, FALSE, LEN,
	// identifiers
	IDENT,
	// an integer, real, logical and string constants
	ICONST, RCONST, SCONST, BCONST,

	// the arithmetic operators, logic operators, relational operators
	PLUS, MINUS, MULT, DIV, ASSOP, EQ, POW,
	GTHAN, LTHAN, CAT,
	//Delimiters
	COMMA, LPAREN, RPAREN, DOT, DCOLON, DEF,
	// any error returns this token
	ERR,
	// when completed (EOF), return this token
	DONE,
};


//Class definition of LexItem
class LexItem {
	Token	token;
	string	lexeme;
	int	lnum;

public:
	LexItem() {
		token = ERR;
		lnum = -1;
	}
	LexItem(Token token, string lexeme, int line) {
		this->token = token;
		this->lexeme = lexeme;
		this->lnum = line;
	}

	bool operator==(const Token token) const { return this->token == token; }
	bool operator!=(const Token token) const { return this->token != token; }

	Token	GetToken() const { return token; }
	string	GetLexeme() const { return lexeme; }
	int	GetLinenum() const { return lnum; }
};



extern ostream& operator<<(ostream& out, const LexItem& tok);
extern LexItem id_or_kw(const string& lexeme, int linenum);
extern LexItem getNextToken(istream& in, int& linenum);


#endif /* LEX_H_ */
