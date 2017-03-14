#include "interface.h"
#include "includes.h"

bool semantic_analyzer::find(map<Lexeme, bool>& a, Lexeme& ident) {
	for (auto t : a)if (t.first.t == ident.t && t.first.s==ident.s)return true;
	return false;
}

/*void semantic_analyzer::push_name(Lexeme& ident) {
	if (!find(semantic_analyzer::TID_FOR_IDENT, ident))semantic_analyzer::TID_FOR_IDENT[ident]=1;
	else throw(Error("redefinition of" + ident.s));
}*/

void semantic_analyzer::push_name_in_set(Lexeme name) { //version of Yura
	if (STACK_FOR_SET_OF_IDENT.top().find(name.s) == STACK_FOR_SET_OF_IDENT.top().end())
		STACK_FOR_SET_OF_IDENT.top().insert(name.s);
	else
		throw(Error("redefinition of \"" + name.s+"\"", name.line));
}

void semantic_analyzer::find_name(Lexeme name) {
	if (STACK_FOR_SET_OF_IDENT.top().find(name.s) == STACK_FOR_SET_OF_IDENT.top().end())
		throw(Error("Variable \"" + name.s + "\" didn't find!", name.line));
}

