LISP_HANDLE_OPER_ARITHM_("add", [](TLispNumber lhs, TLispNumber rhs){ return lhs + rhs; })
LISP_HANDLE_OPER_ARITHM_("sub", [](TLispNumber lhs, TLispNumber rhs){ return lhs - rhs; })
LISP_HANDLE_OPER_ARITHM_("mul", [](TLispNumber lhs, TLispNumber rhs){ return lhs * rhs; })
LISP_HANDLE_OPER_ARITHM_("div", [](TLispNumber lhs, TLispNumber rhs){ return lhs / rhs; })
LISP_HANDLE_OPER_ARITHM_("mod", [](TLispNumber lhs, TLispNumber rhs){ return lhs % rhs; })
