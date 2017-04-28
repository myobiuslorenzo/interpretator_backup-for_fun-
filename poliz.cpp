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
void Poliz::push(string s, elemOfPoliz::typeElemOfPoliz t, typeIdent type) {
	if (t == IDENT) {
		Var v;
		Var c = find(s);
		if (c.error == "error") {
			v.t = type;
			var.insert(make_pair(s, v));
		}
	}
	pol.push_back(elemOfPoliz(s,t));

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
Var Poliz::find(string s) {
	for(auto t:var) {
		if (t.first == s)return t.second;
	}
	Var v;
	v.error = "error";
	return v;
}
void Poliz::assign(elemOfPoliz& p, elemOfPoliz& q) {
	Var v=find(p.i.name);
	for (auto& t : var) {
		if (t.first == p.i.name) {
			//if (v.error != "error")t.second = v;
			//else {
				Var gg;
				t.second.ldd = q.i.ldd;
				t.second.i = q.i.i;
				t.second.b = q.i.b;
				t.second.c = q.i.c;
				t.second.d = q.i.d;

				v = find(p.i.name);
		//	}
		}
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
void Poliz::scan() {
	//vector<string> rightradical;//калькул€тор разберетс€ сам с правоассоциативными операци€ми ?
	vector<string> unary_oper;
	//push_to_vector_from_file(rightradical, "rightradical.txt");
	push_to_vector_from_file(unary_oper, "unary_oper.txt");
	Precalculator precalc;
	size_t i = 0;
	string ex;
	/*for (auto t : pol) {
		cout << t.i.name << ' ' << t.oper << ' ';
		for (auto y : t.expr)cout << y.s << ' ';
		cout << endl;
	}*/
	Stack<elemOfPoliz> stack;// imagination
	while (i < pol.size()) {
		switch (pol[i].t) {
		case EXPR:
		{
			ex = get_expr(pol[i].expr);
			ld elem;
			elem = precalc.calculate(ex);
			Ident TM(elem);
			elemOfPoliz W(TM);
			stack.push(W);
		}
			break;
		case IDENT:
			//cout << pol[i].i.name;
			stack.push(pol[i]);
			break;
		case CONST:
			stack.push(pol[i]);
			break;
		case OPER:
		{
			if (pol[i].oper == "<<") {
				elemOfPoliz u = stack.pop();
				Var x = find(u.i.name);
				if (x.error == "error") {
					switch (u.i.t) {
					case INT:
						cout << u.i.i;
						break;
					case DOUBLE:
						cout << u.i.d;
						break;
					case LONG_DOUBLE:
						cout << u.i.ldd;
						break;
					case BOOL:
					    cout << u.i.b;
						break;
					case CHAR:
						cout << u.i.c;
						break;
					case STRING:
					    cout << u.i.name;
						break;
					}
				}
				else {
					/*switch (x.t) {
					case INT:
						cout << x.i;
						break;
					case DOUBLE:
					    cout << x.d;
						break;
					case LONG_DOUBLE:
						cout << x.ldd;
						break;
					case BOOL:
						cout << x.b;
						break;
					case CHAR:
						cout << x.c;
						break;
					case STRING:
						cout << pol[i].i.name;
						break;
					}*/
					cout << x.ldd;
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
			} else if (pol[i].oper == "=") {
				elemOfPoliz u = stack.pop();
				elemOfPoliz w = stack.pop();
				assign(w, u);
			}else{
				elemOfPoliz u = stack.pop();
				elemOfPoliz w = stack.pop();
				bool t;
				switch (u.i.t) {
					case INT:
						if (pol[i].oper == ">")t = (u.i.i > w.i.i);
						if (pol[i].oper == "<")t = (u.i.i < w.i.i);
						if (pol[i].oper == "!=")t = (u.i.i != w.i.i);
						if (pol[i].oper == "==")t = (u.i.i == w.i.i);
						if (pol[i].oper == "&&")t = (u.i.i && w.i.i);
						if (pol[i].oper == "||")t = (u.i.i || w.i.i);
						break;
					case DOUBLE:
						if (pol[i].oper == ">")t = (u.i.d > w.i.d);
						if (pol[i].oper == "<")t = (u.i.d < w.i.d);
						if (pol[i].oper == "!=")t = (u.i.d != w.i.d);
						if (pol[i].oper == "==")t = (u.i.d == w.i.d);
						if (pol[i].oper == "&&")t = (u.i.d && w.i.d);
						if (pol[i].oper == "||")t = (u.i.d || w.i.d);
						break;
					case LONG_DOUBLE:
						if (pol[i].oper == ">")t = (u.i.ldd > w.i.ldd);
						if (pol[i].oper == "<")t = (u.i.ldd < w.i.ldd);
						if (pol[i].oper == "!=")t = (u.i.ldd != w.i.ldd);
						if (pol[i].oper == "==")t = (u.i.ldd == w.i.ldd);
						if (pol[i].oper == "&&")t = (u.i.ldd && w.i.ldd);
						if (pol[i].oper == "||")t = (u.i.ldd || w.i.ldd);
						break;
					case BOOL:
						if (pol[i].oper == ">")t = (u.i.b > w.i.b);
						if (pol[i].oper == "<")t = (u.i.b < w.i.b);
						if (pol[i].oper == "!=")t = (u.i.b != w.i.b);
						if (pol[i].oper == "==")t = (u.i.b == w.i.b);
						if (pol[i].oper == "&&")t = (u.i.b && w.i.b);
						if (pol[i].oper == "||")t = (u.i.b || w.i.b);
						break;
					case CHAR:
						if (pol[i].oper == ">")t = (u.i.c > w.i.c);
						if (pol[i].oper == "<")t = (u.i.c < w.i.c);
						if (pol[i].oper == "!=")t = (u.i.c != w.i.c);
						if (pol[i].oper == "==")t = (u.i.c == w.i.c);
						if (pol[i].oper == "&&")t = (u.i.c && w.i.c);
						if (pol[i].oper == "||")t = (u.i.c || w.i.c);
						break;
					case STRING:
						cout << "\n?????!\n";
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