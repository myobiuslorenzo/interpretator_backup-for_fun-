#include "interface.h"
#include "includes.h"
Lexeme lex;
Lexeme buff;
typeIdent TYPE;
size_t index = 0;
string let = ("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
string buff_type;
bool isLet(char c) { //опасная штука, проверь
	return let.find(c) != string::npos;
}

/*void syntax_analyzer::getc(Lexeme& L) {
if (ifs.eof())
L = Lexeme();
else
ifs.read((char*)&L, sizeof(Lexeme));
}*/
/*Lexeme syntax_analyzer::watch(int shift) {
ifstream::streampos pos = ifs.tellg();
ifs.seekg(sizeof(Lexeme)*(shift-1), ios::cur);

Lexeme t;
ifs.read((char*)&t, sizeof(Lexeme));
cout << t.s << ' ' << lex.s << endl;*/
//ifs.seekg(pos/*sizeof(Lexeme)*(-shift)*/, ios::beg);
/*cout << t.s << ' ' << lex.s << endl;
return t;
}*/

void syntax_analyzer::getc(Lexeme& L) {


	if (ifs.eof() || index >= BOL_S.size())
		L = Lexeme();
	else {
		L = BOL_S[index];

		if (index == 0 && L.s == "") {
			index++;
			L = BOL_S[index];
		}

	}

	if (exprIsNow)
		addToExpr();

	index++;
}

void syntax_analyzer::addToExpr() {
	precalc.expr.push_back(lex);
}

void syntax_analyzer::watch(Lexeme& t, int shift) {
	if (index + shift >= BOL_S.size())
		t = Lexeme();
	else t = BOL_S[index + shift - 1];
}
void syntax_analyzer::program() {
	SemA.STACK_FOR_SET_OF_IDENT.push(set<string>());

	getc(lex); //тут такая штука, что у меня в начале массива лексем "нулевая" лексема стоит. Поэтому я считываю. У тебя не так?

	if (lex.t == END)throw(Error("Empty program\n"));
	while (lex.s != "int") {
		if (lex.t == END)
			throw(Error("int expected", lex.line));

		Lexeme t;
		watch(t, 1);
		if (t.s != "main")
			description();
		else
			throw(Error("int  expected, but \"" + lex.s + "\" found", t.line));
	}

	if (ifs.eof())
		throw(Error("main expected", lex.line));

	getc(lex);

	if (lex.s != "main")
		throw(Error("main expected", lex.line));

	getc(lex);

	if (lex.s != "(")
		throw(Error("( expected", lex.line));

	getc(lex);

	if (lex.s != ")")
		throw(Error(") expected", lex.line));
	getc(lex);

	if (lex.s != "{")
		throw(Error("{ expected", 1));

	Operator();
}
void syntax_analyzer::name() { //вот тут я не уверен, что всё так, ибо не так как по грамматике 
	SemA.find_name(lex); //poliz working here

	if (!isLet(lex.s[0])) {
		string err("The name can't begin with '");
		err += lex.s[0] + "'!";
		throw(Error(err, lex.line));
	}

	//if (simpleExpr) addToExpr();
	exprIsNow = false; //FLAG //с комментом синаут работает

	if (descript)
		pol.push(lex.s, elemOfPoliz::typeElemOfPoliz::IDENT, TYPE);
	else
		if (!cinoutNow&&!assigment)
			addToExpr();

	if (assigment) pol.push(lex.s, elemOfPoliz::typeElemOfPoliz::IDENT);
	

	getc(lex);
}

void syntax_analyzer::nameIn() { //вот тут я не уверен, что всё так, ибо не так как по грамматике 
	SemA.find_name(lex); //poliz working here

	if (!isLet(lex.s[0])) {
		string err("The name can't begin with '");
		err += lex.s[0] + "'!";
		throw(Error(err, lex.line));
	}

	//if (simpleExpr) addToExpr();
	exprIsNow = false; //FLAG //с комментом синаут работает

	pol.push(lex.s, elemOfPoliz::typeElemOfPoliz::IDENT, TYPE);


	getc(lex);
}

void syntax_analyzer::nameForExpr() { //вот тут я не уверен, что всё так, ибо не так как по грамматике 
	SemA.find_name(lex); //poliz working here

	if (precalc.expr.empty()) addToExpr();
	if (!isLet(lex.s[0])) {
		string err("The name can't begin with '");
		err += lex.s[0] + "'!";
		throw(Error(err, lex.line));
	}

	getc(lex);
}



void syntax_analyzer::nameForWhile() { //вот тут я не уверен, что всё так, ибо не так как по грамматике 
	SemA.find_name(lex); //poliz working here

	if (!isLet(lex.s[0])) {
		string err("The name can't begin with '");
		err += lex.s[0] + "'!";
		throw(Error(err, lex.line));
	}

	if (!cinoutNow) exprIsNow = false; //FLAG //с комментом синаут работает

	if (descript)
		pol.push(lex.s, elemOfPoliz::typeElemOfPoliz::IDENT, TYPE);
	else
		if (!exprIsNow)
			addToExpr();

	getc(lex);
}

void syntax_analyzer::nameForSect() { //вот тут я не уверен, что всё так, ибо не так как по грамматике 
	SemA.find_name(lex); //poliz working here

	if (!isLet(lex.s[0])) {
		string err("The name can't begin with '");
		err += lex.s[0] + "'!";
		throw(Error(err, lex.line));
	}

	if (!cinoutNow) exprIsNow = false; //FLAG //с комментом синаут работает

	if (descript)
		pol.push(lex.s, elemOfPoliz::typeElemOfPoliz::IDENT, TYPE);
	else
		if (!exprIsNow)
			addToExpr();
}

void syntax_analyzer::Operator() {
	Lexeme t;

	if (lex.s == "cinout")
		input_output_operator();//poliz working
	else
		if (lex.s == "{") {
			set<string>s = SemA.STACK_FOR_SET_OF_IDENT.top();

			SemA.STACK_FOR_SET_OF_IDENT.push(s);
			composite_operator();
			SemA.STACK_FOR_SET_OF_IDENT.pop();

		} else
			if (lex.s == "do" || lex.s == "for" || lex.s == "while")
				special_operator();
			else
				if (lex.s == "int" || lex.s == "bool" || lex.s == "double" || lex.s == "char") {
					buff_type = lex.s;
					//descript = true;
					description();
					//descript = false;
				} else
					expression_statement();

				//getc(lex);

				//if (lex.t != END)
				//getc(lex);
}
void syntax_analyzer::expression_statement() {
	if (lex.s == "}") throw(Error("found \"}\", but expression expected.", lex.line));

	expression();
	if (lex.s != ";")throw(Error("found \"" + lex.s + "\", but ; expected...where is my ; ? :'(", lex.line));
	getc(lex);
}
void syntax_analyzer::expression() {
	Lexeme t;
	if (!descript)
		watch(t, 1);
	else {
		watch(t, 1);
		assigment = true;
	}

	if (t.s == "=" && lex.t == IDENT || descript) {
		assigment = true;
		exprIsNow = false;
		if(!descript) name();
		//Lexeme costylek = lex;
		getc(lex);
		//SEA.checkop(tmp, lex);
		descript = false;

		assigment = false;
		exprIsNow = true;
		expression();
		assigment = true;
		//elemOfPoliz e(costylek.s);
		//pol.push(e);

	} else {
		exprIsNow = true;
		//if(!cinoutNow) addToExpr();
		expression_1();
	}

	if (precalc.expr.empty() || operOfFor) pushExprInPol = false;
	else
		pushExprInPol = true;

	//pol.push(Ident((double)precalc.calculate())); //очистка сделана в прекалке

	if (!precalc.expr.empty() && precalc.expr.back().s == ";") precalc.expr.pop_back();

	if (pushExprInPol|| descript) {
		pol.push(precalc.expr);
		precalc.expr.clear();
	}

	exprIsNow = false;

	if (assigment) {
		pol.push(string("="));
		assigment = false;
	}

	if (ratioOper != "NONE") {
		pol.push(ratioOper);
		ratioOper="NONE";
	}

	//	getc(lex);
}

void syntax_analyzer::expressionForWhile() { //poliz working
	exprIsNow = true;
	Lexeme t;
	watch(t, 1);
	Lexeme tmp = lex;
	if (t.s == "=" && lex.t == IDENT) {
		name();
		getc(lex);
		Lexeme costylek = lex;
		getc(lex);
		//SEA.checkop(tmp, lex);
		expression();
		elemOfPoliz e(costylek.s);
		pol.push(e);
	} else {
		expression_1ForWhile();
	}

	if (pushExprInPol) {
		pol.push(precalc.expr);
		precalc.expr.clear();
	}

	if (ratioOper != "NONE") {
		pol.push(ratioOper);
		ratioOper = "NONE";
	}
}
void syntax_analyzer::expression_1() {

	int i = 1;
	Lexeme t = lex;
	while (t.t != PUNCT&&t.s != "!"&&t.s != "<"&&t.s != ">" &&t.s != "=="&&t.s != "<="&&t.s != ">=") {
		watch(t, i);
		i++;
	}

	//if (t.s=="+"||t.s=="-") {
	if (t.t == PUNCT)
		simple_expression();
	else {
		addToExpr();
		pol.push(precalc.expr);
		precalc.expr.clear();

		//	expression_1(); Это ,хоть и по грамматике, все-таки лишнее. С этим будет бесконечный цикл 
		exprIsNow = false;
		getc(lex);
		

		ratio_operation();
		//getc(lex);

		exprIsNow = true;

		expression_1();
	}
}

void syntax_analyzer::expression_1ForWhile() {

	int i = 1;
	Lexeme t = lex;
	while (t.t != PUNCT&&t.s != "!"&&t.s != "<"&&t.s != ">" &&t.s != "=="&&t.s != "<="&&t.s != ">=") {
		watch(t, i);
		i++;
	}

	//if (t.s=="+"||t.s=="-") {
	if (t.t == PUNCT) {
		simple_expression();
	}  else {
		//	expression_1(); Это ,хоть и по грамматике, все-таки лишнее. С этим будет бесконечный цикл
		//precalc.expr.push_back(lex); //КОСТЫЛЬ //теперь почему-то без него легче
		simple_expressionWhile();
		pol.push(precalc.expr);
		precalc.expr.clear();

		//getc(lex);
		ratio_operation();
		//getc(lex);

		simple_expressionWhile();
		if(!precalc.expr.empty()&&precalc.expr.back().s==")") precalc.expr.pop_back();
	}
}

void syntax_analyzer::simple_expressionWhile() {
	simpleExpr = true;
	terminal();
	precalc.expr.pop_back();

	while (lex.s == "+" || lex.s == "-" || lex.s == "||") {
		addition_operation();
		//getc(lex);

		terminal();
		//getc(lex);
	}

	simpleExpr = false;
}

void syntax_analyzer::ratio_operation() {
	if (lex.s == "!=" || lex.s == "<" || lex.s == ">" || lex.s == "==" || lex.s == "<=" || lex.s == ">=") {
		ratioOper = lex.s;
		getc(lex);
	} else {
		Lexeme t;
		watch(t, 1);

		if (t.s == "="&&lex.s == "!") {
			getc(lex);
			getc(lex);

			ratioOper="!=";
			return;
		}

		throw(Error("ration operation expected", lex.line));
	}

	//getc(lex); //FLAG
}
void syntax_analyzer::simple_expression() { //poliz working
	 
	simpleExpr = true;
	terminal();

	while (lex.s == "+" || lex.s == "-" || lex.s == "||") {
		addition_operation();
		//getc(lex);

		terminal();
		//getc(lex);
	}

	simpleExpr = false;
}
void syntax_analyzer::terminal() { //poliz working
	atom_1();

	while (lex.s == "*" || lex.s == "/" || lex.s == "&&" || lex.s == "div" || lex.s == "%") {
		multiplication_operation();
		atom_1();
	}

}
void syntax_analyzer::atom_1() { //poliz working
	int i = 1;
	Lexeme t = lex;

	while (t.t != PUNCT&&t.s != "^"&&t.s != "++"&&t.s != "--") {
		watch(t, i);
		i++;
	}

	if (t.t == PUNCT)
		atom();
	else {
		if (t.s == "^") {
			atom();
			exponentiation();
			atom();
		} else {
			atom();
			increment();
		}
	}
}
void syntax_analyzer::atom() { //poliz working
	if (lex.s == "(") {
		//precalc.expr += '(';
		if (!exprIsNow)
			addToExpr();

		getc(lex);

		expression();

		//if (!exprIsNow)
		//addToExpr();

		if (lex.s != ")")
			throw(Error(") expected", lex.line));

		getc(lex);

	} else {
		if (lex.t == IDENT) {
			if (!simpleExpr) name();
			else
				nameForExpr();
			//getc(lex);// или не здесь, а перед инк. // ЭТО БЫЛО У ОЛИ. У ЮРЫ НЕ БЫЛО //если убрать, то работает (by Yura)
			if (lex.s == ";")return;// expression must be finished for this time
		} else
			special_atom();
	}

	//exprIsNow = false; //FLAG с комментом синаут работает
}
void syntax_analyzer::special_atom() { //ФЛАГ всё норм с ! ??? //poliz working
									   //должно быть норм
	if (lex.s == "!") {
		if (!exprIsNow)
			addToExpr();

		getc(lex);
		atom();
	}

	if (isLet(lex.s[0]))
		bool_value();
	else if (lex.s == "=") {
		//надо, кажется, пихнть сслылку на индетификатор - куда присваивать
		//ЭТО НЕ РАБОТАЕТ!!!
		getc(lex);
		expression();
	} else {
		constant();
	}
}
void syntax_analyzer::addition_operation() {
	if (lex.s != "+"&&lex.s != "-"&&lex.s != "||")
		throw(Error("addition operation expected", lex.line));
	//addToExpr();
	getc(lex);
}
void syntax_analyzer::multiplication_operation() {
	if (lex.s != "*"&&lex.s != "/"&&lex.s != "&&"&&lex.s != "div"&&lex.s != "%")
		throw(Error("addition operation expected", lex.line));

	//pol.push(lex.s);
	getc(lex);
}
void syntax_analyzer::exponentiation() {
	if (lex.s != "^")
		throw(Error("exponentiation expected", lex.line));

	//pol.push(lex.s);
	getc(lex);
}
void syntax_analyzer::assigment_operation() {
	assigment = true;
	if (lex.s != "=")
		throw(Error("assigment operation expected", lex.line));

	getc(lex);
}
void syntax_analyzer::increment() {
	if (lex.s != "--" && lex.s != "++")
		throw(Error("increment or decrement expected", lex.line));

	string var = precalc.expr.back().s;
	pol.push(elemOfPoliz(Ident(var)));
	if(lex.s=="++")
		precalc.expr.push_back(Lexeme(OPER, "+"));
	else
		precalc.expr.push_back(Lexeme(OPER, "-"));

	precalc.expr.push_back(Lexeme(CONST, "1"));
	precalc.expr.push_back(Lexeme(CONST, ";")); //костыль, экспрешион убирает последний элемент -> убирет ; 

	assigment = true;
	getc(lex);
}
void syntax_analyzer::sign() {
	if (lex.s != "-"&&lex.s != "+")
		throw(Error("sign expected", lex.line));

	//pol.push(lex.s);
	getc(lex);
}
void syntax_analyzer::constant() {
	if (lex.t != CONST)
		throw(Error("constant expected", lex.line));

	addToExpr(); //так вайл работает
	exprIsNow = false;
	getc(lex);
}
void syntax_analyzer::bool_value() {
	if (lex.s == "return")
		getc(lex);
	else
		if (lex.s != "true"&&lex.s != "false")
			throw(Error("bool value expected", lex.line));

	if (lex.s == "true")
		precalc.expr.push_back(Lexeme(CONST, "1"));
		//precalc.expr.back().s = "1";
	else
		precalc.expr.push_back(Lexeme(CONST, "0"));
		//precalc.expr.back().s = "0";

	getc(lex);
}
///////////////////////////////////////////////////
void syntax_analyzer::input_output_operator() { //poliz working
	if (lex.s != "cinout")throw(Error("cinout expected, but not this heresy", lex.line));
	getc(lex);
	list_of_elements();
	if (lex.s != ";")throw(Error("; expected...where is my ; ? :(", lex.line));
	getc(lex);
}
void syntax_analyzer::list_of_elements() { //poliz working
	element();
	while (lex.s != ";") {
		element();
		getc(lex);
	}
}
void syntax_analyzer::element() {//poliz working here
	pushExprInPol = true;
	cinoutNow = true;

	if (lex.s == "<<") {
		getc(lex);
		if (lex.strbool == true) {
			pol.push(lex.s);
			getc(lex);
			return;////?
		} else if (lex.s == "endl") {
			pol.push(Ident('\n'));
			return;
		} else expression();

		pol.push(string("<<"));
	} else if (lex.s == ">>") {
		getc(lex);
		//pol.push(Ident(lex.s));
		nameIn();

		pol.push(string(">>"));
	} else throw(Error("<< or >> expected", lex.line));

	cinoutNow = false;
}
void syntax_analyzer::composite_operator() { //poliz working
	if (lex.s != "{")throw(Error("{ expected", lex.line));
	getc(lex);

	if (lex.s != "}")	Operator();
	while (lex.s != ";" && lex.s != "}"&& lex.s != "return") {
		Operator();
	}

	if (lex.s == "}" || lex.s == "return")
		getc(lex);
	else
		throw(Error("} expected", lex.line));
}
void syntax_analyzer::description() {
	descript = true;
	type();

	SemA.push_name_in_set(lex);

	section();
	while (lex.s == ",") {
		getc(lex);
		SemA.push_name_in_set(lex);
		section();
	}if (lex.s != ";")throw(Error(";  expected...where is my ; ? :(", lex.line));
	getc(lex); //ЕСЛИ ЭТО ЗАКОММЕНТИТЬ, ТО ПРИСВАИВАНИЕ РАБОТАЕТ, НО НЕ РАБОТАЕТ СЕКЦИЯ

	descript = false;
}
void syntax_analyzer::section() {
	int i = 1; bool t = false;
	while (i <= 2) {
		watch(buff, i);
		if (buff.s == "=")t = true;
		i++;
	}
	if (t) {
		nameForSect();
		//now , after name calling, lex.s=="="
		getc(lex);//next lexeme //чтобы работало присваивание в expr это надо (by Yura)
		assigment = true;
		expression();
	} else name();
}
void ::syntax_analyzer::type() {
	if (lex.s != "int" && lex.s != "bool" && lex.s != "double"&&lex.s != "char")throw(Error("Unknown type", lex.line));
	if (lex.s == "int")TYPE = INT;
	else if (lex.s == "bool")TYPE = BOOL;
	else if (lex.s == "double")TYPE = LONG_DOUBLE;//costyl
	else if (lex.s == "char")TYPE = CHAR;
	getc(lex);
}
void syntax_analyzer::special_operator() {
	if (lex.s == "do") {
		getc(lex);
		dowhile_operator();
	} else for_operator();
}
void syntax_analyzer::dowhile_operator() { //не смущайся, все норм, оператор - {...}  POLIZ HERE
										   //don't need to check "do"existence
	pushExprInPol = false;
	whileOrForBody = true;

	int indexOfBodyWhile = (int) pol.pol.size();
	Operator();

	//vector<Lexeme> bodyOfWhile = precalc.expr;
	//pol.push(precalc.expr);

	precalc.expr.clear();
	pushExprInPol = true;

	if (lex.s != "while")throw(Error("while expected", lex.line));
	getc(lex);
	if (lex.s != "(")throw(Error("( expected", lex.line));
	getc(lex);
	expressionForWhile();

	int IndexOfAddressOfS2 = pol.pol.size(); //look in grammatic and you will understand
	pol.push(-1);

	elemOfPoliz e(elemOfPoliz::RETRANS);
	pol.push(e);

	pol.push(indexOfBodyWhile); //check this horror!

	elemOfPoliz ee(elemOfPoliz::TRANS);
	pol.push(ee);

	pol.pol[IndexOfAddressOfS2] = elemOfPoliz((int) pol.pol.size());

	if (lex.s != ")")throw(Error(") expected", lex.line));
	getc(lex);
	if (lex.s != ";")throw(Error("; expected", lex.line));
	whileOrForBody = false;
	getc(lex);

}
void syntax_analyzer::for_operator() {
	whileOrForBody = true;
	if (lex.s != "for") throw(Error("for expected", lex.line));
	getc(lex);
	if (lex.s != "(") throw(Error("( expected", lex.line));
	getc(lex);
	buff = lex;
	bool t = false; int i = 1;
	while (buff.s != "none") {
		watch(buff, i);
		if (buff.s == "else") {
			t = true;
			break;
		}
		i++;
	}

	int indexOfAdressOfExitOfFor; //жуткий костыль. надо передавать из фора индекс, где должно быть записан индекс след элемента в полизе за фором

	if (lex.s != "int" && lex.s != "double" && lex.s != "bool") {
		buff = lex; bool t = false;
		int i = 1;
		while (buff.s != "none") {
			watch(buff, i);
			if (buff.s == "to" || buff.s == "downto")t = true;
			i++;
		}
		if (t)pfor_operator();
		else cfor_operator(indexOfAdressOfExitOfFor);
	} else {
		cfor_operator(indexOfAdressOfExitOfFor);
	}
	if (t) else_branch(indexOfAdressOfExitOfFor);
	else pol.pol[indexOfAdressOfExitOfFor] = (int) pol.pol.size();

	whileOrForBody = false;
}
void syntax_analyzer::cfor_operator(int& indexOfAdressOfExitOfFor) { //dangeous item, need to check // poliz here
										// you mustn't check the for existence
	if (lex.s == "int" || lex.s == "bool" || lex.s == "double") {
		description();

		pushExprInPol = true;
		expression(); //условие

		int indexOfAdressOfElseOfFor = (int) pol.pol.size();
		pol.push(-1);

		elemOfPoliz e(elemOfPoliz::RETRANS);
		pol.push(e);

		int indexOfStartOfFor = (int) pol.pol.size();

		pol.push(pol.pol[(int) pol.pol.size() - 5]); //добавляю условие повторно для выхода, а не для элза
		pol.push(pol.pol[(int) pol.pol.size() - 5]); //добавляю условие повторно для выхода, а не для элза
		pol.push(pol.pol[(int) pol.pol.size() - 5]); //добавляю условие повторно для выхода, а не для элза

		indexOfAdressOfExitOfFor = (int) pol.pol.size();
		pol.push(-1);

		pol.push(e);

		if (lex.s != ";")throw(Error("; expected", lex.line));
		getc(lex);

		pushExprInPol = false;
		expression();
		stack<elemOfPoliz> operOfFor;

		for (size_t i = 0; i < 3; i++) {
			operOfFor.push(pol.pol.back());
			pol.pol.pop_back();
		}
		
		pushExprInPol = true;

		if (lex.s != ")")throw(Error(") expected", lex.line));
		getc(lex);
		Operator();

		for (size_t i = 0; i < 3; i++) {
			pol.pol.push_back(operOfFor.top());
			operOfFor.pop();
		}

		pol.push(indexOfStartOfFor);
		elemOfPoliz ee(elemOfPoliz::TRANS);
		pol.push(ee);

		pol.pol[indexOfAdressOfElseOfFor] = (int) pol.pol.size();
	} else {
		pushExprInPol = true;
		expression(); //условие

		int indexOfAdressOfElseOfFor = (int) pol.pol.size();
		pol.push(-1);

		elemOfPoliz e(elemOfPoliz::RETRANS);
		pol.push(e);

		int indexOfStartOfFor = (int) pol.pol.size();

		pol.push(pol.pol[(int) pol.pol.size() - 5]); //добавляю условие повторно для выхода, а не для элза
		pol.push(pol.pol[(int) pol.pol.size() - 5]); //добавляю условие повторно для выхода, а не для элза
		pol.push(pol.pol[(int) pol.pol.size() - 5]); //добавляю условие повторно для выхода, а не для элза

		indexOfAdressOfExitOfFor = (int) pol.pol.size();
		pol.push(-1);

		pol.push(e);

		if (lex.s != ";")throw(Error("; expected", lex.line));
		getc(lex);

		pushExprInPol = false;
		expression();
		stack<elemOfPoliz> operOfFor;

		for (size_t i = 0; i < 3; i++) {
			operOfFor.push(pol.pol.back());
			pol.pol.pop_back();
		}

		pushExprInPol = true;

		if (lex.s != ")")throw(Error(") expected", lex.line));
		getc(lex);
		Operator();

		for (size_t i = 0; i < 3; i++) {
			pol.pol.push_back(operOfFor.top());
			operOfFor.pop();
		}

		pol.push(indexOfStartOfFor);
		elemOfPoliz ee(elemOfPoliz::TRANS);
		pol.push(ee);

		pol.pol[indexOfAdressOfElseOfFor] = (int) pol.pol.size();
	}
}
void syntax_analyzer::pfor_operator() {
	name();
	//getc(lex);
	//maybe := - mistake. we don't have this operator....
	if (lex.s != "=")throw(Error("= expected, but not this heresy", lex.line));
	getc(lex);
	expression();
	way();
	expression();
	if (lex.s != ")") throw(Error(") expected", lex.line));
	getc(lex);
	Operator();
}
void syntax_analyzer::way() {
	if (lex.s != "to" && lex.s != "downto")throw(Error("to or downto expected,  but not this heresy", lex.line));
	getc(lex);
}
void syntax_analyzer::else_branch(int indexOfAdressOfExitOfFor) {
	if (lex.s != "else")throw(Error("else expected, but not this heresy", lex.line));
	getc(lex);
	Operator();

	pol.pol[indexOfAdressOfExitOfFor] = (int)pol.pol.size();
}
