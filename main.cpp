#include <cstdlib>
#include <cstdio>

#include "Interpreter.h"

#define LISP_CONSOLEBUF_SIZE (64*1024)

int main()
{
    char console_buf[LISP_CONSOLEBUF_SIZE] =
    "(defun fact (num)"
    "("
    "   (cond "
    "       ((gt num 0) (mul (fact (sub num 1)) num))"
    "       (1 1)"
    "   )"
    ")"
    ")"
    "(fact 6)";

    CLispInterpreter interpreter;

    printf("%d \n", interpreter.eval_program(console_buf));

    return 0;
}
