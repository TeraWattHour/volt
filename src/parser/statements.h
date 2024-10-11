#pragma once

#include "parser.h"

class Block : public Stmt {
public:
    Block(std::vector<std::unique_ptr<Stmt>> &stmts) : stmts(std::move(stmts)) {}
    
    void print(std::ostream &os) const override {
        os << "{" << std::endl;
        for (auto &stmt : stmts) {
            os << "    ";
            (*stmt).print(os);
            os << std::endl;
        }
        os << "}";
    }
private:
    std::vector<std::unique_ptr<Stmt>> stmts;
};

class ExpressionStatement : public Stmt {
public:
    ExpressionStatement(std::unique_ptr<Expr>& expr, bool is_returned = false) : expr(std::move(expr)), is_returned(is_returned) {}
    
    void print(std::ostream &os) const override {
        (*expr).print(os);
        if (!is_returned) os << ";";
    }
private:
    std::unique_ptr<Expr> expr;
    bool is_returned = false;
};

class Let : public Stmt {
public:
    Let(Token name) : name(name) {}
    Let(Token name, std::unique_ptr<Expr> expr) : name(name), value(std::move(expr)) {}
    Let(Token name, std::unique_ptr<Block> closure) : name(name), value(std::move(closure)) {}
    
    void set_value(std::unique_ptr<Expr> expr) {
        value = std::move(expr);
    }
    
    void set_value(std::unique_ptr<Block> closure) {
        value = std::move(closure);
    }
    
    void print(std::ostream &os) const override {
        os << "let " << std::string(name.start, name.end - name.start) << " ";
        if (std::holds_alternative<std::unique_ptr<Expr>>(value)) {
            os << "= ";
        }
        std::visit([&os](auto &v) {
            (*v).print(os);
        }, value);
        os << ";";
    }
private:
    Token name;
    std::variant<std::unique_ptr<Expr>, std::unique_ptr<Block>> value;
};
