#include "includes.h"

enum typeLex {SERV, IDENT, OPER, PUNCT, CONST, DIFF, END}; //служебные, идентификаторы(имена), операции,пунктуация, константы, иное
enum typeIdent{INT, DOUBLE, CHAR, BOOL};
const int MAX_LEX = 9999;//а если текст входной программы будет очень большим? Хотя нам сейчас это не слишком важно.

////////////////////////Struct of data///////////////////////////////////
struct Lexeme{ 
    Lexeme(typeLex tl, string str, int _coord,int _line){
        t=tl;
        s=str;
		coord = _coord;
		line = _line;
    }
	Lexeme(typeLex tl, char str, int _coord, int _line) {
		t = tl;
		s.push_back(str);
		coord = _coord;
		line = _line;
	}
	void rev() {
		strbool = !strbool;
	}
    Lexeme(){
        t=DIFF;
		s = "none";
    }
    string s;   //содержимое
    typeLex t;    //тип, описание в inludes.h
	int coord; //позиция в строке
	int line=1;//на какой строке оно находится
	bool strbool = false;
	bool operator <(Lexeme q)const {
		if (coord < q.coord)return true;
		else return false;
	}
};

struct Ident {
	typeIdent t;
	string name;

	double d;
	int i;
	char c;
	bool b;

	Ident() {};
	Ident(string s, double d): name(s), d(d), t(DOUBLE) {};
	Ident(string s, int d) : name(s), i(d), t(INT) {};
	Ident(string s, char d) : name(s), c(d), t(CHAR) {};
	Ident(string s, bool d) : name(s), b(d), t(BOOL) {};
	Ident(double d) : name("CONST"), d(d), t(DOUBLE) {};
	Ident(int d) : name("CONST"), i(d), t(INT) {};
	Ident(char d) : name("CONST"), c(d), t(CHAR) {};
	Ident(bool d) : name("CONST"), b(d), t(BOOL) {};
	Ident(string s) : name(s) {};
};

struct Var {
	typeIdent t;

	double d;
	int i;
	char c;
	bool b;
};

struct elemOfPoliz {
	enum typeElemOfPoliz { IDENT, CONST, OPER, TRANS };
	typeElemOfPoliz t;
	Ident i;
	string oper;

	elemOfPoliz(string o)//dangerous!!!!!! 
	{
		t = OPER;
		oper = o;
	}

	elemOfPoliz(Ident id) : i(id) {};
	elemOfPoliz(string s, double d) : i(s, d) {};
	elemOfPoliz(string s, int d) : i(s, d) {};
	elemOfPoliz(string s, char d) : i(s, d) {};
	elemOfPoliz(string s, bool d) : i(s, d) {};
	elemOfPoliz(double d) : i(d) {};
	elemOfPoliz(int d) : i(d) {};
	elemOfPoliz(char d) : i(d) {};
	elemOfPoliz(bool d) : i(d) {};
};

struct BoxOfLexeme{

	set<Lexeme>masOfLex;
	vector<Lexeme>finalmasoflex;
    long long i=0;

	void Add(Lexeme L) {
		masOfLex.insert(L);
	}
	void print() {
		ofstream out("Output.txt");
		for (Lexeme x: masOfLex) {
			out.write((char*)&x, sizeof(Lexeme));
			finalmasoflex.push_back(x);
		//	cout << x.s << ' ' << x.t << endl;
		}
		out.close();
	}
};

//Error() - создание неизвестной ошибки,
//Error("division on NULL") - создание ошибки "division on NULL"
struct Error{
    string str;
	int line = 1;
    Error(){
        str="Unknown error";
    }

    Error(string s){
        str=s+" unknown line...";
    }
	Error(string s,int _line) {
		str = s; line = _line;
	}
};

struct Stack {//stack of types, operators and operands
	vector<Lexeme> st;
	void push(Lexeme& l) {
		st.push_back(l);
	}
	Lexeme pop() {
		Lexeme t = st[st.size() - 1];
		st.pop_back();
		return t;
	}
	bool is_empty() {
		return st.size() == 0;
	}
	Lexeme top() {
		return st[st.size() - 1];
	}
};

/////////////////////////Solution struct//////////////////////////////////
struct LecsAnalyzer{
    BoxOfLexeme BOL; //массив с лексемами. Создаешь объект Lexeme и вызываешь BOL.Add(объект лексемы)
                     //чтобы создать лексему: Lexeme L(тип_лексемы, содержимое)
    int Analyze();//Вызов самого лексического анализатора.
	////////
	int findpos(int coord, vector<pair<int, int> >& lines);
	bool find(char c, string& s);
	bool find(string c, vector<string>& s);
	void regex_s(string s, regex& rgx, typeLex type, string& str, vector<pair<int, int> >& lines);
	void regex_s(string ss, regex& rgx, typeLex type, set<string>& serv, vector<pair<int, int> >& lines);
};

struct semantic_analyzer {
	bool find(map<Lexeme, bool>& a, Lexeme& ident);
	void push_name_in_set(Lexeme name);
	void find_name(Lexeme name);
	void push_name(Lexeme& ident);

	stack<set<string> > STACK_FOR_SET_OF_IDENT;
	map <Ident, bool> TID_FOR_IDENT;
	map <Lexeme, bool> TID_FOR_USERS_TYPES;
};

struct Poliz {
	map<string, Var>var;
	vector<elemOfPoliz>pol;
	
	void push(elemOfPoliz);
	void push(string s, double d);
	void push(string s, int d);
	void push(string s, char d);
	void push(string s, bool d);
	void push(double d);
	void push(int d);
	void push(char d);
	void push(bool d);
	void push(string d);
	void Write();
	void Condition();
	void Cinout(char state);
	void Assignment();
};


struct Precalculator {
	string expr;

	ld calculate(string);
	ld calculate();
	void calculation(Lexeme& res, Lexeme& op1, Lexeme& oper, Lexeme& op2);
	void checkop(Lexeme& op1, Lexeme& oper, Lexeme& op2);
	void checknot(Lexeme& op, Lexeme& sigh);
};

struct syntax_analyzer {
	semantic_analyzer SemA;
	Poliz pol;
	Precalculator precalc;

	ifstream ifs;
	vector<Lexeme> BOL_S;
	syntax_analyzer(string s, vector<Lexeme>& a) {
		ifs.open(s);
		BOL_S = a;
		Lexeme End;
		End.t = END;
		BOL_S.push_back(End);
	}

	~syntax_analyzer() {
		ifs.close();
	}

	void getc(Lexeme& L);
	void watch(Lexeme& t,int shift);
	void program();
	void name();
	void Operator();
	void expression_statement();
	void expression();
	void expressionForWhile();
	void expression_1ForWhile();
	void  expression_1();
	void ratio_operation();
	void simple_expression();
	void terminal();
	void atom_1();
	void atom();
	void special_atom();
	void addition_operation();
	void multiplication_operation();
	void exponentiation(); //возведение в степень
	void assigment_operation();
	void increment();
	void sign();
	void constant();
	void bool_value();
	void input_output_operator();
	void list_of_elements();
	void element();
	void composite_operator();
	void description();
	void section();
	void type();
	void special_operator();
	void dowhile_operator();
	void for_operator();
	void cfor_operator();
	void pfor_operator();
	void way();
	void else_branch();

};


