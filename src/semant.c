#include "semant.h"
#include "types.h"
#include "env.h"
#include "symbol.h"
#include "stack.h"
#include "temp.h"
#include <stdlib.h>
#define DEBUG 0 /* debug log */

static Ty_fieldList makeTyFieldList(S_table tenv, A_fieldList fieldList);
Stack s = NULL;
S_table funcenv = NULL;

void showTy(S_symbol sym, Ty_ty type) {
    if(S_ismark(sym)) {
        printf("marksym\n");
    }
    else {
        printf("type: ");
        Ty_print(type);
        printf("\n");
    }    
}
void showSym(S_symbol sym, E_enventry entry) {
    if(S_ismark(sym)) {
        printf("marksym\n");
    }
    else {
        if(entry->kind == E_funEntry) {
            printf("function: %s", S_name(sym));
            printf("      formals: ");
            TyList_print(entry->u.fun.formals);
            printf("      result: ");
            Ty_print(entry->u.fun.result);
            printf("\n");
        }
        else if(entry->kind == E_varEntry) {
            printf("var: %s", S_name(sym));
            printf("      type: ");
            Ty_print(entry->u.var.ty);
            printf("      isloop: %d\n", entry->u.var.isloopvar);        
        }
    }    
}

void SEM_transProg(A_exp exp)
{
    s = Stack_init();
    funcenv = E_base_funset();
    S_table tenv = E_base_tenv();
    S_table venv = E_base_venv();
    transExp(venv, tenv, exp);
}

/* check assign expression */
int checkAssignType(Ty_ty lType, Ty_ty rType) 
{
    Ty_ty actual_l = actual(lType);
    Ty_ty actual_r = actual(rType);
    int lKind = actual_l->kind;
    int rKind = actual_r->kind;
    /* right exp cannot be of type void */
    if(rKind == Ty_void)
        return FALSE;
    return ((lKind == Ty_record || lKind == Ty_array) && actual_l == actual_r) ||
    (lKind == Ty_record && rKind == Ty_nil) ||
    (lKind != Ty_record && lKind != Ty_array && lKind == rKind);
}

/* check if-then-else expression */
int checkIfType(Ty_ty thenType, Ty_ty elseType) {
    /* void or nil type in then and else allowed */
    Ty_ty actual_then = actual(thenType);
    Ty_ty actual_else = actual(elseType);
    int thenKind = actual_then->kind;
    int elseKind = actual_else->kind;
    return ((thenKind == Ty_record || thenKind == Ty_array) && actual_then == actual_else) ||
    (thenKind == Ty_record && elseKind == Ty_nil) ||
    (thenKind == Ty_nil && elseKind == Ty_record) ||
    (thenKind != Ty_record && elseKind != Ty_array && thenKind == elseKind);
}

struct expty expTy(Tr_exp exp, Ty_ty ty)
{
    struct expty e;
    e.exp = exp;
    e.ty = ty;
    return e;
}

struct expty transExp(S_table venv, S_table tenv, A_exp a)
{
    switch(a->kind) {
        case A_varExp: {
            return transVar(venv, tenv, a->u.var);
        }
        case A_nilExp: {
            return expTy(NULL, Ty_Nil());
        }
        case A_intExp: {
            return expTy(NULL, Ty_Int());
        }
        case A_stringExp: {
            return expTy(NULL, Ty_String());
        }
        case A_opExp: {
            A_oper oper = a->u.op.oper;
            struct expty left = transExp(venv, tenv, a->u.op.left);
            struct expty right = transExp(venv, tenv, a->u.op.right);
            /* actual types of left and right operands */
            Ty_ty leftActual = actual(left.ty);
            Ty_ty rightActual = actual(right.ty);
            switch(oper) {
                /* + - * / */
                case A_plusOp:
                case A_minusOp:
                case A_timesOp:
                case A_divideOp:
                    if(leftActual->kind != Ty_int) {
                        EM_error(a->u.op.left->pos, "int type required for left operand");
                    }
                    if(rightActual->kind != Ty_int) {
                        EM_error(a->u.op.right->pos, "int type required for right operand");
                    }
                    return expTy(NULL, Ty_Int());
                /* = <> */
                case A_eqOp:
                case A_neqOp:                  
                    /* neither left nor right operand can be of void type */
                    if(leftActual->kind == Ty_void || rightActual->kind == Ty_void) {
                        if(leftActual->kind = Ty_void) {
                            EM_error(a->u.op.left->pos, "type of left operand is void");
                        }
                        if(rightActual->kind == Ty_void) {
                            EM_error(a->u.op.right->pos, "type of right operand is void");
                        }
                    }                    
                    /* left and right operands cannot be of void type at the same time */
                    else if(leftActual->kind == Ty_nil && rightActual->kind == Ty_nil) {
                        EM_error(a->u.op.left->pos, "left and right operands cannot be of void type at the same time");
                    }
                    else if(leftActual->kind == Ty_record && rightActual->kind == Ty_nil) {
                        
                    }
                    else if(rightActual->kind == Ty_record && leftActual->kind == Ty_nil) {
                        
                    }
                    else if(leftActual->kind == rightActual->kind) {

                    }
                    else {
                        EM_error(a->u.op.left->pos, "left and right operand types are inconsistent");
                    }
                    return expTy(NULL, Ty_Int());
                /* < <= > >= */
                case A_ltOp:
                case A_leOp:
                case A_gtOp:
                case A_geOp:
                    if(leftActual->kind == Ty_int && rightActual->kind == Ty_int) {

                    }
                    else if(leftActual->kind == Ty_string && rightActual->kind == Ty_string) {

                    }
                    else {
                        EM_error(a->u.op.left->pos, "left and right operand should be of int or string type");
                    }
                    return expTy(NULL, Ty_Int());                
            }
        }
        
        case A_assignExp: {
            struct expty var = transVar(venv, tenv, a->u.assign.var);
            struct expty exp = transExp(venv, tenv, a->u.assign.exp);
            if(!checkAssignType(var.ty, exp.ty)) {
                EM_error(a->pos, "assign expression type mismatch");
                return expTy(NULL, Ty_Void());
            }
            if(a->u.assign.var->kind == A_simpleVar) {
                E_enventry simpleVarEntry = S_look(venv, a->u.assign.var->u.simple);
                /* check if it is LoopVarEntry */
                if(simpleVarEntry != NULL && simpleVarEntry->u.var.isloopvar) {
                    EM_error(a->u.assign.var->pos, "cannot assign value to loop variable");
                }
            }
            return expTy(NULL, Ty_Void());
        }        
        case A_ifExp: {
            struct expty test, then, elsee;
            test = transExp(venv, tenv, a->u.iff.test);
            if(test.ty->kind != Ty_int)
                EM_error(a->u.iff.test->pos, "int type required for test expression of if statement");
            then = transExp(venv, tenv, a->u.iff.then);
            if(a->u.iff.elsee) {
                elsee = transExp(venv, tenv, a->u.iff.elsee);
                if(!checkIfType(then.ty, elsee.ty)) {
                    EM_error(a->u.iff.elsee->pos, "if-then-else branches must return same type");
                }
                return expTy(NULL, then.ty);
            }
            else {
                if(then.ty->kind != Ty_void) {
                    EM_error(a->u.iff.then->pos, "if-then must produce no value");
                }
                return expTy(NULL, Ty_Void());
            }
        }
        case A_whileExp: {
            struct expty test = transExp(venv, tenv, a->u.whilee.test);
            if(test.ty->kind != Ty_int) {
                EM_error(a->u.whilee.test->pos, "int type required for test expression of while statement");
            }
            Stack_push(s, Temp_newlabel());
            struct expty body = transExp(venv, tenv, a->u.whilee.body);
            Stack_pop(s);
            if(body.ty->kind != Ty_void) {
                EM_error(a->u.whilee.body->pos, "while must produce no value");
            }
            return expTy(NULL, Ty_Void());
        }
        case A_seqExp: {
            A_expList seq = a->u.seq;
            int empty = FALSE;
            struct expty lastExp = expTy(NULL, Ty_Void());
            if(seq == NULL) {
                empty = TRUE;
            }
            for(; seq; seq = seq->tail) {
                lastExp = transExp(venv, tenv, seq->head);
            }
            if(empty) {
                return expTy(NULL, Ty_Void());
            }
            else {
                return lastExp;
            }
        }
        case A_breakExp: {
            if(Stack_empty(s)) {
                EM_error(a->pos, "break expression must be inside loop");
            }
            return expTy(NULL, Ty_Void());
        }
        case A_forExp: {
            struct expty lo = transExp(venv, tenv, a->u.forr.lo);
            struct expty hi = transExp(venv, tenv, a->u.forr.hi);
            if(lo.ty->kind != Ty_int) {
                EM_error(a->u.forr.lo->pos, "int type required for lower limit of for expression");
            }
            if(hi.ty->kind != Ty_int) {
                EM_error(a->u.forr.hi->pos, "int type required for upper limit of for expression");
            }            
            S_beginScope(venv);
            /* put loop var entry into env */
            S_enter(venv, a->u.forr.var, E_VarEntry(Ty_Int(), TRUE));
            #if DEBUG
            printf("after put loop var\n");
            S_show(venv, showSym);
            #endif
            Stack_push(s, Temp_newlabel());
            struct expty body = transExp(venv, tenv, a->u.forr.body);
            if(body.ty->kind != Ty_void) {
                EM_error(a->u.forr.body->pos, "body of for expression must produce no value");
            }
            Stack_pop(s);
            S_endScope(venv);
            #if DEBUG
            printf("exit for body\n");
            S_show(venv, showSym);
            #endif
            return expTy(NULL, Ty_Void());
        }
        case A_arrayExp: {
            Ty_ty ty = S_look(tenv, a->u.array.typ);
            if(ty)
                ty = actual(ty);
            if(!ty) {                
                EM_error(a->pos, "undefined array type");
                return expTy(NULL, Ty_Int());
            }
            /* to avoid segment fault */
            else if(ty->kind != Ty_array) {
                EM_error(a->pos, "wrong array type");
                return expTy(NULL, Ty_Int());
            }
            else {              
                struct expty size = transExp(venv, tenv, a->u.array.size);
                struct expty init = transExp(venv, tenv, a->u.array.init);
                if(size.ty->kind != Ty_int) {
                    EM_error(a->u.array.size->pos, "int type required for size expression of array");
                }               
                if(!checkAssignType(ty->u.array, init.ty)) {
                    EM_error(a->u.array.init->pos, "array initialization type inconsistent");
                }                               
                return expTy(NULL, actual(ty));
            }
        }        
        case A_recordExp: {
            Ty_ty ty = S_look(tenv, a->u.record.typ);
            if(ty)
                ty = actual(ty);
            if(!ty) {
                EM_error(a->pos, "undefined record type");
                return expTy(NULL, Ty_Int());
            }
            /* to avoid segment fault */
            else if(ty->kind != Ty_record) {
                EM_error(a->pos, "wrong record type");
                return expTy(NULL, Ty_Int());
            }
            else {
                /* field types from symbol table */
                Ty_fieldList tyFieldList = ty->u.record;
                /* field list from record expression */
                A_efieldList recList = a->u.record.fields;
                /* fields must be initialized in order */
                for(; recList; recList = recList->tail, tyFieldList = tyFieldList->tail) {
                    struct expty e = transExp(venv, tenv, recList->head->exp);
                    if(recList->head->name != tyFieldList->head->name) {
                        EM_error(a->pos, "invdlid field name %s", S_name(recList->head->name));
                    }
                    if(!checkAssignType(tyFieldList->head->ty, e.ty)) {
                        EM_error(recList->head->exp->pos, "type mismatch");
                    }
                }
                return expTy(NULL, actual(ty));
            }
        }
        case A_letExp: {
            struct expty e;
            A_decList d = a->u.let.decs;
            S_beginScope(venv);
            S_beginScope(tenv);
            for(; d; d = d->tail) {
                transDec(venv, tenv, d->head);
            }
            e = transExp(venv, tenv, a->u.let.body);
            S_endScope(tenv);
            S_endScope(venv);
            #if DEBUG
            printf("exit let expression, tenv: \n");
            S_show(tenv, showTy);
            printf("exit let expression, venv: \n");
            S_show(venv, showSym);
            #endif
            return expTy(NULL, e.ty);
        }
        case A_callExp: {
            E_enventry func = S_look(venv, a->u.call.func);
            if(!(func && func->kind == E_funEntry)) {
                EM_error(a->pos, "undefined function %s", S_name(a->u.call.func));
                return expTy(NULL, Ty_Void());
            }
            else {
                /* check formals */
                Ty_tyList formalList = func->u.fun.formals;
                A_expList argList = a->u.call.args;
                for(; formalList && argList; formalList = formalList->tail, argList = argList->tail) {
                    struct expty arg = transExp(venv, tenv, argList->head);
                    if(!checkAssignType(formalList->head, arg.ty)) {
                        EM_error(argList->head->pos, "function parameter type mismatch");
                    }
                }                
                if(formalList == NULL && argList != NULL) {
                    EM_error(a->pos, "too many arguments");                    
                }
                else if(formalList != NULL && argList == NULL) {
                    EM_error(a->pos, "too few arguments");
                }
                return expTy(NULL, actual(func->u.fun.result));                
            }
        }
        
    }
}

struct expty transVar(S_table venv, S_table tenv, A_var v) 
{
    switch(v->kind) {
        case A_simpleVar: {
            E_enventry var = S_look(venv, v->u.simple);
            if(var && var->kind == E_varEntry) {
                return expTy(NULL, actual(var->u.var.ty));
            }
            else {
                EM_error(v->pos, "undefined variable %s", S_name(v->u.simple));
                return expTy(NULL, Ty_Int());
            }
        }
        case A_fieldVar: {
            struct expty rec = transVar(venv, tenv, v->u.field.var);
            if(rec.ty->kind != Ty_record) {
                EM_error(v->u.field.var->pos, "not a record type");
            }
            else {
                Ty_fieldList fl = rec.ty->u.record;
                for(; fl; fl = fl->tail) {
                    if(fl->head->name == v->u.field.sym) {
                        return expTy(NULL, actual(fl->head->ty));
                    }
                }
                EM_error(v->u.field.var->pos, "no such field %s for record type", S_name(v->u.field.sym));
            }
            return expTy(NULL, Ty_Int());
        }
        case A_subscriptVar: {
            struct expty arr = transVar(venv, tenv, v->u.subscript.var);
            if(arr.ty->kind != Ty_array) {
                EM_error(v->u.subscript.var->pos, "not an array type");
                return expTy(NULL, Ty_Int());
            }
            else {
                struct expty index = transExp(venv, tenv, v->u.subscript.exp);
                if(index.ty->kind != Ty_int) {
                    EM_error(v->u.subscript.exp->pos, "int required for array index");
                }
                return expTy(NULL, actual(arr.ty->u.array));
            }
        }
    }
}

Ty_ty transTy(S_table tenv, A_ty a)
{
    switch(a->kind) {
        case A_nameTy: { 
            Ty_ty nameTy = S_look(tenv, a->u.name);
            if(!nameTy) {
                EM_error(a->pos, "undefine type %s", S_name(a->u.name));
            }
            return nameTy;
        }
        case A_recordTy: {
            Ty_fieldList tyFieldList = makeTyFieldList(tenv, a->u.record);
            return Ty_Record(tyFieldList);
        }
        case A_arrayTy: {
            Ty_ty arrayTy = S_look(tenv, a->u.name);
            if(!arrayTy) {
                EM_error(a->pos, "undefined type %s", S_name(a->u.name));
            }
            return Ty_Array(arrayTy);
        }
    }
}
/* make type field list for record type */
static Ty_fieldList makeTyFieldList(S_table tenv, A_fieldList fieldList) {
    Ty_fieldList tyFieldList = NULL;
    Ty_fieldList tailList = NULL;
    for(; fieldList; fieldList = fieldList->tail) {
        Ty_ty t = S_look(tenv, fieldList->head->typ);
        if(!t) {
            EM_error(fieldList->head->pos, "undefined type %s", S_name(fieldList->head->typ));
        }
        else {
            Ty_field f = Ty_Field(fieldList->head->name, t);
            if(tyFieldList) {
                /* link to tail */
                tailList->tail = Ty_FieldList(f, NULL);
                tailList = tailList->tail;
            }
            else {
                tyFieldList = Ty_FieldList(f, NULL);
                tailList = tyFieldList;
            }
        }
    }
    return tyFieldList;
}
static Ty_tyList makeTyList(S_table tenv, A_fieldList fieldList) {
    Ty_tyList paramTyList = NULL;
    Ty_tyList tailList = NULL;
    for(; fieldList; fieldList = fieldList->tail) {
        Ty_ty ty = S_look(tenv, fieldList->head->typ);
        if(!ty) {
            EM_error(fieldList->head->pos, "undefined parameter type %s", S_name(fieldList->head->typ));
        }
        else {
            if(paramTyList) {
                tailList->tail = Ty_TyList(ty, NULL);
                tailList = tailList->tail;
            }
            else {
                paramTyList = Ty_TyList(ty, NULL);
                tailList = paramTyList;
            }            
        }
    }
    return paramTyList;
}

void transDec(S_table venv, S_table tenv, A_dec d) {
    switch(d->kind) {        
        case A_varDec: {          
            struct expty init = transExp(venv, tenv, d->u.var.init);
            if(init.ty->kind == Ty_void) {
                EM_error(d->u.var.init->pos, "invalid initialization expression");
            }
            if(d->u.var.typ) {
                Ty_ty type = S_look(tenv, d->u.var.typ);
                if(!type) {
                    EM_error(d->pos, "undefined type for var initialization");
                    type = Ty_Void();
                }
                if(!checkAssignType(type, init.ty)) {
                    EM_error(d->pos, "type mismatch for var initialzation");
                }
                S_enter(venv, d->u.var.var, E_VarEntry(actual(type), FALSE));
                #if DEBUG
                printf("put var into table, with type\n");
                S_show(venv, showSym);
                #endif
            }
            else {
                if(init.ty->kind == Ty_nil) {
                    EM_error(d->u.var.init->pos, "cannot initialize with nil");
                }
                S_enter(venv, d->u.var.var, E_VarEntry(init.ty, FALSE));
                #if DEBUG
                printf("put var into table, without type\n");
                S_show(venv, showSym);
                #endif
            }            
           break;            
        }
        
        case A_typeDec: {
            A_nametyList nameTyList;
            /* avoid duplicated type declaration in a list */
            S_table temp = S_empty(); 
            /* process headers */
            for(nameTyList = d->u.type; nameTyList; nameTyList = nameTyList->tail) {
                /* check duplicated type declaration */                              
                if(S_contain(temp, nameTyList->head->name)) {
                    EM_error(d->pos, "duplicated type declaration in a list");
                }    
                S_enter(temp, nameTyList->head->name, NULL);
                S_enter(tenv, nameTyList->head->name, Ty_Name(nameTyList->head->name, NULL));
                #if DEBUG
                printf("put new type into table\n");
                S_show(tenv, showTy);
                #endif
            }
            /* free memory */
            S_free(temp);  
            /* process bodies */
            for(nameTyList = d->u.type; nameTyList; nameTyList = nameTyList->tail) {
                Ty_ty t = transTy(tenv, nameTyList->head->ty);
                Ty_ty nameTy = S_look(tenv, nameTyList->head->name);
                nameTy->u.name.ty = t;                
            }
            
            /* check loop in type decs */            
            for(nameTyList = d->u.type; nameTyList; nameTyList = nameTyList->tail) {
                int loop = isLoop(S_look(tenv, nameTyList->head->name));
                if(loop) {
                    EM_error(d->pos, "found loop in type decs");
                }                
            }
            break;
        }

        case A_functionDec: {
            A_fundecList funDecList;            
            /* avoid function declaration in a list */
            S_table temp = S_empty();
            /* process headers */
            for(funDecList = d->u.function; funDecList; funDecList = funDecList->tail) {
                Ty_ty resultTy = NULL;
                /* check duplicated function declaration */
                if(S_contain(temp, funDecList->head->name)) {
                    EM_error(funDecList->head->pos, "duplicated function declaration");
                }
                /* check for name conflicts with standard library functions */
                if(S_contain(funcenv, funDecList->head->name)) {
                    EM_error(funDecList->head->pos, "name conflict with standard library function %s", S_name(funDecList->head->name));
                }
                if(funDecList->head->result) {
                    resultTy = S_look(tenv, funDecList->head->result);
                    if(!resultTy) {
                        EM_error(funDecList->head->pos, "undefined return type");
                    }
                }
                if(!resultTy) {
                    resultTy = Ty_Void();
                }
                /* make formal type list */
                Ty_tyList formalTyList = makeTyList(tenv, funDecList->head->params);

                S_enter(temp, funDecList->head->name, NULL);
                S_enter(venv, funDecList->head->name, E_FunEntry(formalTyList, resultTy));
                #if DEBUG
                printf("after put function into table\n");
                S_show(venv, showSym);
                #endif
            }
            /* free memory */
            S_free(temp);
            /* process function bodies */
            for(funDecList = d->u.function; funDecList; funDecList = funDecList->tail) {
                E_enventry funEntry = S_look(venv, funDecList->head->name);
                Ty_tyList formalTyList = funEntry->u.fun.formals;
                A_fieldList params = funDecList->head->params;
                Ty_ty resultTy = funEntry->u.fun.result;
                S_beginScope(venv);
                /* store formals */
                for(; params && formalTyList; params = params->tail, formalTyList = formalTyList->tail) {
                    S_enter(venv, params->head->name, E_VarEntry(formalTyList->head, FALSE));
                }
                #if DEBUG
                printf("after put formal list of function\n");
                S_show(venv, showSym);
                #endif
                struct expty e = transExp(venv, tenv, funDecList->head->body);
                if(!(checkAssignType(resultTy, e.ty) || (resultTy->kind == Ty_void && e.ty->kind == Ty_void))) {
                    EM_error(funDecList->head->body->pos, "return type mismatch");
                }
                S_endScope(venv);
                #if DEBUG
                printf("exit function dec\n");
                S_show(venv, showSym);
                #endif
            }
            break;
        }   
    }
}

