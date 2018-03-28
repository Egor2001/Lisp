#include <cstdlib>
#include <cstdio>

#include "Interpreter.h"

#define LISP_CONSOLEBUF_SIZE (64*1024)

//example:
//"(setq var 3) (add (sub var 0 1) (setq var 2) 1 (mul var 1))"

int main()
{
    char console_buf[LISP_CONSOLEBUF_SIZE] = "(setq var 3) (add (sub var 0 1) (setq var 2) 1 (mul var 1))";

    CLispInterpreter interpreter;

    printf("%u \n", interpreter.eval_program(console_buf));

    return 0;
}