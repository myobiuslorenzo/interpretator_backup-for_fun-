#include "interface.h"
#include "includes.h"
#include "StrCalc\strCalc.h"

void Precalculator::calculation(Lexeme& res, Lexeme& op1, Lexeme& oper, Lexeme& op2) {

}

/*void Precalculator::checkop(Lexeme& op1, Lexeme& oper, Lexeme& op2) {//it is assumed that we can't assign a char to int or smth
	if (op1.t != op2.t)throw(Error("Type mismatch")); //only one-type operators
													  //how fix which operations we can apply to this operands?
													  //const-strings you don't have to meat in this function
	Lexeme res;
	calculation(res, op1, oper, op2);
	Precalculator::Calc.push(res);
}

void Precalculator::checknot(Lexeme& op, Lexeme& sigh) {
	if (sigh.s == "-")
		if (op.s[0] != '-')op.s = "-" + op.s;
		else op.s[0] = 0;
		Precalculator::Calc.push(op);
}*/

ld Precalculator::calculate(string s) {
	ofstream ofs("InForCalc.txt");
	ofs << "\\calc -c\n"<<s<<endl<<"END";
	ofs.close();

	expr.clear();
	return Calc();
}

ld Precalculator::calculate() {
	ofstream ofs("InForCalc.txt");
	ofs << "\\calc -c\n" << expr << endl << "END";
	ofs.close();

	expr.clear();
	return Calc();
}