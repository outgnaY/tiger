#ifndef SEMANT_H
#define SEMANT_H

#include "absyn.h"
#include "types.h"

typedef void* Tr_exp;
void SEM_transProg(A_exp exp);
struct expty {Tr_exp exp; Ty_ty ty;};

/* function prototypes */
struct expty expTy(Tr_exp exp, Ty_ty ty);
struct expty transVar(S_table venv, S_table tenv, A_var v);
struct expty transExp(S_table venv, S_table tenv, A_exp a);
void transDec(S_table venv, S_table tenv, A_dec d);
Ty_ty transTy(S_table tenv, A_ty a);

#endif /* SEMANT_H */