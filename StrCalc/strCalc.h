#include <fstream>
#include <iomanip>
#include <exception>
#include <string>
#include <iostream>
#include <algorithm>
#include <map>
#include <iostream>
#include <cstdio>

using namespace std;
ifstream test("test.txt");
ifstream infs("InForCalc.txt");

/*Для всех возможных видов ошибок
По умолчанию: неизвестная ошибка*/
class error : exception {
private: string errorMessage = "Unknown error!";
public:
	error(string s) {
		errorMessage = s;
	}
	error() {

	}
	string WTF() {
		return errorMessage;
	}
};


class expression {
private:
	map <string, double> CONSTANTS
	{
		{ "pi", 3.14159265359 },
		{ "e", 2.71828182846 }
	};

	map <char, int> priority =
	{
		/*Заменено: унарный минус на #,
		log на &,
		sin на %,
		&& на $,
		|| на @*/

		{ ')', 0 },

		{ '+', 1 },
		{ '-', 1 },
		{ '@', 2 },

		{ '*', 2 },
		{ '/', 2 },
		{ '$', 2 },

		{ '^', 3 }, //правая ассоциотивность

		{ '#', 4 },

		{ '&', 5 },
		{ '%', 5 },
	};


	enum ElemType { INT, DOUBLE, STRING, CHAR };//типы элементов

	struct Value //то же самое, что с флагами, только яснее. В elemType тип элемента.
	{
		ElemType elemtype;

		int i;
		double d;
		string str;
		char ch;
	};

	/* Элемент стека, очереди и прочей фигни*/
	struct node {
		node() : next(NULL), elem() {}
		Value elem;
		node* next;
	};

	void Clear(node*& hv) {
		if (hv != NULL) {
			Clear(hv->next);
			delete hv;
			hv = NULL;
		}
	}

	struct queue {
		node* begin = NULL;
		node* end = NULL;
	};

	queue* queueForTranslator = NULL;
	queue* queueForCalculator = NULL;
	node* stack = NULL;
	node* stackForBrackets = NULL;

	/*Для стека, отслеживающего скобки*/


	/*---------------------------------Работа со стеком--------------------------------------------------------------*/
	void pushToStack(node*& hv, int arg) {
		node* t = new node;
		t->next = hv;
		hv = t;
		hv->elem.elemtype = INT;
		hv->elem.i = arg;
	}

	void pushToStack(node*& hv, double arg) {
		node* t = new node;
		t->next = hv;
		hv = t;
		hv->elem.elemtype = DOUBLE;
		hv->elem.d = arg;
	}

	void pushToStack(node*& hv, char arg) {
		if (hv != NULL) {
			node *t = new node;
			t->next = hv;
			hv = t;
			hv->elem.elemtype = CHAR;
			hv->elem.ch = arg;
		} else {
			hv = new node;
			hv->elem.elemtype = CHAR;
			hv->elem.ch = arg;
		}
	}

	void pushToStack(node*& hv, string arg) {
		node* t = new node;
		t->next = hv;
		hv = t;
		hv->elem.elemtype = STRING;
		hv->elem.str = arg;
	}

	Value pop(node*& hv) {
		if (isEmpty(hv)) throw error("Error - stack is empty!");

		Value x;

		if (hv->next != NULL) {
			node* t = hv;
			hv = hv->next;

			x = t->elem;
			delete(t);
		}

		else {
			x = hv->elem;
			delete hv;
			hv = NULL;
		}

		return x;
	}

	bool isEmpty(node*& hv) {
		if (hv == NULL) return true;
		else return false;
	}

	char erase(node*& hv, node*& delet)//Только для операций
	{
		node* t = hv;

		while (t->next != delet) {
			t = t->next;
		}

		node* d = t->next;
		t->next = t->next->next;

		char ch = d->elem.ch;
		delete d;

		return ch;
	}

	void ClearStack(node** hv) {
		if ((*hv) != NULL)
			Clear((*hv)->next);

		delete (*hv);
		*hv = NULL;
	}

	void spitOutRight(node*& hv, char oper) {
		for (auto it = priority.begin(); it != priority.end(); ++it)//что за операция?
		{
			if (it->first == oper) {
				int prior = it->second;

				//node* t = hv;
				bool end = false;

				while (!end)//проход по стеку и проверка нужно ли убрать элемент
				{
					if (hv->elem.ch == '(') {
						end = true;
						break;
					}

					for (auto jt = priority.begin(); jt != priority.end(); ++jt)//что за опер?
					{
						if (hv->elem.ch == jt->first) //какой операции равен этот элем стека и больше ли приоритет у него
						{
							if (jt->second > prior) pushToQueue(queueForCalculator, pop(hv));
							else {
								end = true;
								break;
							}
						}
					}

					//t = t->next;
				}

				break;
			}
		}
	}

	void spitOutLeft(node*& hv, char oper) {
		for (auto it = priority.begin(); it != priority.end(); ++it)//что за операция?
		{
			if (it->first == oper) {
				int prior = it->second;

				//node* t = hv;
				bool end = false;

				while (!end)//проход по стеку и проверка нужно ли убрать элемент
				{
					if (hv->elem.ch == '(') {
						end = true;
						break;
					}

					for (auto jt = priority.begin(); jt != priority.end(); ++jt)//что за опер?
					{
						if (hv->elem.ch == jt->first) //какой операции равен этот элем стека и больше ли приоритет у него
						{
							if (jt->second >= prior) pushToQueue(queueForCalculator, pop(hv));
							else {
								end = true;
								break;
							}
						}
					}

					//t = t->next;
				}

				break;
			}
		}
	}

	void cleanBrackets(node*& hv) {
		node* t = hv;
		while (t->elem.ch != '(') {
			pushToQueue(queueForCalculator, erase(hv, t));
			t = t->next;
		}

		pop(hv);
		pop(hv);
	}

	/*--------------------------------Работа с очередью--------------------------------------------------------------*/
	void pushToQueue(queue*& hv, int arg) {
		if (hv != NULL) {
			hv->end->next = new node;
			hv->end = hv->end->next;
			hv->end->elem.elemtype = INT;
			hv->end->elem.i = arg;
		}

		else {
			hv = new queue;
			hv->begin = new node;
			hv->end = new node;
			hv->end = hv->begin;
			hv->begin->elem.elemtype = INT;
			hv->begin->elem.i = arg;

			hv->end->elem.elemtype = INT;
			hv->end->elem.i = arg;
		}
	}

	void pushToQueue(queue*& hv, double arg) {
		if (hv != NULL) {
			hv->end->next = new node;
			hv->end = hv->end->next;
			hv->end->elem.elemtype = DOUBLE;
			hv->end->elem.d = arg;
		}

		else {
			hv = new queue;
			hv->begin = new node;
			hv->end = new node;
			hv->end = hv->begin;
			hv->begin->elem.elemtype = DOUBLE;
			hv->begin->elem.d = arg;

			hv->end->elem.elemtype = DOUBLE;
			hv->end->elem.d = arg;
		}
	}

	void pushToQueue(queue*& hv, char arg) {
		if (hv != NULL) {
			hv->end->next = new node;
			hv->end = hv->end->next;
			hv->end->elem.elemtype = CHAR;
			hv->end->elem.ch = arg;
		}

		else {
			hv = new queue;
			hv->begin = new node;
			hv->end = new node;
			hv->end = hv->begin;
			hv->begin->elem.elemtype = CHAR;
			hv->begin->elem.ch = arg;

			hv->end->elem.elemtype = CHAR;
			hv->end->elem.ch = arg;
		}
	}

	void pushToQueue(queue*& hv, string arg) {
		if (hv != NULL) {
			hv->end->next = new node;
			hv->end = hv->end->next;
			hv->end->elem.elemtype = STRING;
			hv->end->elem.str = arg;
		}

		else {
			hv = new queue;
			hv->begin = new node;
			hv->end = new node;
			hv->end = hv->begin;
			hv->begin->elem.elemtype = STRING;
			hv->begin->elem.str = arg;

			hv->end->elem.elemtype = STRING;
			hv->end->elem.str = arg;
		}
	}

	void  pushToQueue(queue*& hv, Value arg) {
		if (hv != NULL) {
			hv->end->next = new node;
			hv->end = hv->end->next;
			hv->end->elem = arg;
		}

		else {
			hv = new queue;
			hv->begin = new node;
			hv->end = new node;
			hv->end = hv->begin;
			hv->begin->elem = arg;

			hv->end->elem = arg;
		}
	}

	void printQueue(node* beg, node* end) {
		if (beg != end) {
			switch (beg->elem.elemtype) {
				case INT:
				{
					cout << beg->elem.i;
					break;
				}

				case DOUBLE:
				{
					cout << beg->elem.d;
					break;
				}

				case STRING:
				{
					cout << beg->elem.str;
					break;
				}

				case CHAR:
				{
					cout << beg->elem.ch;
					break;
				}
			}
			return printQueue(beg->next, end);
		}

		else {

			switch (beg->elem.elemtype) {
				case INT:
				{
					cout << beg->elem.i;
					break;
				}

				case DOUBLE:
				{
					cout << beg->elem.d;
					break;
				}

				case STRING:
				{
					cout << beg->elem.str;
					break;
				}

				case CHAR:
				{
					cout << beg->elem.ch;
					break;
				}
			}
		}

		cout << endl;
	}

	bool isEmpty(queue*& hv) {
		if (hv == NULL) return true;
		if (hv->begin == NULL) return true;
		else return false;
	}

	Value pop(queue*& hv) {
		if (isEmpty(hv)) throw error("Error - queue is epmty!");

		Value x;
		if (hv->begin->next != NULL) {
			node* t = hv->begin;
			hv->begin = hv->begin->next;

			x = t->elem;
			delete(t);
		}

		else {
			x = hv->begin->elem;

			delete hv;
			hv = NULL;
		}

		return x;
	}

	void ClearQueue(queue** hv) {
		if ((*hv) != NULL) {
			Clear((*hv)->begin->next);

			delete (*hv)->begin;
			delete (*hv)->end;
		}

		delete (*hv);
		*hv = NULL;
	}

	/*--------------------------------Мат блок--------------------------------------------------------------*/

	double Sqrt(double d) {
		if (d < 0) {
			throw error("The square root of a negative number!");
		}

		return sqrt(d);
	}

	double Log(node*& st) {
		Value val = pop(st);
		double base, x;

		if (val.elemtype == DOUBLE)
			x = val.d;
		else x = val.i;

		val = pop(st);

		if (val.elemtype == DOUBLE) base = val.d;
		else base = val.i;

		if (x <= 0) throw error("Error - argument of logarithm <= 0!");
		if (base == 1) throw error("Error - logarithm base = 1!");
		if (base <= 0) throw error("Error - logarithm base <= 0!");
		if (log(base) == 0) throw error("Error - division on 0 (in logarithm)!");

		return log(x) / log(base);
	}

	double Sin(node*& st) {
		Value val = pop(st);
		double n1;

		if (val.elemtype == DOUBLE)
			n1 = val.d;
		else n1 = val.i;

		return sin(n1);
	}

	double BinPow(double num, int deg) {
		int res = num, d = 1;

		while (d != abs(deg)) {
			if (d * 2 <= abs(deg)) {
				d *= 2;
				res *= res;
			} else {
				d++;
				res *= num;
			}
		}

		if (deg < 0 && num > 0) return 1 / res;
		if (deg < 0 && deg % 2 == 1 && num < 0) return -1 / res;
		if (deg >= 0 && deg % 2 == 1 && num < 0) return -res;

		return res;
	}

	double LgPow(double num, double deg) {
		return exp(deg * log(num));
	}

	double Involution(node*& st) {
		Value val = pop(st);
		bool bin;

		double deg, num;

		if (val.elemtype == DOUBLE) {
			bin = false;
			deg = val.d;
		}

		else {
			bin = true;
			deg = val.i;
		}

		val = pop(st);

		if (val.elemtype == DOUBLE) num = val.d;
		else num = val.i;

		if (bin) return BinPow(num, deg);
		else return LgPow(num, deg);
	}

	double Subtraction(node*& st) {
		Value val = pop(st);
		double n1, n2;

		if (val.elemtype == DOUBLE)
			n1 = val.d;
		else n1 = val.i;

		val = pop(st);

		if (val.elemtype == DOUBLE) n2 = val.d;
		else n2 = val.i;

		return n2 - n1;
	}

	double Addition(node*& st) {
		Value val = pop(st);
		double n1, n2;

		if (val.elemtype == DOUBLE)
			n1 = val.d;
		else n1 = val.i;

		val = pop(st);

		if (val.elemtype == DOUBLE) n2 = val.d;
		else n2 = val.i;

		return n2 + n1;
	}

	double Multiplication(node*& st) {
		Value val = pop(st);
		double n1, n2;

		if (val.elemtype == DOUBLE)
			n1 = val.d;
		else n1 = val.i;

		val = pop(st);

		if (val.elemtype == DOUBLE) n2 = val.d;
		else n2 = val.i;

		return n2 * n1;
	}

	double Division(node*& st) {
		Value val = pop(st);
		double n1, n2;

		if (val.elemtype == DOUBLE)
			n1 = val.d;
		else n1 = val.i;

		val = pop(st);

		if (val.elemtype == DOUBLE) n2 = val.d;
		else n2 = val.i;

		if (n1 == 0) throw error("Error - division on 0!");
		return n2 / n1;
	}

	double LogicalAnd(node*& st) {
		Value val = pop(st);
		double n1, n2;

		if (val.elemtype == DOUBLE)
			n1 = val.d;
		else n1 = val.i;

		val = pop(st);

		if (val.elemtype == DOUBLE) n2 = val.d;
		else n2 = val.i;

		if (n1 == 0 || n2 == 0)
			return 0;
		else
			return 1;
	}

	double LogicalOr(node*& st) {
		Value val = pop(st);
		double n1, n2;

		if (val.elemtype == DOUBLE)
			n1 = val.d;
		else n1 = val.i;

		val = pop(st);

		if (val.elemtype == DOUBLE) n2 = val.d;
		else n2 = val.i;

		if (n1 == 0 && n2 == 0)
			return 0;
		else
			return 1;
	}

public:

	map <string, double> valueOfVariables;

	long double Calc();

	void Translate()//STRING вообще не используется//протести работу с синусом
	{
		/*cout << "IN: ";
		printQueue(queueForTranslator->begin, queueForTranslator->end);*/

		stack = NULL;

		while (!isEmpty(queueForTranslator)) {
			Value val = pop(queueForTranslator);


			switch (val.elemtype) {
				case INT:
				{
					pushToQueue(queueForCalculator, val.i);
					break;
				}

				case DOUBLE:
				{
					pushToQueue(queueForCalculator, val.d);
					break;
				}

				case STRING:
				{
					pushToQueue(queueForCalculator, val.str);
					break;
				}

				case CHAR:
				{
					/*Заменено: унарный минус на #,
					log на &,
					sin на %,*/

					switch (val.ch) {

						case '(':
						{
							pushToStack(stack, val.ch);
							break;
						}

						case '^':
						{
							pushToStack(stack, val.ch);
							spitOutRight(stack, val.ch);
							break;
						}

						case ')':
						{
							//pushToStack(stack, val.ch);
							spitOutRight(stack, val.ch);
							pop(stack);
							break;
						}



						default:
						{

							spitOutLeft(stack, val.ch);
							pushToStack(stack, val.ch);
							break;
						}
					}
				}
			}
		}
	}

	long double Calculate() {
		//cout << endl << "POST: ";
		//printQueue(queueForCalculator->begin, queueForCalculator->end);

		stack = NULL;
		long double answ;
		bool neg = false;

		if (valueOfVariables.size() != 0) AskVariables();

		while (!isEmpty(queueForCalculator)) {
			Value val = pop(queueForCalculator);

			if (neg) {
				if (val.elemtype == DOUBLE) val.d = -val.d;
				else val.i = -val.i;

				neg = false;
			}

			switch (val.elemtype) {
				case INT:
				{
					pushToStack(stack, val.i);
					break;
				}

				case DOUBLE:
				{
					pushToStack(stack, val.d);
					break;
				}

				case CHAR:
				{
					/*Заменено: унарный минус на #,
					log на &,
					sin на %,*/

					switch (val.ch) {

						case '^':
						{
							pushToStack(stack, Involution(stack));
							break;
						}

						case '-':
						{
							pushToStack(stack, Subtraction(stack));
							break;
						}

						case '+':
						{
							pushToStack(stack, Addition(stack));
							break;
						}

						case '*':
						{
							pushToStack(stack, Multiplication(stack));
							break;
						}

						case '$':
						{
							pushToStack(stack, LogicalAnd(stack));
							break;
						}

						case '@':
						{
							pushToStack(stack, LogicalOr(stack));
							break;
						}

						case '/':
						{
							pushToStack(stack, Division(stack));
							break;
						}

						case '#':
						{
							neg = true;
							break;
						}

						case '&':
						{
							pushToStack(stack, Log(stack));
							break;
						}


						case '%':
						{
							pushToStack(stack, Sin(stack));
							break;
						}
					}

					break;
				}

				case STRING:
				{
					bool cons = false;

					for (auto it = CONSTANTS.begin(); it != CONSTANTS.end(); ++it) {
						if (val.str == it->first) {
							pushToStack(stack, it->second);
							cons = true;
							break;
						}
					}

					if (!cons) {
						for (auto it = valueOfVariables.begin(); it != valueOfVariables.end(); ++it) {
							if (val.str == it->first) {
								pushToStack(stack, it->second);
								break;
							}
						}
					}

					break;
				}
			}

		}

		Value val = pop(stack);

		if (val.elemtype == DOUBLE) return val.d;
		else return val.i;
	}
	/*Считывание!!!---------------------------------------------------------------------------------------------------*/
	/*Процедура считывания
	Вход: два потока входной
	В ходе программы меняет stackForBrackets
	Результат в queueForCalculator
	В случае некорректного ввода кидает исключение
	ps: Заменяет унарный минус на #,
	log на &,
	sin на %,
	Удаляет унарный плюс*/
	void read(istream &in) {
		char c, state = 'A';
		int tmp;
		int i = 0;
		string tmpStr;
		pushToQueue(queueForTranslator, '(');
		pushToStack(stackForBrackets, '(');

		while ((infs.get(c))) {
			i++;
			bool flag = false;
			if ((c == '\n')) {
				c = ')';
				flag = true;
			}
			switch (state) {
				case 'A':
					if (c == '(') {
						pushToStack(stackForBrackets, c);
						pushToQueue(queueForTranslator, c);
						break;
					}
					if (c == ')') {
						try {
							pop(stackForBrackets);
							pushToQueue(queueForTranslator, c);
						} catch (error &e) {
							throw error(("Bad symbol: Sorry, you have problems with brackets on " + to_string(i)) + " position!");
							break;
						}
						break;
					}
					if ((c == '*') || (c == '/') || (c == '^')) {
						if (!((queueForTranslator->end->elem.ch != '(') && (queueForTranslator->end->elem.ch != '*') && (queueForTranslator->end->elem.ch != '/') && (queueForTranslator->end->elem.ch != '+') && (queueForTranslator->end->elem.ch != '-') && (queueForTranslator->end->elem.ch != '#') && (queueForTranslator->end->elem.ch != '&'))) {
							throw error(("Bad symbol: Sorry, you have problems with brackets on " + to_string(i)) + " position!");
						}
						pushToQueue(queueForTranslator, c);
						break;
					}
					if (c == '+') {
						state = 'B';
						break;
					}
					if (c == '-') {
						pushToQueue(queueForTranslator, '#');
						state = 'B';
						break;
					}
					if (c >= '0' && c <= '9') {
						tmp = c - '0';
						state = 'C';
						break;
					}
					if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
						tmpStr = c;
						state = 'D';
						break;
					}
					throw error((("Bad symbol: Sorry, you have bad symbol on ") + to_string(i)) + " position!");
					break;


				case 'B':
					if (c >= '0' && c <= '9') {
						tmp = c - '0';
						state = 'C';
						break;
					}
					if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
						tmpStr = c;
						state = 'D';
						break;
					}
					if (c == '(') {
						state = 'V';
						pushToStack(stackForBrackets, c);
						break;
					}
					throw error((("Bad symbol: Sorry, you have bad symbol on ") + to_string(i)) + " position!");
					break;


				case 'C':
					if ((c >= '0') && (c <= '9')) {
						tmp *= 10;
						tmp += c - '0';
						break;
					}
					if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') {
						pushToQueue(queueForTranslator, tmp);
						pushToQueue(queueForTranslator, c);
						state = 'A';
						break;
					}
					if (c == ')') {
						try {
							state = 'A';
							pop(stackForBrackets);
							pushToQueue(queueForTranslator, tmp);
							pushToQueue(queueForTranslator, c);
						} catch (error &e) {
							throw error(("Bad symbol: Sorry, you have problems with brackets on " + to_string(i)) + " position!");
							break;
						}
						break;
					}
					if (c == '.') {
						state = 'E';
						tmpStr = to_string(tmp);
						tmpStr += '.';
						break;
					}
					throw error((("Bad symbol: Sorry, you have bad symbol on ") + to_string(i)) + " position!");
					break;
				case 'D':
					if (c == ')') {
						try {
							state = 'A';
							pop(stackForBrackets);
							pushToQueue(queueForTranslator, tmpStr);
							if (CONSTANTS.find(tmpStr) == CONSTANTS.end())
								valueOfVariables.insert(make_pair(tmpStr, 0));
							pushToQueue(queueForTranslator, c);
						} catch (error &e) {
							throw error(("Bad symbol: Sorry, you have problems with brackets on " + to_string(i)) + " position!");
							break;
						}
						break;
					}
					if (c == '(' && tmpStr == "log") {
						state = 'J';
						//pushToQueue(queueForTranslator, c);
						break;
					}
					if ((c == '(') && (tmpStr == "sin")) {
						state = 'A';
						pushToQueue(queueForTranslator, '%');
						pushToStack(stackForBrackets, c);
						pushToQueue(queueForTranslator, c);
						break;
					}
					if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') {
						pushToQueue(queueForTranslator, tmpStr);
						if (CONSTANTS.find(tmpStr) == CONSTANTS.end())
							valueOfVariables.insert(make_pair(tmpStr, 0));
						pushToQueue(queueForTranslator, c);
						state = 'A';
						break;
					}
					if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
						tmpStr += c;
						break;
					}
					throw error((("Bad symbol: Sorry, you have bad symbol on ") + to_string(i)) + " position!");
					break;
				case 'E':
					if (c >= '0' && c <= '9') {
						tmpStr += c;
						state = 'F';
						break;
					}
					throw error((("Bad symbol: Sorry, you have bad symbol on ") + to_string(i)) + " position!");
					break;
				case 'F':
					if (c >= '0' && c <= '9') {
						tmpStr += c;
						break;
					}
					if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') {
						pushToQueue(queueForTranslator, stod(tmpStr));
						pushToQueue(queueForTranslator, c);
						state = 'A';
						break;
					}
					if (c == 'e' || c == 'E') {
						tmpStr += toupper(c);
						state = 'G';
						break;
					}
					if (c == ')') {
						try {
							state = 'A';
							pop(stackForBrackets);
							pushToQueue(queueForTranslator, stod(tmpStr));
							pushToQueue(queueForTranslator, c);
						} catch (error & e) {
							throw error(("Bad symbol: Sorry, you have problems with brackets on " + to_string(i)) + " position!");
							break;
						}
						break;
					}
					throw error((("Bad symbol: Sorry, you have bad symbol on ") + to_string(i)) + " position!");
					break;
				case 'G':
					if (c == '+' || c == '-') {
						tmpStr += c;
						state = 'H';
						break;
					}
					throw error((("Bad symbol: Sorry, you have bad symbol on ") + to_string(i)) + " position!");
					break;
				case 'H':
					if (c >= '0' && c <= '9') {
						tmpStr += c;
						state = 'I';
						break;
					}
					throw error((("Bad symbol: Sorry, you have bad symbol on ") + to_string(i)) + " position!");
					break;
				case 'I':
					if (c >= '0' && c <= '9') {
						tmpStr += c;
						break;
					}
					if (c == ')') {
						try {
							state = 'A';
							pop(stackForBrackets);
							pushToQueue(queueForTranslator, c);
						} catch (error& e) {
							throw error(("Bad symbol: Sorry, you have problems with brackets on " + to_string(i)) + " position!");
							break;
						}
						break;
					}
					if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') {
						pushToQueue(queueForTranslator, stod(tmpStr));
						pushToQueue(queueForTranslator, c);
						state = 'A';
						break;
					}
					throw error((("Bad symbol: Sorry, you have bad symbol on ") + to_string(i)) + " position!");
					break;
				case 'J':
					if (c >= '0' && c <= '9') {
						tmp = c - '0';
						state = 'K';
						break;
					}
					if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
						tmpStr = c;
						state = 'S';
						break;
					}
					throw error((("Bad symbol: Sorry, you have bad symbol on ") + to_string(i)) + " position!");
					break;
				case'K':
					if (c >= '0' && c <= '9') {
						tmp *= 10;
						tmp += c - '0';
						break;
					}
					if (c == '.') {
						state = 'L';
						tmpStr = to_string(tmp);
						tmpStr += c;
						break;
					}
					if (c == ',') {
						pushToQueue(queueForTranslator, tmp);
						pushToQueue(queueForTranslator, '&');
						pushToStack(stackForBrackets, '(');
						pushToQueue(queueForTranslator, '(');
						state = 'A';
						break;
					}
					throw error((("Bad symbol: Sorry, you have bad symbol on ") + to_string(i)) + " position!");
					break;
				case 'L':
					if (c >= '0' && c <= '9') {
						tmpStr += c;
						state = 'M';
						break;
					}
					throw error((("Bad symbol: Sorry, you have bad symbol on ") + to_string(i)) + " position!");
					break;
				case 'M':
					if (c >= '0' && c <= '9') {
						tmpStr += c;
						break;
					}
					if (c == ',') {
						pushToQueue(queueForTranslator, stod(tmpStr));
						//valueOfVariables[tmpStr] = 0;
						pushToStack(stackForBrackets, '(');
						pushToQueue(queueForTranslator, '(');
						state = 'A';
						break;
					}
					if (c == 'e' || c == 'E') {
						tmpStr += toupper(c);
						state = 'N';
						break;
					}
					throw error((("Bad symbol: Sorry, you have bad symbol on ") + to_string(i)) + " position!");
					break;
				case 'N':
					if (c == '+' || c == '-') {
						tmpStr += c;
						state = 'O';
						break;
					}
					throw error((("Bad symbol: Sorry, you have bad symbol on ") + to_string(i)) + " position!");
					break;
				case 'O':
					if (c >= '0' && c <= '9') {
						tmpStr += c;
						state = 'Q';
						break;
					}
					throw error((("Bad symbol: Sorry, you have bad symbol on ") + to_string(i)) + " position!");
					break;
				case 'Q':
					if (c >= '0' && c <= '9') {
						tmpStr += c;
						break;
					}
					if (c == ',') {
						pushToQueue(queueForTranslator, stod(tmpStr));
						pushToQueue(queueForTranslator, '(');
						pushToStack(stackForBrackets, '(');
						state = 'A';
						break;
					}
					throw error((("Bad symbol: Sorry, you have bad symbol on ") + to_string(i)) + " position!");
					break;
				case 'S':
					if ((c >= 'a' && c <= 'z') || (c >= 'A' || c <= 'Z') || (c >= '0' && c <= '9')) {
						tmpStr += c;
						break;
					}
					if (c == ',') {
						pushToQueue(queueForTranslator, tmpStr);
						if (CONSTANTS.find(tmpStr) == CONSTANTS.end())
							valueOfVariables[tmpStr] = 0;
						pushToQueue(queueForTranslator, '(');
						pushToStack(stackForBrackets, '(');
						state = 'A';
						break;
					}
					throw error((("Bad symbol: Sorry, you have bad symbol on ") + to_string(i)) + " position!");
					break;
				case 'V':
					if (c == '+') {
						state = 'B';
						break;
					}
					if (c == '-') {
						pushToQueue(queueForTranslator, '#');
						state = 'B';
						break;
					}
					if (c >= '1' && c <= '9') {
						state = 'C';
						tmp = c - '0';
						break;
					}
					if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
						tmpStr = c;
						state = 'D';
						break;
					}
					throw error((("Bad symbol: Sorry, you have bad symbol on ") + to_string(i)) + " position!");
					break;
			}
			if (flag) {
				break;
			}
		}
		if (!isEmpty(stackForBrackets)) {
			throw error("Bad symbol: Sorry, you have problems with brackets in the end\n");
		}
		//pushToQueue(queueForTranslator, ')');
	}
	/*Конец считывания------------------------------------------------------------------------------------------------*/

	/*Опрос перемнных
	Изменяет valueOfVariables
	Взаимодействует с пользователем!!! через sdtio*/

	void AskVariables() {
		string tmp = "yes";
		while (true) {
			if (tmp == "yes") {
				for (auto &i : valueOfVariables) {
					cout << i.first << " = ";
					infs >> i.second;
				}
			} else {
				cout << "Enter <name> <value>, then press enter button. To stop the process enter \"\\stop\"\n";
				string tmp2;
				int tmpV;
				infs >> tmp2;
				while (tmp2 != "\\stop") {
					infs >> tmpV;
					auto i = valueOfVariables.find(tmp2);
					if (i == valueOfVariables.end()) {
						cout << "\nVariable does not exist. Do you want save it? [yes/no] ";
						infs >> tmp;
						if (tmp == "no") {
							infs >> tmp2;
							continue;
						}
						valueOfVariables.insert(make_pair(tmp2, tmpV));
					} else {
						i->second = tmpV;
					}
					infs >> tmp2;
				}
			}
			cout << "\nEvery thing is ok?\n";
			for (auto &i : valueOfVariables) {
				cout << i.first << " = " << i.second << "\n";
			}
			cout << "\nDo you want change smth? [yes/no] ";
			infs >> tmp;
			if (tmp == "no") {
				return;
			}
			cout << "\nDo you want change all? [yes/no] ";
			infs >> tmp;
		}
	}

	void AddConstant(string s, double v) {
		CONSTANTS.insert(make_pair(s, v));
	}

	void PrintConstant(ostream & out) {
		for (auto &i : CONSTANTS) {
			out << i.first << " " << i.second << '\n';
		}
	}

	void Clear() {
		ClearStack(&stackForBrackets);
		ClearStack(&stack);
		ClearQueue(&queueForCalculator);
		ClearQueue(&queueForTranslator);
	}
};

ld Calc() {
	infs.close();
	infs.open("InForCalc.txt");

	string tmp;
	expression expr;
	while (true) {
		infs >> tmp;

		if (tmp == "END")
			return -1;

		if (tmp == "\\calc") {
			infs >> tmp;
			expr.Clear();
			if (tmp == "-p") {
				cout << '=' << expr.Calculate() << '\n';
			}
			if (tmp == "-f") {
				infs >> tmp;
				fstream in;
				try {
					in.open(tmp, fstream::in);
				} catch (error &e) {
					cout << "File does not exist!\n";
					continue;
				}
				try {
					expr.read(in);
					expr.Translate();
					cout << '=' << expr.Calculate() << '\n';
				} catch (error &e) {
					cout << e.WTF() << '\n';
					getline(infs, tmp);
				}
				continue;
			} else if (tmp == "-c") {
				try {
					infs.get();
					expr.read(infs);
					expr.Translate();
					return expr.Calculate();
				} catch (error &e) {
					cout << e.WTF() << '\n';
					if (e.WTF() != "Bad symbol: Sorry, you have problems with brackets in the end\n") {
						getline(infs, tmp);
					}

				}
				continue;
			} else {
				cout << "Illegal options " << tmp << "\n Usage:\n	\\calc -f [path] -- Read from file\n	\\calc -c -- Read from console\n";
			}
		}
		if (tmp == "\\help") {
			cout << "NAME\n    \\help -- Writes help information\n\n\nNAME\n    \\calc -- Calculate value of expression\nSYNOPSIS\n    \\calc -f [path] -- Read from file\n    \\calc -p -- Calculate previous again\n    \\calc -c -- Read from console\n\n\nNAME\n    \\EnV -- Ask value of variables\n\nNAME\n    \\exit -- Exit from programm\n\nNAME\n    \\delete -- delete variable\n    \\delete all -- delete all\n\nNAME\n    \\addconst [name] [value] -- add new constant\n\nNAME\n    \\prconst -- print all contants\n";
			continue;
		}
		if (tmp == "\\EnV") {
			expr.AskVariables();
			continue;
		}
		if (tmp == "\\delete") {
			infs >> tmp;
			if (tmp == "all") {
				expr.valueOfVariables.clear();
			} else {
				if (expr.valueOfVariables.find(tmp) == expr.valueOfVariables.end()) {
					cout << "Variable does not exist.\n";
				} else {
					expr.valueOfVariables.erase(tmp);
				}
			}
			continue;
		}
		if (tmp == "\\addconst") {
			infs >> tmp;
			double tmp2;
			infs >> tmp2;
			expr.AddConstant(tmp, tmp2);
			continue;
		}
		if (tmp == "\\prconst") {
			//expr.PrintConstant(cout);
			continue;
		}
		if (tmp == "\\exit") {
			break;
		}
		cout << tmp << ": Command not found!\n";
	}

	return 0;
}


