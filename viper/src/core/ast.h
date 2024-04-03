#pragma once

/*
 *  ast.h
 *
 *  This contains the interface for the Abstract Syntax Tree of the Viper language
 *
 */

#include "core/result.h"
#include "core/scope.h"
#include "defines.h"
#include "token.h"
#include "core/context.h"

#include <memory>

namespace viper {

// Operator precedences
using prec_e = enum class precedence {
    LOWEST = 0,
    ASSIGN,
    COMPARISON,
    ADDSUB,
    MULDIVMOD,
    BITSHIFT,
    PREFIX,
    CALL,
    INVALID_OP
};

/* The kind of node in the AST */
enum NodeKind {
    AST_NOOP,
    AST_INVALID_NODE
};

/* The data type of node in AST */
struct Type {
    enum TypeKind {
        BOOL,
        BYTE,
        I8,
        I16,
        I32,
        I64,
        U8,
        U16,
        U32,
        U64,
        F32,
        F64,
        ENUM,
        POINTER,
        PROCEDURE,
        ARRAY,
        STRUCTURE,
        UNION,
    };

};

/* Node in the AST */
struct ASTNode {
    ASTNode(NodeKind _kind) : kind(_kind) {}
    ASTNode() {}
    NodeKind kind;
    token tok;
    ASTNode* parent;

    static std::shared_ptr<ASTNode> create_new(token tok, NodeKind kind);

    Context& getContext() {return context;}
    void setContext(Context context) {}
    virtual void print() {}

private:

    Context context;
    ASTNode* parent_node;
    std::string module;
    Scope* scope;
};

// For procedure nodes
struct ProcedureNode : public ASTNode {
    ProcedureNode();
    ~ProcedureNode();
    
    std::string& get_name();
    void set_name(const std::string& name);

    void set_return_type(ASTNode* node);
    const ASTNode* get_return_type() const;

    void add_parameter(ASTNode* param);

    void add_statement(ASTNode* stmt);

    void print() override {
        // Print function prototype
        std::printf("proc <%s>: <%s> (", name.c_str(), return_declarator->tok.name.c_str());
        for (const auto& param : this->parameters) {
            param->print();
            std::printf(", ");
        }
        std::printf(") {\n");

        /// Print the code body
        for (const auto& stmt : code_body) {
            std::printf("    ");
            stmt->print();
            std::printf("\n");
        }

        std::printf("}\n");
    }

    private:
    std::string name;                 // unmangled name of the procedure
    std::string lookup_name;        
    std::string mangled_name;         // the mangled name of the procedure
    std::vector<ASTNode*> parameters; // the parameter definitions for the function
    ASTNode* return_declarator;       
    ASTNode* procedure_declarator;    
    Scope* scope;
    std::vector<ASTNode*> code_body;  // code block body of the procedure
};

struct ProcParameter : public ASTNode {
    void set_name(const std::string& str) {
        name = str;
    }
    std::string get_name() const { 
        return name;
    }

    void set_data_type(const token& tok) {
        data_type = tok;
    }
    token get_data_type() const {
        return data_type;
    }

    void print() override {
        std::printf("<%s>: <%s>", this->name.c_str(), data_type.name.c_str());
    }

    private:
    std::string name;
    token data_type;
};

/* Represents the declaration of a new variable 
   let x: i32 = 0;
 * */
struct VariableDeclaration : public ASTNode {
    VariableDeclaration(
        const std::string& name,
        ASTNode* type_spec,
        ASTNode* expr
    ) : name(name), name_mangled(name), type_spec(type_spec), value(expr) {}
    ~VariableDeclaration() {}

    std::string name;
    std::string name_mangled;
    ASTNode* type_spec;
    ASTNode* value;

    void print() override {
        std::printf("let <%s>: <%s> = ", name.c_str(), type_spec->tok.name.c_str());
        this->value->print();
    }
};


/* Represents an expression. Evaluates to a value */
struct ExpressionNode : public ASTNode {
};

/* Represents an expression statement. 
 * x = 10 + b;
 * x + 2;
 */
struct ExpressionStatementNode : public ASTNode{
    ExpressionNode* expr;

    void print() override {
        expr->print();
    }
};

/* Represents an expression with a prefix operator
 * !x
 * ~x
 */
struct ExpressionPrefixNode : public ExpressionNode {
    token op;
    ExpressionNode* rhs;

    void print() override {
        std::printf("%s", token::kind_to_str(op.kind).c_str());
        rhs->print();
    }
};

/* Represents an expression with an infix operator
 * x + y
 * y % 6
 */
struct ExpressionBinaryNode : public ExpressionNode {
    ExpressionNode* lhs;
    token op;
    ExpressionNode* rhs;

    void print() override {
        std::printf("[");
        rhs->print();
        std::printf(" ");
        lhs->print();
        std::printf("]");
    }
};

/* Represents an integer literal */
struct IntegerLiteralNode : public ExpressionNode {
    IntegerLiteralNode(u64 value) : value(value) {}

    void print() override {
        std::printf("%lu", this->value);
    }

    u64 value;
};

/* Represents a boolean "true" or "false" literal expression */
struct BooleanLiteralNode : public ExpressionNode {
    BooleanLiteralNode(bool is_true) : is_true(is_true) {}

    void print() override {
        if (is_true) {
            std::printf("true");
        } else {
            std::printf("false");
        }
    }

    bool is_true; // whether or not this evalueates to true or false
};

/* Represents a floating point number literal */
struct FloatLiteralNode : public ExpressionNode {
    FloatLiteralNode(f64 value) : value(value) {}

    void print() override {
        std::printf("%lf", value);
    }

    f64 value;
};

/* The structure for the 
 * abstract syntax tree */
struct AST {
    ASTNode* head;

    static std::shared_ptr<AST> create_new();
    const std::vector<ASTNode*>& get_nodes() const;
    
    void add_node(ASTNode* node);

    void print_tree() {
        for (const auto& node : nodes) {
            node->print();
            std::printf("\n");
        }
    }

    private:
        AST() {}
        std::vector<ASTNode*> nodes;
};

}
