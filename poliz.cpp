#include "interface.h"
#include "includes.h"
#include <sstream>

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

void Poliz::push(vector<Lexeme> v) {
	pol.push_back(elemOfPoliz(v));
}

string Poliz::get(Var& v) {
	stringstream ss;
	switch (v.t) {
	case INT:
		ss<< v.i;
		break;
	case DOUBLE:
		ss << v.d;
		break;
	case LONG_DOUBLE:
		ss << v.ldd;
		break;
	case CHAR:
		ss << v.c;
		break;
	case BOOL:
		ss << v.b;
		break;
	}
	string s;
	 ss >> s;
	return s;
}

template <class T>
void push_to_vector_from_file(vector<T>& Fnaf, string naf) {
	ifstream one;
	one.open(naf);
	while (!one.eof()) {
		string w;
		one >> w;
		Fnaf.push_back(w);
	}
	one.close();
}
bool Poliz::find(string s, vector<string>& q) {
	for (auto t : q)if (t == s)return true;
	else return false;
}
Var Poliz::find(string s,int r=0) {
	for(auto t:var) {
		if (t.first == s)return t.second;
	}
	r = -1;
}
void Poliz::assign(elemOfPoliz& p, elemOfPoliz& q) {
	for (auto t : var) {
		if (t.first == p.i.name)t.second = find(q.i.name);
	}
}
void Poliz::assign(elemOfPoliz& p, Var& q) {
	for (auto t : var) {
		if (t.first == p.i.name)t.second = q;
	}
}
bool Poliz::find_var(string s, Var& v) {
	for (auto t : var) {
		if (t.first == s) {
			v = t.second;
			return true;
		}
	}
	return false;
}
string Poliz::get_expr(vector<Lexeme>& expr) {
	string s,res="";
	for (int i = 0; i < expr.size(); ++i) {
		Var V;
		if (expr[i].t==IDENT && find_var(expr[i].s, V)) {
			s = get(V);
			res += s;
		}
		else {
			res += expr[i].s;
		}
	}
	return res;
}
bool Poliz::logic(string s) {
	return 0;
}
void Poliz::scan() {
	//vector<string> rightradical;//калькул€тор разберетс€ сам с правоассоциативными операци€ми ?
	vector<string> unary_oper;
	//push_to_vector_from_file(rightradical, "rightradical.txt");
	push_to_vector_from_file(unary_oper, "unary_oper.txt");
	Precalculator precalc;
	size_t i = 0;
	string ex;
	Stack<elemOfPoliz> stack;// imagination
	//cout << "trololo";
	while (i < pol.size()) {
		switch (pol[i].t) {
		case EXPR:
		{
		//	cout << "1";
			ex = get_expr(pol[i].expr);
			ld elem;
			cout << ex << endl;
			elem = precalc.calculate(ex);
			cout << elem << endl;
			Ident TM(elem);
			elemOfPoliz W(TM);
			stack.push(W);
		}
			break;
		case IDENT:
			//cout << "2";
			stack.push(pol[i]);
			break;
		case CONST:
			//cout << "3";
			stack.push(pol[i]);
			break;
		case OPER:
		{
			if (pol[i].oper == "<<") {
				elemOfPoliz u = stack.pop();
				int r = 0;
				Var x = find(u.oper,r);
				switch (u.i.t) {
				case INT: 
					if(u.t==TRANS)cout << x.i;
					else cout << u.i.i;
					break;
				case DOUBLE:
					if (u.t == TRANS)cout << x.d;
					else cout << u.i.d;
					break;
				case LONG_DOUBLE:
					if (u.t == TRANS)cout << x.ldd;
					else cout << u.i.ldd;
					break;
				case BOOL:
					if (u.t == TRANS)cout << x.b;
					else cout << u.i.b;
					break;
				case CHAR:
					if (u.t == TRANS)cout << x.c;
					else cout << u.i.c;
					break;
				case STRING:
					if (u.t == TRANS)cout << pol[i].i.name;
					else cout << u.i.name;
					break;
				}
			}
			else if (pol[i].oper == ">>") {
				elemOfPoliz u = stack.pop();
				Var z;
				switch (u.i.t) {
				case INT:
					cin >> z.i;
					assign(pol[i], z);
					break;
				case DOUBLE:
					cin >> z.d;
					assign(pol[i], z);
					break;
				case LONG_DOUBLE:
					cin >> z.ldd;
					assign(pol[i], z);
					break;
				case BOOL:
					cin >> z.b;
					assign(pol[i], z);
					break;
				case CHAR:
					cin >> z.c;
					assign(pol[i], z);
					break;
				case STRING:
					cin >> pol[i].i.name;
					break;
				}
			}
		}
			break;
		case TRANSONLIE: //условный переход
		//	cout << "5";
    	 {//почему оно ругаетс€, если скобки "{}" убрать?
			elemOfPoliz q = stack.pop();
			elemOfPoliz w = stack.pop();
			if (q.i.b != true) {
				stack.clear();
				i = w.i.i-1;//ссылка на €чейку полиза(тоже инт же, не стал извращатьс€)
			}
		}
		break;
		case TRANS:  //безусловный переход. ќн тоже нужен, и без него никак.
			//cout << "6";
			auto a = stack.pop();
			stack.clear();
			i = a.i.i-1;//иначе оказатьс€ можем на €чейку дальше, чем надо
			break;
		}
		//cout << "end\n";
		i++;
	}
}