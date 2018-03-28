#ifndef LISP_FUNCTION_POOL_H_INCLUDED
#define LISP_FUNCTION_POOL_H_INCLUDED

class CLispFunctionPool
{
public:
    struct SNode
    {
    public:
        SNode() = default;
        SNode             (const SNode&) = delete;
        SNode& operator = (const SNode&) = delete;

        SNode(SNode&& assign_node):
            name(""), args_name_vec(), list_pos(nullptr)
        {
            std::swap(*this, assign_node);
        }

        SNode& operator = (SNode&& assign_node)
        {
            std::swap(*this, assign_node);

            return (*this);
        }

        friend void swap(SNode& lhs, SNode& rhs)
        {
            using std::swap;

            swap(lhs.name,          rhs.name);
            swap(lhs.args_name_vec, rhs.args_name_vec);
            swap(lhs.list_pos,      rhs.list_pos);
        }

    public:
        char                                 name[LISP_NAMEBUF_SIZE];
        std::vector<char[LISP_NAMEBUF_SIZE]> args_name_vec;
        const char*                          list_pos;
    };

public:
    CLispFunctionPool() = default;

    CLispFunctionPool             (const CLispFunctionPool&) = delete;
    CLispFunctionPool& operator = (const CLispFunctionPool&) = delete;

    CLispFunctionPool(CLispFunctionPool&& assign_pool):
        node_vec_()
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
    const SNode& eval(char name[LISP_NAMEBUF_SIZE])
    {
        auto search_pred = [name](const SNode& node)->bool
        {
            return !strncasecmp(name, node.name, LISP_NAMEBUF_SIZE-1);
        };

        auto iter = std::find_if(node_vec_.begin(), node_vec_.end(), search_pred);

        LISP_REQUIRE_(iter != node_vec_.end(), "no function named 'name'")

        return (*iter);
    }

    void define(const char name_set[LISP_NAMEBUF_SIZE],
                std::vector<char[LISP_NAMEBUF_SIZE]>&& args_name_vec_set,
                const char* list_pos_set)
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
            new_node.list_pos = list_pos_set;

            size_t args_count = args_name_vec_set.size();
            new_node.args_name_vec.resize(args_count, {});

            for (size_t i = 0; i < args_count; ++i)
                memcpy(new_node.args_name_vec[i], args_name_vec_set[i], sizeof(char)*LISP_NAMEBUF_SIZE);

            node_vec_.emplace_back(std::move(new_node));
        }
        else
        {
            iter->list_pos = list_pos_set;
            iter->args_name_vec.clear();

            size_t args_count = args_name_vec_set.size();
            iter->args_name_vec.resize(args_count);

            for (size_t i = 0; i < args_count; ++i)
                memcpy(iter->args_name_vec[i], args_name_vec_set[i], sizeof(char)*LISP_NAMEBUF_SIZE);
        }
    }

private:
    std::vector<SNode> node_vec_;
};

#endif // LISP_FUNCTION_POOL_H_INCLUDED
