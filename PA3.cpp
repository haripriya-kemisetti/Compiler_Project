/* Implementation of Interpreter for the SFort95 Language
 * parserInterp.cpp
 * Programming Assignment 3
 * Spring 2024
*/
#include <stack>

#include "parserInterp.h"

map<string, bool> defVar;
map<string, Token> SymTable;

map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants
queue <Value>* ValQue; //declare a pointer variable to a queue of Value objects

namespace Parser {
	bool pushed_back = false;
	LexItem pushed_token;

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
	ValQue = new queue<Value>;
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
	bool ex = ExprList(in, line);

	if (!ex) {
		ParseError(line, "Missing expression after Print Statement");
		return false;
	}

	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	cout << endl;
	return ex;
}//End of PrintStmt

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	Value retVal;

	status = Expr(in, line, retVal);
	if (!status) {
		ParseError(line, "Missing Expression");
		return false;
	}
	ValQue->push(retVal);
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == COMMA) {

		status = ExprList(in, line);

	}
	else if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		//cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else {
		Parser::PushBackToken(tok);
		return true;
	}
	//for(const auto &pair: TempsResults){
	//   cout<< "Key: " << pair.first << ", Value: " << pair.second << endl;
	//}
	return status;
}//End of ExprList

bool Prog(istream& in, int& line) {
	bool dl = false, sl = false;
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			dl = Decl(in, line);
			if (!dl)
			{
				ParseError(line, "Incorrect Declaration in Program");
				return false;
			}
			sl = Stmt(in, line);
			if (!sl)
			{
				ParseError(line, "Incorrect Statement in program");
				return false;
			}
			tok = Parser::GetNextToken(in, line);

			if (tok.GetToken() == END) {
				tok = Parser::GetNextToken(in, line);

				if (tok.GetToken() == PROGRAM) {
					tok = Parser::GetNextToken(in, line);

					if (tok.GetToken() == IDENT) {
						cout << "(DONE)" << endl;
						return true;
					}
					else
					{
						ParseError(line, "Missing Program Name");
						return false;
					}
				}
				else
				{
					ParseError(line, "Missing PROGRAM at the End");
					return false;
				}
			}
			else
			{
				ParseError(line, "Missing END of Program");
				return false;
			}
		}
		else
		{
			ParseError(line, "Missing Program name");
			return false;
		}
	}
	else if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		//cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	else
	{
		ParseError(line, "Missing Program keyword");
		return false;
	}
}

bool Decl(istream& in, int& line) {
	bool status = false;
	LexItem tok;
	string strLen;
	Value retVal;
	LexItem t = Parser::GetNextToken(in, line);
	if (t == INTEGER || t == REAL || t == CHARACTER) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == DCOLON) {
			status = VarList(in, line, t);
			if (status)
			{
				status = Decl(in, line);

				if (!status)
				{
					ParseError(line, "Declaration Syntactic Error.");
					return false;
				}
				return status;
			}
			else
			{
				ParseError(line, "Missing Variables List.");
				return false;
			}
		}
		else if (t == CHARACTER && tok.GetToken() == LPAREN)
		{
			tok = Parser::GetNextToken(in, line);

			if (tok.GetToken() == LEN)
			{
				tok = Parser::GetNextToken(in, line);

				if (tok.GetToken() == ASSOP)
				{
					tok = Parser::GetNextToken(in, line);

					if (tok.GetToken() == ICONST)
					{
						strLen = tok.GetLexeme();

						tok = Parser::GetNextToken(in, line);
						if (tok.GetToken() == RPAREN)
						{
							tok = Parser::GetNextToken(in, line);
							if (tok.GetToken() == DCOLON)
							{
								status = VarList(in, line, t, stoi(strLen));

								if (status)
								{
									//cout << "Definition of Strings with length of " << strLen << " in declaration statement." << endl;
									status = Decl(in, line);
									if (!status)
									{
										ParseError(line, "Declaration Syntactic Error.");
										return false;
									}
									return status;
								}
								else
								{
									ParseError(line, "Missing Variables List.");
									return false;
								}
							}
						}
						else
						{
							ParseError(line, "Missing Right Parenthesis for String Length definition.");
							return false;
						}

					}
					else
					{
						ParseError(line, "Incorrect Initialization of a String Length");
						return false;
					}
				}
			}
		}
		else
		{
			ParseError(line, "Missing Double Colons");
			return false;
		}

	}

	Parser::PushBackToken(t);
	return true;
}
bool VarList(istream& in, int& line, LexItem& idtok, int strlen) {
	bool status = false, exprstatus = false;
	string identstr;
	Value retVal;

	LexItem tok = Parser::GetNextToken(in, line);
	if (tok == IDENT)
	{
		identstr = tok.GetLexeme();
		if (!(defVar.find(identstr)->second))
		{
			defVar[identstr] = true;
		}
		else
		{
			//cout<< "b4: " << tok << endl;
			ParseError(line, "Variable Redefinition");
			return false;
		}
		SymTable[identstr] = idtok.GetToken();
		if (SymTable[identstr] == CHARACTER) {
			string st(strlen, ' ');
			retVal = Value(st);
			TempsResults[identstr] = retVal;
		}

	}
	else
	{

		ParseError(line, "Missing Variable Name");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok == ASSOP)
	{

		exprstatus = Expr(in, line, retVal);

		if (!exprstatus)
		{
			ParseError(line, "Incorrect initialization for a variable.");
			return false;
		}
		if (idtok == REAL) {
			if (retVal.GetType() == VINT) {
				retVal = double(retVal.GetInt());
			}
		}
		if (idtok == CHARACTER) {
			string s = retVal.GetString();
			int spaces = strlen - s.length();
			if (s.length() > strlen) {
				retVal = Value(s.substr(0, strlen));
			}
			else {
				retVal = Value(s + string(spaces, ' '));
			}
		}
		TempsResults[identstr] = retVal;
		//cout << "Initialization of the variable " << identstr << " in the declaration statement." << endl;
		tok = Parser::GetNextToken(in, line);

		if (tok == COMMA) {
			status = VarList(in, line, idtok, strlen);
		}
		else
		{
			Parser::PushBackToken(tok);
			return true;
		}
	}

	else if (tok == COMMA) {
		status = VarList(in, line, idtok, strlen);
	}
	else if (tok == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");

		return false;
	}
	else {
		Parser::PushBackToken(tok);
		return true;
	}

	return status;
}
bool Stmt(istream& in, int& line) {
	bool status;

	LexItem t = Parser::GetNextToken(in, line);

	switch (t.GetToken()) {

	case PRINT:
		status = PrintStmt(in, line);

		if (status)
			status = Stmt(in, line);
		break;

	case IF:
		status = BlockIfStmt(in, line);
		if (status)
			status = Stmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
		status = AssignStmt(in, line);
		if (status)
			status = Stmt(in, line);
		break;


	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
}
bool SimpleStmt(istream& in, int& line) {
	bool status;

	LexItem t = Parser::GetNextToken(in, line);

	switch (t.GetToken()) {

	case PRINT:
		status = PrintStmt(in, line);

		if (!status)
		{
			ParseError(line, "Incorrect Print Statement");
			return false;
		}
		//cout << "Print statement in a Simple If statement." << endl;
		break;

	case IDENT:
		Parser::PushBackToken(t);
		status = AssignStmt(in, line);
		if (!status)
		{
			ParseError(line, "Incorrect Assignent Statement");
			return false;
		}
		//cout << "Assignment statement in a Simple If statement." << endl;

		break;


	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
}

bool BlockIfStmt(istream& in, int& line) {
	bool ex = false, status;
	LexItem t;

	t = Parser::GetNextToken(in, line);
	if (t != LPAREN) {
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	Value retVal;
	ex = RelExpr(in, line, retVal);
	if (!ex) {
		ParseError(line, "Missing if statement condition");
		return false;
	}
	if (retVal.GetType() == VERR) {
		ParseError(line, "Illegal operand types for a RelExpr.");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if (t != RPAREN) {
		ParseError(line, "Missing Right Parenthsis");
		return false;
	}
	if (retVal.GetBool() == 0) {
		while (t != ELSE && t != END) {
			t = Parser::GetNextToken(in, line);
		}
		status = Stmt(in, line);
		if (!status) {
			ParseError(line, "Missing Statement for If-Else statement");
			return false;
		}
		else
			t = Parser::GetNextToken(in, line);
		if (t != END) {
			ParseError(line, "Missing END of IF statement");
			return false;
		}

		t = Parser::GetNextToken(in, line);
		if (t == IF) {
			return true;
		}
		Parser::PushBackToken(t);
		ParseError(line, "Missing IF at End of IF statement");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if (t != THEN) {
		Parser::PushBackToken(t);
		status = SimpleStmt(in, line);
		if (status) {
			return true;
		}
		else {
			ParseError(line, "If Statement Error");
			return false;
		}
	}
	status = Stmt(in, line);
	if (!status) {
		ParseError(line, "Missing Statement for If Then");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t == ELSE) {
		status = Stmt(in, line);
		if (!status) {
			ParseError(line, "Missing Statement for If Stmt Else");
			return false;
		}
		else
			t = Parser::GetNextToken(in, line);
	}
	if (t != END) {
		ParseError(line, "Missing END of IF");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t == IF) {
		return true;
	}
	Parser::PushBackToken(t);
	ParseError(line, "Missing IF at End of IF statement");
	return false;
}

bool AssignStmt(istream& in, int& line) {
	bool varstatus = false, status = false;
	LexItem t;
	LexItem tt;
	int assign = line;
	Value retVal;

	varstatus = Var(in, line, t);

	if (varstatus) {

		tt = Parser::GetNextToken(in, line);

		if (tt == ASSOP) {
			status = Expr(in, line, retVal);
			if (!status) {
				ParseError(line, "Missing Expression in Assignment Statment");
				return status;
			}
			if (SymTable[t.GetLexeme()] == CHARACTER) {

				if (retVal.GetType() != VSTRING) {
					ParseError(assign, "Illegal mixed assignment operation");
					return false;
				}
				string s = retVal.GetString();
				int space = (TempsResults[t.GetLexeme()].GetString()).length() - s.length();
				if (s.length() > TempsResults[t.GetLexeme()].GetString().length()) {
					retVal = Value(s.substr(0, TempsResults[t.GetLexeme()].GetString().length()));
				}
				else {
					retVal = Value(s + string(space, ' '));
				}
			}
			TempsResults[t.GetLexeme()] = retVal;
			if (TempsResults[t.GetLexeme()].GetType() == VERR) {
				ParseError(assign, "Illegal operand utilized");
				return false;
			}

		}
		else if (tt.GetToken() == ERR) {
			ParseError(line, "Unrecognized Input Pattern");
			//cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else {
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
	}
	else {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	return status;
}

bool Var(istream& in, int& line, LexItem& idtok) {
	string identstr;

	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == IDENT) {
		identstr = tok.GetLexeme();
		idtok = tok;

		if (!(defVar.find(identstr)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}
		return true;
	}
	else if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		//cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}

bool RelExpr(istream& in, int& line, Value& retVal) {
	bool t1 = Expr(in, line, retVal);
	LexItem tok;

	if (!t1) {
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		//cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if (tok == EQ || tok == LTHAN || tok == GTHAN)
	{
		Value temp;
		t1 = Expr(in, line, temp);
		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		switch (tok.GetToken()) {
		case EQ:
			retVal = retVal == temp;
			break;
		case LTHAN:
			retVal = retVal < temp;
			break;
		case GTHAN:
			retVal = retVal > temp;
			break;
		default:
			break;
		}

	}

	return true;
}

bool Expr(istream& in, int& line, Value& retVal) {
	bool t1 = MultExpr(in, line, retVal);
	LexItem tok;

	if (!t1) {
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok == PLUS || tok == MINUS || tok == CAT)
	{
		Value temp;
		t1 = MultExpr(in, line, temp);
		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		if (tok == PLUS) {
			retVal = retVal + temp;
		}
		else if (tok == MINUS) {
			retVal = retVal - temp;
		}
		else if (tok == CAT) {
			retVal = retVal.Catenate(temp);
		}
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR) {
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}

	}
	Parser::PushBackToken(tok);
	return true;
}
bool MultExpr(istream& in, int& line, Value& retVal) {
	bool t1 = TermExpr(in, line, retVal);
	LexItem tok;

	if (!t1) {
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok == MULT || tok == DIV)
	{
		Value temp;
		t1 = TermExpr(in, line, temp);

		if (!t1) {
			ParseError(line, "Missing operand after operator");
			return false;
		}
		if (tok == MULT) {
			retVal = retVal * temp;
		}
		else if (tok == DIV) {
			if (temp.IsInt()) {
				if (temp.GetInt() == 0) {
					ParseError(line - 1, "CANNOT DIVIDE BY ZERO");
					return false;
				}
			}
			else if (temp.IsReal()) {
				if (temp.GetReal() == 0) {
					ParseError(line - 1, "CANNOT DIVIDE BY ZERO");
					return false;
				}
			}
			retVal = retVal / temp;
		}
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR) {
			ParseError(line, "Unrecognized Input Pattern");
			//cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}

	}
	Parser::PushBackToken(tok);
	return true;
}

bool TermExpr(istream& in, int& line, Value& retVal) {
	bool t1 = SFactor(in, line, retVal);
	stack<Value> pow;
	Value expo;
	Value base;
	LexItem tok;
	Value temp2;
	if (!t1) {
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		//cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	pow.push(retVal);
	while (tok == POW)
	{
		t1 = SFactor(in, line, temp2);

		if (!t1) {
			ParseError(line, "Missing exponent operand");
			return false;
		}
		pow.push(temp2);

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR) {
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}

	}
	while (!pow.empty()) {
		if (pow.size() == 1) {
			break;
		}
		expo = pow.top();
		pow.pop();
		base = pow.top();
		pow.pop();
		retVal = base.Power(expo);
		retVal = pow.top().Power(retVal);
	}
	Parser::PushBackToken(tok);
	return true;
}

bool SFactor(istream& in, int& line, Value& retVal) {
	LexItem t = Parser::GetNextToken(in, line);
	bool status;
	int sign = 0;
	if (t == MINUS)
	{
		sign = -1;
	}
	else if (t == PLUS)
	{
		sign = 1;
	}
	else
		Parser::PushBackToken(t);

	status = Factor(in, line, sign, retVal);
	return status;
}

bool Factor(istream& in, int& line, int sign, Value& retVal) {

	LexItem tok = Parser::GetNextToken(in, line);

	//cout << tok.GetLexeme() << endl;
	if (tok == IDENT) {

		string lexeme = tok.GetLexeme();
		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Using Undefined Variable");
			return false;
		}

		retVal = TempsResults[lexeme];

		if (TempsResults[lexeme].GetType() == VERR) {
			ParseError(line, "Using UNINITIALIZED Variable");
			return false;
		}
		if (sign == -1) {
			if (retVal.IsInt()) {
				retVal = -1 * retVal.GetInt();
			}
			else if (retVal.IsReal()) {
				retVal = -1 * retVal.GetReal();
			}
			else if (retVal.IsString()) {
				ParseError(line, "Invalid Use of Operator");
				return false;
			}
		}

		return true;
	}
	else if (tok == ICONST) {
		retVal.SetType(VINT);
		if (sign == -1) {
			retVal.SetInt(-1 * stoi(tok.GetLexeme()));
		}
		else {
			retVal.SetInt(stoi(tok.GetLexeme()));
		}
		return true;
	}
	else if (tok == SCONST) {

		retVal.SetType(VSTRING);
		retVal.SetString(tok.GetLexeme());

		return true;
	}
	else if (tok == RCONST) {
		retVal.SetType(VREAL);
		if (sign == -1) {
			retVal.SetReal(-1 * stod(tok.GetLexeme()));
		}
		else {
			retVal.SetReal(stod(tok.GetLexeme()));
		}

		return true;
	}
	else if (tok == LPAREN) {
		bool ex = Expr(in, line, retVal);
		if (!ex) {
			ParseError(line, "Missing expression after left parenthesis");
			return false;
		}
		if (Parser::GetNextToken(in, line) == RPAREN)
			return ex;
		else
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing right parenthesis after expression");
			return false;
		}
	}
	else if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}


	return false;
}