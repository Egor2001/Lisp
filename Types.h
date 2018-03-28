#ifndef LISP_TYPES_H_INCLUDED
#define LISP_TYPES_H_INCLUDED

#include <stdexcept>
#include <cstring>
#include <algorithm>

#define LISP_REQUIRE_(cond, message) \
        if (!(cond)) throw std::invalid_argument(message);

#define LISP_NAMEBUF_SIZE 64

typedef uint32_t TLispNumber;

struct SLispSymbol
{
public:
    SLispSymbol() = default;

    SLispSymbol(const SLispSymbol& assign_symb): str("")
    {
        memcpy(str, assign_symb.str, LISP_NAMEBUF_SIZE);
    }

    SLispSymbol& operator = (const SLispSymbol& assign_symb)
    {
        memcpy(str, assign_symb.str, LISP_NAMEBUF_SIZE);

        return (*this);
    }

    SLispSymbol(SLispSymbol&& assign_symb): str("")
    {
        swap(*this, assign_symb);
    }

    SLispSymbol& operator = (SLispSymbol&& assign_symb)
    {
        swap(*this, assign_symb);

        return (*this);
    }

public:
    friend void swap(SLispSymbol& lhs, SLispSymbol& rhs)
    {
        std::swap(lhs.str, rhs.str);
    }

    friend bool operator == (const SLispSymbol& lhs, const SLispSymbol& rhs)
    {
        return !strncasecmp(lhs.str, rhs.str, LISP_NAMEBUF_SIZE-1);
    }

public:
    char str[LISP_NAMEBUF_SIZE];
};

#endif // LISP_TYPES_H_INCLUDED
