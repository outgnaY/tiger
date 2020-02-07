#include "table.h"
#include "symbol.h"

int main(int argc, char **argv)
{
    int a = 1, b = 2, c = 3;
    S_table t = S_empty();
    S_enter(t, S_Symbol("a"), &a);
    S_enter(t, S_Symbol("b"), &b);
    S_enter(t, S_Symbol("c"), &c);
    S_free(t);
}