#ifndef LISP_FUNCTION_POOL_H_INCLUDED
#define LISP_FUNCTION_POOL_H_INCLUDED

#include <vector>

#include "Types.h"

class CLispFunctionPool
{
public:
    struct SNode
    {
    public:
        SNode() = default;

        SNode(const SLispSymbol& name_set, const std::vector<SLispSymbol>& args_name_vec_set,
              const char* list_pos_set):
              name(name_set), args_name_vec(args_name_vec_set), list_pos(list_pos_set)
        {}

        SNode             (const SNode&) = delete;
        SNode& operator = (const SNode&) = delete;

        SNode(SNode&& assign_node):
            name(), args_name_vec(), list_pos(nullptr)
        {
            swap(*this, assign_node);
        }

        SNode& operator = (SNode&& assign_node)
        {
            swap(*this, assign_node);

            return (*this);
        }

        friend void swap(SNode& lhs, SNode& rhs)
        {
            std::swap(lhs.name,          rhs.name);
            std::swap(lhs.args_name_vec, rhs.args_name_vec);
            std::swap(lhs.list_pos,      rhs.list_pos);
        }

    public:
        SLispSymbol              name;
        std::vector<SLispSymbol> args_name_vec;
        const char*              list_pos;
    };

public:
    CLispFunctionPool() = default;

    CLispFunctionPool             (const CLispFunctionPool&) = delete;
    CLispFunctionPool& operator = (const CLispFunctionPool&) = delete;

    CLispFunctionPool(CLispFunctionPool&& assign_pool): node_vec_()
    {
        std::swap(node_vec_, assign_pool.node_vec_);
    }

    CLispFunctionPool& operator = (CLispFunctionPool&& assign_pool)
    {
        std::swap(node_vec_, assign_pool.node_vec_);

        return (*this);
    }

    ~CLispFunctionPool()
    {
        for (auto& node : node_vec_)
            node.args_name_vec.clear();

        node_vec_.clear();
    }

public:
    const SNode& eval(const SLispSymbol& name)
    {
        auto search_pred = [name](const SNode& node) { return (name == node.name); };
        auto iter = std::find_if(node_vec_.begin(), node_vec_.end(), search_pred);

        LISP_REQUIRE_(iter != node_vec_.end(), "no function named 'name'")

        return (*iter);
    }

    void define(const SLispSymbol& name_set,
                std::vector<SLispSymbol>&& args_name_vec_set,
                const char* list_pos_set)
    {
        auto search_pred = [name_set](const SNode& node) { return (name_set == node.name); };
        auto iter = std::find_if(node_vec_.begin(), node_vec_.end(), search_pred);

        if (iter == node_vec_.end())
        {
            node_vec_.push_back({ name_set, args_name_vec_set, list_pos_set });
        }
        else
        {
            iter->list_pos      = list_pos_set;
            iter->args_name_vec = args_name_vec_set;
        }
    }

private:
    std::vector<SNode> node_vec_;
};

#endif // LISP_FUNCTION_POOL_H_INCLUDED
