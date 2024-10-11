#pragma once

#include "parser.h"

class Infix : public Expr {
public:
    Token op;
    std::unique_ptr<Expr> lhs, rhs;
    
    Infix(Token op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
        : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    
    void print(std::ostream& os) const override {
        os << "(" << *lhs << " " << op << " " << *rhs << ")";
    }
};

class Prefix : public Expr {
public:
    Token op;
    std::unique_ptr<Expr> rhs;
    
    Prefix(Token op, std::unique_ptr<Expr> rhs) : op(op), rhs(std::move(rhs)) {}
    
    void print(std::ostream& os) const override {
        os << "(" << op << *rhs << ")";
    }
};

class Integer : public Expr {
public:
    Integer(Token token, int64_t i) : token(token), value(i) {}
    
    void print(std::ostream &os) const override {
        os << value;
    }
private:
    Token token;
    int64_t value;
};

class Double : public Expr {
public:
    Double(Token token, double d) : token(token), value(d) {}
    
    void print(std::ostream &os) const override {
        os << value;
    }
private:
    Token token;
    double value;
};

class Cast : public Expr {
public:
    Cast(Token token, std::unique_ptr<Expr> &lhs, Token rhs) : token(token), lhs(std::move(lhs)), rhs(rhs) {}
    
    void print(std::ostream &os) const override {
        lhs->print(os);
        os << "::" << rhs;
    }
private:
    Token token;
    std::unique_ptr<Expr> lhs;
    Token rhs;
};
