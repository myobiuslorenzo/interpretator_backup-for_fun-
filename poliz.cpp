#include "interface.h"
#include "includes.h"

//hi

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