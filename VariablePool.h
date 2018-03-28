#ifndef LISP_VARIABLE_POOL_H_INCLUDED
#define LISP_VARIABLE_POOL_H_INCLUDED

#include <vector>
#include <cstring>

#include "Types.h"

class CLispVariablePool
{
public:
    struct SNode
    {
        char        name[LISP_NAMEBUF_SIZE];
        TLispNumber value;
    };

public:
    CLispVariablePool() = default;

    CLispVariablePool             (const CLispVariablePool&) = delete;
    CLispVariablePool& operator = (const CLispVariablePool&) = delete;

    CLispVariablePool(CLispVariablePool&& assign_pool):
        node_vec_()
    {
        std::swap(node_vec_, assign_pool.node_vec_);
    }

    CLispVariablePool& operator = (CLispVariablePool&& assign_pool)
    {
        std::swap(node_vec_, assign_pool.node_vec_);

        return (*this);
    }

    ~CLispVariablePool()
    {
        node_vec_.clear();
    }

public:
    const SNode& eval(char name[LISP_NAMEBUF_SIZE])
    {
        auto search_pred = [name](const SNode& node)->bool
        {
            return !strncasecmp(name, node.name, LISP_NAMEBUF_SIZE-1);
        };

        auto iter = std::find_if(node_vec_.begin(), node_vec_.end(), search_pred);

        LISP_REQUIRE_(iter != node_vec_.end(), "no variable named 'name'")

        return (*iter);
    }

    void define(const char name_set[LISP_NAMEBUF_SIZE],
                TLispNumber value_set)
    {
        auto search_pred = [name_set](const SNode& node)->bool
        {
            return !strncasecmp(name_set, node.name, LISP_NAMEBUF_SIZE-1);
        };

        auto iter = std::find_if(node_vec_.begin(), node_vec_.end(), search_pred);

        if (iter == node_vec_.end())
        {
            SNode new_node = {};

            memcpy(new_node.name, name_set, sizeof(char)*LISP_NAMEBUF_SIZE);
            new_node.value = value_set;

            node_vec_.push_back(new_node);
        }
        else
        {
            iter->value = value_set;
        }
    }

private:
    std::vector<SNode> node_vec_;
};

#endif // LISP_VARIABLE_POOL_H_INCLUDED
