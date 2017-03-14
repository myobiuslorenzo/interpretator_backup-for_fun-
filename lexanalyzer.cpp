#include "interface.h"
#include "includes.h"
int LecsAnalyzer::findpos(int coord, vector<pair<int, int> >& lines) {
	int t_0 = 1;
	for (auto t : lines) {
		if (coord >= t.first) t_0 = t.second;
	}
	return t_0;
}
bool LecsAnalyzer::find(char c, string& s) {// стрёмные ф-ции.
	for (auto t : s)if (t == c)return true;
	return false;
}
bool LecsAnalyzer::find(string c, vector<string>& s) {
	for (auto t : s)if (t == c)return true;
	return false;
}
void LecsAnalyzer::regex_s(string s, regex& rgx, typeLex type, string& str, vector<pair<int, int> >& lines) {
	smatch m; int ind = 0;
	while (regex_search(s, m, rgx)) {
		regex_search(s, m, rgx);
		for (size_t i = 0; i<m.size(); ++i) {
			string x = m[i];
			bool t = false; int count = 0;
			if (type == CONST) {
				string x1;
				if (x[0]<'0' || x[0]>'9')count = 1;
				for (size_t i = 0; i < x.size(); ++i) {
					if (x[i] >= '0' && x[i] <= '9')x1.push_back(x[i]);
					if (x[i] >= 'a' && x[i] <= 'z' || x[i] >= 'A' && x[i] <= 'Z')t = true;
				}
				x = x1;
			}
			if (!t) {
				Lexeme L(type, x, ind + m.position(i) + count, findpos(ind + m.position(i) + count, lines));
				BOL.Add(L);
			}
			ind += m.position(i);
			ind += x.size();
			count = 0;
			s = m.suffix().str();
		}
	}
}
void LecsAnalyzer::regex_s(string ss, regex& rgx, typeLex type, set<string>& serv, vector<pair<int, int> >& lines) {
	smatch m; int ind = 0;
	while (regex_search(ss, m, rgx)) {
		regex_search(ss, m, rgx);
		string x = m[0];
		if (serv.find(x) != serv.end())type = SERV;
		if (x[0] >= '0' && x[0] <= '9')type = DIFF;
		Lexeme L(type, x, ind + m.position(0), findpos(ind + m.position(0), lines));
		BOL.Add(L);
		ind += m.position(0) + x.size();
		type = IDENT;
		ss = m.suffix().str();
	}
}
int LecsAnalyzer::Analyze() {
	set<string>serv;//список всех служебных слов
	FILE * in = fopen("Serves.txt", "r");
	string buff;
	string str = "!abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ(){}_.,+;:=-*><//[]0123456789 \n\t";

	for (char c = fgetc(in); c != EOF; c = fgetc(in)) {
		if (c == '\n') {
			serv.insert(buff);
			buff.clear();
			c = fgetc(in);
		}
		buff.push_back(c);
	}
	fclose(in);
	/////////////////////////////////////////////////////
	regex name("[\\w]*[a-zA-Z_]+[\\w]*");//ищет подозрительные на имена объекты
	regex oper("[-=>!+<\\*\\\\&.\\/]{1}");//operations
	regex digit("[0123456789]+");
	regex punct("[\\(\\),:;\\{\\}]{1}");
	string s = " ", sc = "";
	bool q = true; int j = 0, ind = 0;
	vector<pair<int, int> >lines;
	int count_of_lines = 1;
	int ix = 0;
	in = fopen("Input.txt", "a");
	fprintf(in, "%c",'\n');
	fclose(in);
	in = fopen("Input.txt", "r");
	for (char c = fgetc(in); c != EOF; c = fgetc(in)) {//пихание штук в кавычках в констаты и очищение от них строки
		if (c == '\n') {
			count_of_lines++;
			lines.push_back(make_pair(ix, count_of_lines));
		}
		if (!q && c != '"') {
			sc.push_back(c);
			if (sc.size() == 1)ind = j;
		}
		else if (q && c != '"')s.push_back(c);
		if (q && (sc.size() > 0)) {
			Lexeme L(CONST, sc, ind-1, findpos(ind, lines));
			L.rev();
			sc.clear();
			ind += sc.size()-1;
			BOL.Add(L);
		}
		if (c == '"')q = !q;
		j++; ix++;
	}
	if (!q) {// если вдруг кавычки не закрылись, тотал feil
		throw(Error("unclosed quotes"));
		return 1;
	}
	fclose(in);
	if (s == " ")throw (Error("empty program"));
	for (size_t i = 0; i < s.size(); ++i) {//творят страшные вещи, слипаются, как пельмени, по сему обрабатываются отдельно
		if (s[i] == '^') {
			Lexeme L(OPER, s[i], i, findpos(i, lines));
			BOL.Add(L);
			s[i] = ' ';
		}
	}
	vector<string>mas;
	string sr = "+=";
	mas.push_back(sr);
	sr = "-=";
	mas.push_back(sr);
	sr = ">=";
	mas.push_back(sr);
	sr = "<=";
	mas.push_back(sr);
	string punc = "+-=&|><";
	for (size_t i = 0; i < punc.size(); ++i) {
		string sr;
		sr.push_back(punc[i]); sr.push_back(punc[i]);
		mas.push_back(sr);
	}
	{
		for (size_t i = 0; i < s.size(); ++i) {//подчищение в строке двойных операций
			if (find(s[i], punc)) {
				string st;
				st.push_back(s[i]); st.push_back(s[i + 1]);
				if (find(st, mas)) {
					s[i] = ' '; s[i + 1] = ' ';
					Lexeme L(OPER, st, i, findpos(i, lines));
				     BOL.Add(L);
				}
			}
		}
	}
	for (size_t i = 0; i < s.size(); ++i) { //с ними какая-то муть происхает, по сему обрабатываются отдельно
		if (s[i] == '&') {
			Lexeme L(OPER, s[i], i, findpos(i, lines));
			  BOL.Add(L);
			s[i] = ' ';
		}
	}
	//////////////////////////////////////////////
	string musor; ind = 0;
	for (size_t i = 0; i<s.size(); ++i) { //ушлепывает мусор и скобочки
		if (s[i] == '[' || s[i] == ']') {
			Lexeme L(PUNCT, s[i], i, findpos(i, lines));
			BOL.Add(L);
		}
		else if (!find(s[i], str)) {
			musor.push_back(s[i]);
			if (musor.size() == 1)ind = i;
		}
	}
	Lexeme L(DIFF, musor, ind, findpos(ind, lines));
	BOL.Add(L);
	/////////////////////////////////////////////
	regex_s(s, punct, PUNCT, str, lines);// а здесь идет распихивание лексем по категориям
	regex_s(s, oper, OPER, str, lines);
	regex_s(s, name, IDENT, serv, lines);
	for (size_t i = 0; i < s.size(); ++i) {
		if ((s[i] >= '0' && s[i] <= '9') && ((s[i - 1] >= 'a' && s[i - 1] <= 'z' || s[i - 1] >= 'A' && s[i - 1] <= 'Z') || (s[i + 1] >= 'a' && s[i + 1] <= 'z' || s[i + 1] >= 'A' && s[i + 1] <= 'Z'))) {
			s[i] = 'a';
		}
	}
	regex_s(s, digit, CONST, str, lines);
	//////////////////////////////////////////////
	BOL.print();
	return 0;
} 