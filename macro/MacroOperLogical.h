LISP_HANDLE_OPER_LOGICAL_("eq",   [](TLispNumber lhs, TLispNumber rhs){ return lhs == rhs; })
LISP_HANDLE_OPER_LOGICAL_("neq",  [](TLispNumber lhs, TLispNumber rhs){ return lhs != rhs; })
LISP_HANDLE_OPER_LOGICAL_("lt",   [](TLispNumber lhs, TLispNumber rhs){ return lhs <  rhs; })
LISP_HANDLE_OPER_LOGICAL_("gt",   [](TLispNumber lhs, TLispNumber rhs){ return lhs >  rhs; })
LISP_HANDLE_OPER_LOGICAL_("lteq", [](TLispNumber lhs, TLispNumber rhs){ return lhs <= rhs; })
LISP_HANDLE_OPER_LOGICAL_("gteq", [](TLispNumber lhs, TLispNumber rhs){ return lhs >= rhs; })
