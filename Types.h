#ifndef LISP_TYPES_H_INCLUDED
#define LISP_TYPES_H_INCLUDED

#include <stdexcept>

#define LISP_REQUIRE_(cond, message) \
        if (!(cond)) throw std::invalid_argument(message);

#define LISP_NAMEBUF_SIZE 64

typedef uint32_t TLispNumber;

struct SLispSymbol
{
    char str[LISP_NAMEBUF_SIZE];
}

#endif // LISP_TYPES_H_INCLUDED
