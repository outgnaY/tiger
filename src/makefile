all: semanttest parsetest lextest absyntest tabletest

semanttest: semanttest.o semant.o y.tab.o lex.yy.o errormsg.o util.o symbol.o absyn.o table.o env.o types.o stack.o temp.o prabsyn.o
	cc -g -o semanttest semant.o semanttest.o y.tab.o lex.yy.o errormsg.o util.o symbol.o absyn.o table.o env.o types.o stack.o temp.o prabsyn.o

absyntest: absyntest.o prabsyn.o y.tab.o lex.yy.o errormsg.o util.o symbol.o absyn.o table.o
	cc -g -o absyntest absyntest.o prabsyn.o y.tab.o lex.yy.o errormsg.o util.o symbol.o absyn.o table.o

parsetest: parsetest.o y.tab.o lex.yy.o errormsg.o util.o symbol.o absyn.o table.o
	cc -g -o parsetest parsetest.o y.tab.o lex.yy.o errormsg.o util.o symbol.o absyn.o table.o

tabletest: tabletest.o table.o symbol.o util.o
	cc -g -o tabletest tabletest.o table.o symbol.o util.o
#stacktest: stack.o util.o
#	cc -g -o stack.o util.o

lextest: driver.o lex.yy.o errormsg.o util.o
	cc -g -o lextest driver.o lex.yy.o errormsg.o util.o

semanttest.o: semanttest.c semant.h absyn.h util.h errormsg.h
	cc -g -c semanttest.c

absyntest.o: absyntest.c util.h symbol.h absyn.h errormsg.h prabsyn.h
	cc -g -c absyntest.c

parsetest.o: parsetest.c errormsg.h util.h
	cc -g -c parsetest.c

tabletest.o: tabletest.c symbol.h table.h util.h
	cc -g -c tabletest.c

temp.o: temp.c temp.h util.h symbol.h table.h
	cc -g -c temp.c

stack.o: stack.c stack.h util.h
	cc -g -c stack.c

semant.o: semant.c semant.h types.h env.h stack.h temp.h prabsyn.h
	cc -g -c semant.c

env.o: env.c env.h util.h
	cc -g -c env.c

types.o: types.h util.h symbol.h
	cc -g -c types.c

prabsyn.o: prabsyn.c prabsyn.h symbol.h absyn.h util.h
	cc -g -c prabsyn.c

absyn.o: absyn.c absyn.h symbol.h util.h
	cc -g -c absyn.c

table.o: table.c table.h util.h
	cc -g -c table.c

symbol.o: symbol.c symbol.h util.h table.h
	cc -g -c symbol.c

driver.o: driver.c tokens.h errormsg.h util.h
	cc -g -c driver.c

y.tab.o: y.tab.c
	cc -g -c y.tab.c

y.tab.c: tiger.grm
	bison -ydvo $@ $<

y.tab.h: y.tab.c
	echo "y.tab.h was created at the same time as y.tab.c"

errormsg.o: errormsg.c errormsg.h util.h
	cc -g -c errormsg.c

lex.yy.o: lex.yy.c y.tab.h errormsg.h util.h
	cc -g -c lex.yy.c

lex.yy.c: tiger.lex
	lex tiger.lex

util.o: util.c util.h
	cc -g -c util.c

clean: 
	rm -f semanttest absyntest parsetest lextest lex.yy.c *.output y.tab.c y.tab.h *.o
