#ifndef LISP_INTERPRETER_H_INCLUDED
#define LISP_INTERPRETER_H_INCLUDED

#include <cstdio>
#include <locale>
#include <cstring>
#include <algorithm>

#include <stack>

#include "Types.h"
#include "VariablePool.h"
#include "FunctionPool.h"

class CLispInterpreter
{
public:
    CLispInterpreter() = default;

    CLispInterpreter             (const CLispInterpreter&) = delete;
    CLispInterpreter& operator = (const CLispInterpreter&) = delete;

    CLispInterpreter(CLispInterpreter&& assign_interpreter):
        cur_pos_(nullptr), var_pool_stack_(), func_pool_()
    {
        var_pool_stack_.push(CLispVariablePool());

        std::swap(cur_pos_,        assign_interpreter.cur_pos_);
        std::swap(var_pool_stack_, assign_interpreter.var_pool_stack_);
        std::swap(func_pool_,      assign_interpreter.func_pool_);
    }

    CLispInterpreter& operator = (CLispInterpreter&& assign_interpreter)
    {
        std::swap(cur_pos_,        assign_interpreter.cur_pos_);
        std::swap(var_pool_stack_, assign_interpreter.var_pool_stack_);
        std::swap(func_pool_,      assign_interpreter.func_pool_);

        return (*this);
    }

    ~CLispInterpreter()
    {
        cur_pos_ = nullptr;
    }

public:
//    void skip_spaces();
//
//    number_t_ eval_number();
//    number_t_ eval_list();
//
//    number_t_ eval_expr();
//
//    template <typename Pred>
//    number_t_ reduce_oper(Pred&& oper_pred, number_t_ default_val);

    TLispNumber eval_program(const char* program_str_set)
    {
        cur_pos_ = program_str_set;

        TLispNumber result = TLispNumber(0);

        while (*cur_pos_) result = eval_expr();

        cur_pos_ = nullptr;

        return result;
    }

    void skip_spaces()
    {
        while (std::isspace(*cur_pos_)) ++cur_pos_;
    }

    void skip_list()
    {
        LISP_REQUIRE_(*cur_pos_ == '(', "list '(' expected")

        size_t bracket_diff = 1;
        while (*cur_pos_ && bracket_diff > 0)
        {
            ++cur_pos_;

            if      (*cur_pos_ == '(') ++bracket_diff;
            else if (*cur_pos_ == ')') --bracket_diff;
        }

        LISP_REQUIRE_(*cur_pos_ == ')', "list ')' expected")
        ++cur_pos_; skip_spaces();
    }

    void read_symbol(char symb_name[LISP_NAMEBUF_SIZE])
    {
        LISP_REQUIRE_(std::isalpha(*cur_pos_), "symbol expected")

        const char* temp_pos = cur_pos_;
        while (*temp_pos && std::isalpha(*temp_pos)) ++temp_pos;

        LISP_REQUIRE_(temp_pos - cur_pos_ < LISP_NAMEBUF_SIZE, "too long [>= LISP_NAMEBUF_SIZE] symbol name")
        memcpy(symb_name, cur_pos_, temp_pos - cur_pos_);
        symb_name[temp_pos - cur_pos_] = '\0';

        cur_pos_ = temp_pos;
        skip_spaces();
    }

    TLispNumber eval_expr()
    {
        TLispNumber result = TLispNumber(-1);

        if      (std::isdigit(*cur_pos_)) result = eval_number  ();
        else if (std::isalpha(*cur_pos_)) result = eval_variable();
        else if (*cur_pos_ == '(')        result = eval_list    ();
        else                              result = TLispNumber(-1);

        return result;
    }

    TLispNumber eval_number()
    {
        TLispNumber result = TLispNumber(-1);

        LISP_REQUIRE_(std::isdigit(*cur_pos_), "number expected")

        sscanf(cur_pos_, "%ul", &result);

        while (std::isdigit(*cur_pos_)) ++cur_pos_;
        skip_spaces();

        return result;
    }

    TLispNumber eval_variable()
    {
        TLispNumber result = TLispNumber(-1);

        LISP_REQUIRE_(std::isalpha(*cur_pos_), "symbol expected")

        char name[LISP_NAMEBUF_SIZE] = "";
        read_symbol(name);

        const auto& var_node = var_pool_stack_.top().eval(name);
        result = var_node.value;

        return result;
    }

    TLispNumber eval_function()
    {
        TLispNumber result = TLispNumber(-1);

        LISP_REQUIRE_(std::isalpha(*cur_pos_), "symbol expected")

        char name[LISP_NAMEBUF_SIZE] = "";
        read_symbol(name);

        const auto& func_node = func_pool_.eval(name);

        var_pool_stack_.push(CLispVariablePool());

        for (size_t i = 0; i < func_node.args_name_vec.size(); ++i)
        {
            LISP_REQUIRE_(std::isalpha(*cur_pos_) || std::isdigit(*cur_pos_), "function argument expected")
            var_pool_stack_.top().define(func_node.args_name_vec[i], eval_expr());
        }

        LISP_REQUIRE_(*cur_pos_ == ')', "list ')' expected");

        const char* temp_pos = cur_pos_;
        cur_pos_ = func_node.list_pos;

//        eval_program();

        var_pool_stack_.pop();
        cur_pos_ = temp_pos;

        return result;
    }

    TLispNumber eval_list()
    {
        TLispNumber result = TLispNumber(-1);

        LISP_REQUIRE_(*cur_pos_ == '(', "list '(' expected")
        ++cur_pos_; skip_spaces();

        #define LISP_HANDLE_OPER_(OP_NAME_, OP_PRED_, OP_DEFAULT_) \
            else if (!strncasecmp(cur_pos_, OP_NAME_, sizeof(OP_NAME_)-1) && \
                     !std::isalpha(cur_pos_[sizeof(OP_NAME_)-1])) \
            { \
                cur_pos_ += sizeof(OP_NAME_)-1; skip_spaces(); \
                result = reduce_oper(OP_PRED_, OP_DEFAULT_); \
            }

        #define LISP_HANDLE_KEYW_(KW_NAME_, KW_SUFF_) \
            else if (!strncasecmp(cur_pos_, KW_NAME_, sizeof(KW_NAME_)-1) && \
                     !std::isalpha(cur_pos_[sizeof(KW_NAME_)-1])) \
            { \
                cur_pos_ += sizeof(KW_NAME_)-1; skip_spaces(); \
                result = eval_##KW_SUFF_(); \
            }

        if (*cur_pos_ == ')') result = TLispNumber(-1);
        #include "macro/MacroOper.h"
        #include "macro/MacroKeyword.h"
        else result = eval_function();

        #undef LISP_HANDLE_KEYW_
        #undef LISP_HANDLE_OPER_

        LISP_REQUIRE_(*cur_pos_ == ')', "list ')' expected")
        ++cur_pos_; skip_spaces();

        return result;
    }

    template <typename Pred>
    TLispNumber reduce_oper(Pred&& oper_pred, TLispNumber default_val)
    {
        TLispNumber result = default_val;

        if (*cur_pos_ && *cur_pos_ != ')')
            result = eval_expr();

        while (*cur_pos_ && *cur_pos_ != ')')
            result = std::forward<Pred>(oper_pred)(result, eval_expr());

        return result;
    }

    TLispNumber eval_setq()
    {
        TLispNumber result = TLispNumber(-1);

        LISP_REQUIRE_(std::isalpha(*cur_pos_), "symbol expected")

        char var_symb[LISP_NAMEBUF_SIZE] = "";
        read_symbol(var_symb);

        if (*cur_pos_ == ')') result = TLispNumber(0);
        else                  result = eval_expr();

        var_pool_stack_.top().define(var_symb, result);

        return result;
    }

    TLispNumber eval_defun()
    {
        TLispNumber result = TLispNumber(-1);

        LISP_REQUIRE_(var_pool_stack_.size() == 1, "global space expected")
        LISP_REQUIRE_(std::isalpha(*cur_pos_), "symbol expected")

        char                                 func_symb[LISP_NAMEBUF_SIZE] = "";
        std::vector<char[LISP_NAMEBUF_SIZE]> func_args_name_vec;
        const char*                          func_list_pos = nullptr;

        read_symbol(func_symb);

        LISP_REQUIRE_(*cur_pos_ == '(', "list '(' expected")
        ++cur_pos_; skip_spaces();

        while (*cur_pos_ && *cur_pos_ != ')')
        {
            func_args_name_vec.push_back({});
            read_symbol(func_args_name_vec.back());
        }

        LISP_REQUIRE_(*cur_pos_ == ')', "list ')' expected")
        ++cur_pos_; skip_spaces();

        func_list_pos = cur_pos_;
        skip_list();

        func_pool_.define(func_symb, std::move(func_args_name_vec), func_list_pos);

        return result;
    }

private:
    const char* cur_pos_;

    std::stack<CLispVariablePool> var_pool_stack_;
    CLispFunctionPool             func_pool_;
};

#endif // LISP_INTERPRETER_H_INCLUDED

