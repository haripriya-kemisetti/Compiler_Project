#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <iomanip>
#include <stdexcept>
#include <cmath>
#include <sstream>

using namespace std;

enum ValType { VINT, VREAL, VSTRING, VBOOL, VERR };

class Value {
    ValType	T;
    bool    Btemp;
    int 	Itemp;
    double   Rtemp;
    string	Stemp;
    int strLen;


public:
    Value() : T(VERR), Btemp(false), Itemp(0), Rtemp(0.0), Stemp(""), strLen(0) {}
    Value(bool vb) : T(VBOOL), Btemp(vb), Itemp(0), Rtemp(0.0), Stemp(""), strLen(0) {}
    Value(int vi) : T(VINT), Btemp(false), Itemp(vi), Rtemp(0.0), Stemp(""), strLen(0) {}
    Value(double vr) : T(VREAL), Btemp(false), Itemp(0), Rtemp(vr), Stemp(""), strLen(0) {}
    Value(string vs) : T(VSTRING), Btemp(false), Itemp(0), Rtemp(0.0), Stemp(vs), strLen(1) { }


    ValType GetType() const { return T; }
    bool IsErr() const { return T == VERR; }
    bool IsString() const { return T == VSTRING; }
    bool IsReal() const { return T == VREAL; }
    bool IsBool() const { return T == VBOOL; }
    bool IsInt() const { return T == VINT; }

    int GetInt() const { if (IsInt()) return Itemp; throw "RUNTIME ERROR: Value not an integer"; }

    string GetString() const { if (IsString()) return Stemp; throw "RUNTIME ERROR: Value not a string"; }

    double GetReal() const { if (IsReal()) return Rtemp; throw "RUNTIME ERROR: Value not an integer"; }

    bool GetBool() const { if (IsBool()) return Btemp; throw "RUNTIME ERROR: Value not a boolean"; }

    int GetstrLen() const { if (IsString()) return strLen; throw "RUNTIME ERROR: Value not a string"; }

    void SetType(ValType type)
    {
        T = type;
    }

    void SetInt(int val)
    {
        Itemp = val;
    }

    void SetReal(double val)
    {
        Rtemp = val;
    }

    void SetString(string val)
    {
        Stemp = val;
    }

    void SetBool(bool val)
    {
        Btemp = val;
    }

    void SetstrLen(int len)
    {
        strLen = len;
    }


    // numeric overloaded add this to op
    Value operator+(const Value& op) const;

    // numeric overloaded subtract op from this
    Value operator-(const Value& op) const;

    // numeric overloaded multiply this by op
    Value operator*(const Value& op) const;

    // numeric overloaded divide this by oper
    Value operator/(const Value& op) const;

    //string concatenation of this with op
    Value Catenate(const Value& op) const;

    //compute the value of this raised to the exponent op
    Value Power(const Value& op) const;

    //overloaded equality operator of this with op
    Value operator==(const Value& op) const;
    //overloaded greater than operator of this with op
    Value operator>(const Value& op) const;
    //overloaded less than operator of this with op
    Value operator<(const Value& op) const;


    friend ostream& operator<<(ostream& out, const Value& op) {
        if (op.IsInt()) out << op.Itemp;
        else if (op.IsBool()) out << (op.GetBool() ? "true" : "false");
        else if (op.IsString()) out << op.Stemp;
        else if (op.IsReal()) out << fixed << showpoint << setprecision(2) << op.Rtemp;
        else if (op.IsErr()) out << "ERROR";
        return out;
    }
};


#endif
