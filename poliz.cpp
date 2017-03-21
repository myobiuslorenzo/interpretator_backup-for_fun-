#include "interface.h"
#include "includes.h"

void Poliz::Write() {
	ifstream ifs("InputPoliz.txt");

	string rec;
	vector<string>str;
	while (ifs >> rec) {
		if (rec.find(";") != -1 || rec.find("}") != -1)
			str.clear();

		if (rec.find("if(") != -1 || rec == "if")
			Condition();

		if (rec == "cinout")
			Cinout('?');

		if (rec == "cinout<<")
			Cinout('O');

		if (rec == "cinout>>")
			Cinout('I');

		if(rec.find("=")!=-1)

		str.push_back(rec);
	}
}

void Poliz::Condition() {

}

void Poliz::Cinout(char state) {
	switch (state) {
		case 'O':
		{
			string s;
			while (s.find(";") != -1 || s.find("}") != -1) {

			}


		}
		default:
			break;
	}
}

void Poliz::Assignment() {

}

void Poliz::push(elemOfPoliz e) {
	pol.push_back(e);
}

void Poliz::push(string s, double d) {
	pol.push_back(elemOfPoliz(s, d));
}

void Poliz::push(string s, int  d) {
	pol.push_back(elemOfPoliz(s, d));
}

void Poliz::push(string s, char d) {
	pol.push_back(elemOfPoliz(s, d));
}

void Poliz::push(string s, bool d) {
	pol.push_back(elemOfPoliz(s, d));
}

void Poliz::push(double d) {
	pol.push_back(elemOfPoliz(d));
}

void Poliz::push(int d) {
	pol.push_back(elemOfPoliz(d));
}

void Poliz::push(char d) {
	pol.push_back(elemOfPoliz(d));
}

void Poliz::push(bool d) {
	pol.push_back(elemOfPoliz(d));
}

void Poliz::push(string d) {
	pol.push_back(elemOfPoliz(d));
}
template <class T>
void push_to_vector_from_file(vector<T>& Fnaf, string naf) {
	ifstream one.open(naf);
	while (!one.eof()) {
		string w;
		one >> w;
		Fnaf.push_back(w);
	}
	one.close();
}
template <class T>
bool find(T s, vector<T>& q) {
	for (auto t : q)if (t == s)return true;
	else return false;
}

void scan(vector<elemOfPoliz>& pol) {
	//vector<string> rightradical;//калькулятор сам же разберется с ассоциативностью или всеж нет?
	vector<string> unary_oper;
	//push_to_vector_from_file(rightradical, "rightradical.txt");
	push_to_vector_from_file(unary_oper, "unary_oper.txt");
	Precalculator precalc;
	size_t i = 0;
	Stack<elemOfPoliz> stack;// imagination
	while (i < pol.size()) {
		switch (pol[i].t) {
		case IDENT:
			stack.push(pol[i]);
			break;
		case CONST:
			stack.push(pol[i]);
			break;
		case OPER:
			if (!find(pol[i].oper, unary_oper)) {
				auto p = stack.pop();
				auto q = stack.pop();
				//
				auto elem = precalc.calculate(p.i.name + pol[i].oper + q.i.name);
				//
				if (pol[i].oper == "=") {
					//reverse current value of var in TID
				}
				Ident TM(elem);
				elemOfPoliz W(TM);
				stack.push(W);
			}
			else {
				if (pol[i].oper == "<<") {
					switch (pol[i].i.t) {
					case INT:
						cout << pol[i].i.i;
						break;
					case DOUBLE:
						cout << pol[i].i.d;
						break;
					case LONG_DOUBLE:
						cout << pol[i].i.ldd;
						break;
					case BOOL:
						cout << pol[i].i.b;
						break;
					case CHAR:
						cout << pol[i].i.c;
						break;
					case STRING:
						cout << pol[i].i.name;
						break;
					}
					//revese smth in TID
				}
				else if (pol[i].oper == ">>") {
					switch (pol[i].i.t) {
					case INT:
						cin >> pol[i].i.i;
						break;
					case DOUBLE:
						cin >> pol[i].i.d;
						break;
					case LONG_DOUBLE:
						cin >> pol[i].i.ldd;
						break;
					case BOOL:
						cin >> pol[i].i.b;
						break;
					case CHAR:
						cin >> pol[i].i.c;
						break;
					case STRING:
						cin >> pol[i].i.name;
						break;
					}
					//revese smth in TID?
				}
				else {
					auto p = stack.pop();
					auto elem = precalc.calculate(pol[i].oper + p.i.name);
					Ident TM(elem);
					elemOfPoliz W(TM);
					stack.push(W);
					//reverse smth value of var in TID
				}
			}
			break;
		case RETRANS: //условный переход
    	 {//почему оно ругается, если скобки "{}" убрать?
			elemOfPoliz q = stack.pop();
			elemOfPoliz w = stack.pop();
			if (q.i.b != true) {
				stack.clear();
				i = w.i.i-1;//ссылка на ячейку полиза(тоже инт же, не стал извращаться)
			}
		}
		break;
		case TRANS:  //безусловный переход. Он тоже нужен, и без него никак.
			auto a = stack.pop();
			stack.clear();
			i = a.i.i-1;//иначе оказаться можем на ячейку дальше, чем надо
			break;
		}
		i++;
	}
}
