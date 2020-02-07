#include "semant.h"
#include "absyn.h"
#include "util.h"
#include "errormsg.h"
#include <stdlib.h>
#include <stdio.h>

extern int yyparse(void);
extern A_exp absyn_root;

A_exp parse(string fname)
{
    EM_reset(fname);
    if(yyparse() == 0)
        return absyn_root;
    else
        return NULL;    
}

int main(int argc, char **argv)
{
    if (argc != 2) {
		fprintf(stderr, "usage: semanttest filename\n");
		exit(1);
	}
	if (parse(argv[1])) {
		SEM_transProg(absyn_root);
	}
    return 0;
}