/**
 * @file ast.hpp
 * @brief 抽象语法树（AST）核心定义
 *  
 * @author azhz1107cat
 * @date 2025-10-25
 */

#pragma once
#include <memory>
#include <string>
#include <vector>

#include "error/error_reporter.hpp"

namespace kiz {

enum class AstType {
    // 表达式类型（对应 Expression 子类）
    NilExpr, BoolExpr,
    StringExpr, NumberExpr, ListExpr, IdentifierExpr,
    BinaryExpr, UnaryExpr,
    CallExpr,
    GetMemberExpr, GetItemExpr,
    FuncDeclExpr, DictDeclExpr,

    // 语句类型（对应 Statement 子类）
    AssignStmt, NonlocalAssignStmt, GlobalAssignStmt,
    SetMemberStmt, SetItemStmt,
    BlockStmt, IfStmt, WhileStmt,
    ReturnStmt, ImportStmt, ForStmt, TryStmt, CatchStmt,
    NullStmt, ExprStmt,
    BreakStmt, NextStmt, ThrowStmt
};

// AST 基类
struct ASTNode {
    err::PositionInfo pos{};
    AstType ast_type = AstType::NullStmt;
    
    virtual ~ASTNode() = default;
};

// 符号类型信息
struct TypeInfo {
    std::string type_name;
    std::vector<std::unique_ptr<TypeInfo>> subs;
    explicit TypeInfo(std::string tn, std::vector<std::unique_ptr<TypeInfo>> s = {})
        : type_name(std::move(tn)), subs(std::move(s)) {}
};


// 表达式基类
struct Expression :  ASTNode {
    std::unique_ptr<TypeInfo> type_info;
};

// 语句基类
struct Statement :  ASTNode {};

// 字符串字面量
struct StringExpr final :  Expression {
    std::string value;
    explicit StringExpr(const err::PositionInfo& pos, std::string v)
        : value(std::move(v)) {
        this->pos = pos;
        this->ast_type = AstType::StringExpr;
        this->type_info = std::make_unique<TypeInfo>("string");
    }
};

// 数字字面量
struct NumberExpr final :  Expression {
    std::string value;
    explicit NumberExpr(const err::PositionInfo& pos, std::string v)
        : value(std::move(v)) {
        this->pos = pos;
        this->ast_type = AstType::NumberExpr;
        this->type_info = std::make_unique<TypeInfo>("number");
    }
};

// 空值字面量
struct NilExpr final : Expression {
    explicit NilExpr(const err::PositionInfo& pos) {
        this->pos = pos;
        this->ast_type = AstType::NilExpr;
    }
};

// 布尔值字面量
struct BoolExpr final : Expression {
    bool val;
    explicit BoolExpr(const err::PositionInfo& pos, bool v) : val(v) {
        this->pos = pos;
        this->ast_type = AstType::BoolExpr;
    }
};

// 数组字面量
struct ListExpr final :  Expression {
    std::vector<std::unique_ptr<Expression>> elements;
    explicit ListExpr(const err::PositionInfo& pos, std::vector<std::unique_ptr<Expression>> elems)
        : elements(std::move(elems)) {
        this->pos = pos;
        this->ast_type = AstType::ListExpr;
        this->type_info = std::make_unique<TypeInfo>("list");
    }
};

// 标识符
struct IdentifierExpr final :  Expression {
    std::string name;
    explicit IdentifierExpr(const err::PositionInfo& pos, std::string n)
        : name(std::move(n)) {
        this->pos = pos;
        this->ast_type = AstType::IdentifierExpr;
        this->type_info = std::make_unique<TypeInfo>("identifier");
    }
};

// 二元运算
struct BinaryExpr final :  Expression {
    std::string op;
    std::unique_ptr<Expression> left, right;
    BinaryExpr(const err::PositionInfo& pos, std::string o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : op(std::move(o)), left(std::move(l)), right(std::move(r)) {
        this->pos = pos;
        this->ast_type = AstType::BinaryExpr;
        this->type_info = std::make_unique<TypeInfo>("binary_expr");
    }
};

// 一元运算
struct UnaryExpr final :  Expression {
    std::string op;
    std::unique_ptr<Expression> operand;
    UnaryExpr(const err::PositionInfo& pos, std::string o, std::unique_ptr<Expression> e)
        : op(std::move(o)), operand(std::move(e)) {
        this->pos = pos;
        this->ast_type = AstType::UnaryExpr;
        this->type_info = std::make_unique<TypeInfo>("unary_expr");
    }
};

// 赋值
struct AssignStmt final :  Statement {
    std::string name;
    std::unique_ptr<Expression> expr;
    AssignStmt(const err::PositionInfo& pos, std::string n, std::unique_ptr<Expression> e)
        : name(std::move(n)), expr(std::move(e)) {
        this->pos = pos;
        this->ast_type = AstType::AssignStmt;
    }
};

// nonlocal赋值
struct NonlocalAssignStmt final :  Statement {
    std::string name;
    std::unique_ptr<Expression> expr;
    NonlocalAssignStmt(const err::PositionInfo& pos, std::string n, std::unique_ptr<Expression> e)
        : name(std::move(n)), expr(std::move(e)) {
        this->pos = pos;
        this->ast_type = AstType::NonlocalAssignStmt;
    }
};

// global赋值
struct GlobalAssignStmt final :  Statement {
    std::string name;
    std::unique_ptr<Expression> expr;
    GlobalAssignStmt(const err::PositionInfo& pos, std::string n, std::unique_ptr<Expression> e)
        : name(std::move(n)), expr(std::move(e)) {
        this->pos = pos;
        this->ast_type = AstType::GlobalAssignStmt;
    }
};

// 复合语句块
struct BlockStmt final :  Statement {
    std::vector<std::unique_ptr<Statement>> statements{};
    explicit BlockStmt(const err::PositionInfo& pos, std::vector<std::unique_ptr<Statement>> s)
        : statements(std::move(s)) {
        this->pos = pos;
        this->ast_type = AstType::BlockStmt;
    }
};

// if 语句
struct IfStmt final :  Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStmt> thenBlock;
    std::unique_ptr<BlockStmt> elseBlock;
    IfStmt(const err::PositionInfo& pos, std::unique_ptr<Expression> cond, std::unique_ptr<BlockStmt> thenB, std::unique_ptr<BlockStmt> elseB)
        : condition(std::move(cond)), thenBlock(std::move(thenB)), elseBlock(std::move(elseB)) {
        this->pos = pos;
        this->ast_type = AstType::IfStmt;
    }
};

// while 语句
struct WhileStmt final :  Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStmt> body;
    WhileStmt(const err::PositionInfo& pos, std::unique_ptr<Expression> cond, std::unique_ptr<BlockStmt> b)
        : condition(std::move(cond)), body(std::move(b)) {
        this->pos = pos;
        this->ast_type = AstType::WhileStmt;
    }
};

// throw语句
struct ThrowStmt final :  Statement{
    std::unique_ptr<Expression> expr;
    explicit ThrowStmt(const err::PositionInfo& pos, std::unique_ptr<Expression> e)
        : expr(std::move(e)) {
        this->pos = pos;
        this->ast_type = AstType::ThrowStmt;
    }
};

// for语句
struct ForStmt final :  Statement {
    std::string item_var_name;
    std::unique_ptr<Expression> iter;
    std::unique_ptr<BlockStmt> body;
    explicit ForStmt(const err::PositionInfo& pos,
        std::string iv,
        std::unique_ptr<Expression> i,
        std::unique_ptr<BlockStmt> b
    ) : item_var_name(std::move(iv)), iter(std::move(i)), body(std::move(b)) {
        this->pos = pos;
        this->ast_type = AstType::ForStmt;
    }
};

// catch语句
struct CatchStmt final :  Statement {
    std::unique_ptr<Expression> error;
    std::string var_name;
    std::unique_ptr<BlockStmt> catch_block;
    explicit CatchStmt(const err::PositionInfo& pos,
        std::unique_ptr<Expression> e,
        std::string v,
        std::unique_ptr<BlockStmt> c
    ) : error(std::move(e)), var_name(std::move(v)), catch_block(std::move(c)) {
        this->pos = pos;
        this->ast_type = AstType::CatchStmt;
    }
};

// try语句
struct TryStmt final :  Statement {
    std::unique_ptr<BlockStmt> try_block;
    std::vector<std::unique_ptr<CatchStmt>> catch_blocks;
    explicit TryStmt(const err::PositionInfo& pos,
        std::unique_ptr<BlockStmt> t,
        std::vector<std::unique_ptr<CatchStmt>> c
    ) : try_block(std::move(t)), catch_blocks(std::move(c)) {
        this->pos = pos;
        this->ast_type = AstType::TryStmt;
    }
};

// 设置成员
struct SetMemberStmt final :  Statement {
    std::unique_ptr<Expression> g_mem;
    std::unique_ptr<Expression> val;
    SetMemberStmt(const err::PositionInfo& pos, std::unique_ptr<Expression> g_mem, std::unique_ptr<Expression> val)
        : g_mem(std::move(g_mem)), val(std::move(val)) {
        this->pos = pos;
        this->ast_type = AstType::SetMemberStmt;
    }
};

// 函数调用
struct CallExpr final :  Expression {
    std::unique_ptr<Expression> callee;
    std::vector<std::unique_ptr<Expression>> args;
    CallExpr(const err::PositionInfo& pos, std::unique_ptr<Expression> c, std::vector<std::unique_ptr<Expression>> a)
        : callee(std::move(c)), args(std::move(a)) {
        this->pos = pos;
        this->ast_type = AstType::CallExpr;
        this->type_info = std::make_unique<TypeInfo>("call_expr");
    }
};

// 获取成员
struct GetMemberExpr final :  Expression {
    std::unique_ptr<Expression> father;
    std::unique_ptr<IdentifierExpr> child;
    GetMemberExpr(const err::PositionInfo& pos, std::unique_ptr<Expression> f, std::unique_ptr<IdentifierExpr> c)
        : father(std::move(f)), child(std::move(c)) {
        this->pos = pos;
        this->ast_type = AstType::GetMemberExpr;
        this->type_info = std::make_unique<TypeInfo>("get_member_expr");
    }
};

// 获取项
struct GetItemExpr final :  Expression {
    std::unique_ptr<Expression> father;
    std::vector<std::unique_ptr<Expression>> params;
    GetItemExpr(const err::PositionInfo& pos, std::unique_ptr<Expression> f, std::vector<std::unique_ptr<Expression>> p)
        : father(std::move(f)), params(std::move(p)) {
        this->pos = pos;
        this->ast_type = AstType::GetItemExpr;
        this->type_info = std::make_unique<TypeInfo>("get_item_expr");
    }
};

// 声明匿名函数
struct FnDeclExpr final :  Expression {
    std::string name;
    std::vector<std::string> params;
    std::unique_ptr<BlockStmt> body;
    FnDeclExpr(const err::PositionInfo& pos, std::string n, std::vector<std::string> p, std::unique_ptr<BlockStmt> b)
        : name(std::move(n)), params(std::move(p)), body(std::move(b)) {
        this->pos = pos;
        this->ast_type = AstType::FuncDeclExpr;
        this->type_info = std::make_unique<TypeInfo>("function");
    }
};

// 声明字典
struct DictDeclExpr final :  Expression {
    std::string name;
    std::vector<std::pair<std::string, std::unique_ptr<Expression>>> init_list;
    DictDeclExpr(const err::PositionInfo& pos, std::string n, std::vector<std::pair<std::string, std::unique_ptr<Expression>>> i)
        : name(std::move(n)), init_list(std::move(i)) {
        this->pos = pos;
        this->ast_type = AstType::DictDeclExpr;
        this->type_info = std::make_unique<TypeInfo>("dict");
    }
};

// return 语句
struct ReturnStmt final :  Statement {
    std::unique_ptr<Expression> expr;
    explicit ReturnStmt(const err::PositionInfo& pos, std::unique_ptr<Expression> e)
        : expr(std::move(e)) {
        this->pos = pos;
        this->ast_type = AstType::ReturnStmt;
    }
};

// import 语句
struct ImportStmt final :  Statement {
    std::string path;
    explicit ImportStmt(const err::PositionInfo& pos, std::string p)
        : path(std::move(p)) {
        this->pos = pos;
        this->ast_type = AstType::ImportStmt;
    }
};

// 空语句
struct NullStmt final :  Statement {
    explicit NullStmt(const err::PositionInfo& pos) {
        this->pos = pos;
        this->ast_type = AstType::NullStmt;
    }
};

// break 语句
struct BreakStmt final :  Statement {
    explicit BreakStmt(const err::PositionInfo& pos) {
        this->pos = pos;
        this->ast_type = AstType::BreakStmt;
    }
};

// continue 语句
struct NextStmt final :  Statement {
    explicit NextStmt(const err::PositionInfo& pos) {
        this->pos = pos;
        this->ast_type = AstType::NextStmt;
    }
};

// 表达式语句
struct ExprStmt final :  Statement {
    std::unique_ptr<Expression> expr;
    explicit ExprStmt(const err::PositionInfo& pos, std::unique_ptr<Expression> e)
        : expr(std::move(e)) {
        this->pos = pos;
        this->ast_type = AstType::ExprStmt;
    }
};

} // namespace kiz