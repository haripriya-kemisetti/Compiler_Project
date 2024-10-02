#include "val.h"

Value Value::operator*(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() * op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() * op.GetReal());
    }
    else if (IsReal() && op.IsInt()) {
        return Value(GetReal() * op.GetInt());
    }
    else if (IsInt() && op.IsReal()) {
        return Value(GetInt() * op.GetReal());
    }
    return Value();
}

Value Value::Catenate(const Value& op) const {
    if (IsString() && op.IsString()) {
        return Value(GetString() + op.GetString());
    }
    return Value();
}

Value Value::Power(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(pow(GetInt(), op.GetInt()));
    }
    else if (IsReal() && op.IsReal()) {
        return Value(pow(GetReal(), op.GetReal()));
    }
    else if (IsReal() && op.IsInt()) {
        return Value(pow(GetReal(), op.GetInt()));
    }
    else if (IsInt() && op.IsReal()) {
        return Value(pow(GetInt(), op.GetReal()));
    }
    return Value();
}

Value Value::operator<(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() < op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() < op.GetReal());
    }
    else if (IsString() && op.IsString()) {
        for (char c : GetString()) {
            if (isspace(c)) {
                return Value();
            }
        }
        for (char c : op.GetString()) {
            if (isspace(c)) {
                return Value();
            }
        }
        return Value(GetString() < op.GetString());
    }
    else if (IsReal() && op.IsInt()) {
        return Value(GetReal() < op.GetInt());
    }
    else if (IsInt() && op.IsReal()) {
        return Value(GetInt() < op.GetReal());
    }
    return Value();
}

Value Value::operator+(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() + op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() + op.GetReal());
    }
    else if (IsReal() && op.IsInt()) {
        return Value(GetReal() + op.GetInt());
    }
    else if (IsInt() && op.IsReal()) {
        return Value(GetInt() + op.GetReal());
    }
    return Value();
}
Value Value::operator-(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() - op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() - op.GetReal());
    }
    else if (IsReal() && op.IsInt()) {
        return Value(GetReal() - op.GetInt());
    }
    else if (IsInt() && op.IsReal()) {
        return Value(GetInt() - op.GetReal());
    }
    return Value();
}
Value Value::operator/(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() / op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() / op.GetReal());
    }
    else if (IsReal() && op.IsInt()) {
        return Value(GetReal() / op.GetInt());
    }
    else if (IsInt() && op.IsReal()) {
        return Value(GetInt() / op.GetReal());
    }
    return Value();
}
Value Value::operator==(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() == op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() == op.GetReal());
    }
    return Value();
}
Value Value::operator>(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() > op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() > op.GetReal());
    }
    else if (IsString() && op.IsString()) {
        for (char c : GetString()) {
            if (isspace(c)) {
                return Value();
            }
        }
        for (char c : op.GetString()) {
            if (isspace(c)) {
                return Value();
            }
        }
        return Value(GetString() < op.GetString());
    }
    else if (IsReal() && op.IsInt()) {
        return Value(GetReal() > op.GetInt());
    }
    else if (IsInt() && op.IsReal()) {
        return Value(GetInt() > op.GetReal());
    }
    return Value();
}