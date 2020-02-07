#include "env.h"
#include "util.h"
#include <stdlib.h>


E_enventry E_VarEntry(Ty_ty ty, int isloopvar)
{
    E_enventry varEntry = checked_malloc(sizeof(*varEntry));
    varEntry->kind = E_varEntry;
    varEntry->u.var.ty = ty;
    varEntry->u.var.isloopvar = isloopvar;
    return varEntry;
}

E_enventry E_FunEntry(Ty_tyList formals, Ty_ty result)
{
    E_enventry funEntry = checked_malloc(sizeof(*funEntry));
    funEntry->kind = E_funEntry;
    funEntry->u.fun.formals = formals;
    funEntry->u.fun.result = result;
    return funEntry;
}

/* library functions */
S_table E_base_funset(void)
{
    S_table funset = S_empty();
    S_enter(funset, S_Symbol("print"), NULL);
    S_enter(funset, S_Symbol("flush"), NULL);
    S_enter(funset, S_Symbol("getchar"), NULL);
    S_enter(funset, S_Symbol("ord"), NULL);
    S_enter(funset, S_Symbol("chr"), NULL);
    S_enter(funset, S_Symbol("size"), NULL);
    S_enter(funset, S_Symbol("substring"), NULL);
    S_enter(funset, S_Symbol("concat"), NULL);
    S_enter(funset, S_Symbol("not"), NULL);
    S_enter(funset, S_Symbol("exit"), NULL);

    return funset;
}

S_table E_base_tenv(void)
{
    S_table tenv = S_empty();
    S_enter(tenv, S_Symbol("int"), Ty_Int());
    S_enter(tenv, S_Symbol("string"), Ty_String());
    return tenv;
}

S_table E_base_venv(void)
{
    S_table venv = S_empty();
    /* function print(s: string) */
    E_enventry env_print = E_FunEntry(Ty_TyList(Ty_String(), NULL), Ty_Void());
    /* function flush() */
    E_enventry env_flush = E_FunEntry(NULL, Ty_Void());
    /* functon getchar() : string */
    E_enventry env_getchar = E_FunEntry(NULL, Ty_String());
    /* function ord(s: string) : int */
    E_enventry env_ord = E_FunEntry(Ty_TyList(Ty_String(), NULL), Ty_Int());
    /* function chr(i: int) : string */
    E_enventry env_chr = E_FunEntry(Ty_TyList(Ty_Int(), NULL), Ty_String());
    /* function size(s: string) : int */
    E_enventry env_size = E_FunEntry(Ty_TyList(Ty_String(), NULL), Ty_Int());
    /* function substring(s: string, first: int, n: int) : string */
    E_enventry env_substring = E_FunEntry(Ty_TyList(Ty_String(), 
                                            Ty_TyList(Ty_Int(), 
                                                Ty_TyList(Ty_Int(), NULL))), Ty_String());
    /* function concat(s1: string, s2: string) : string */
    E_enventry env_concat = E_FunEntry(Ty_TyList(Ty_String(), 
                                        Ty_TyList(Ty_String(), NULL)), Ty_String());
    /* function not(i: int) : int */
    E_enventry env_not = E_FunEntry(Ty_TyList(Ty_Int(), NULL), Ty_Int());
    /* function exit(i: int) */
    E_enventry env_exit = E_FunEntry(Ty_TyList(Ty_Int(), NULL), Ty_Void());
    /* add to envirnoment */
    S_enter(venv, S_Symbol("print"), env_print);
    S_enter(venv, S_Symbol("flush"), env_flush);
    S_enter(venv, S_Symbol("getchar"), env_getchar);
    S_enter(venv, S_Symbol("ord"), env_ord);
    S_enter(venv, S_Symbol("chr"), env_chr);
    S_enter(venv, S_Symbol("size"), env_size);
    S_enter(venv, S_Symbol("substring"), env_substring);
    S_enter(venv, S_Symbol("concat"), env_concat);
    S_enter(venv, S_Symbol("not"), env_not);
    S_enter(venv, S_Symbol("exit"), env_exit);
    return venv;
}