/*
 * parsetest.c - Parse test source file.
 */

#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "prabsyn.h"

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
		fprintf(stderr, "usage: a.out filename\n");
		exit(1);
	}
	if (parse(argv[1])) {
		pr_exp(stdout, absyn_root, 0);
		printf("\n");
	}
	return 0;
}