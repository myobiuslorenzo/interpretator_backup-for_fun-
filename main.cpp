#include "includes.h"
#include "interface.h"

int main(){
	try { //писать тут
		LecsAnalyzer LA; 
		LA.Analyze();
		syntax_analyzer SA("Output.txt",LA.BOL.finalmasoflex);

		SA.program();
		SA.pol.scan();

/*		Precalculator p;
		string s;
		cin >> s;
		cout<<p.calculate(s);*/
		

		cout << "OK\n";
    }
    catch(Error er){
		cout << "ERROR : " << er.str << " line: " << er.line << endl;
		system("pause");
		return 1;
    }
	system("pause");
	return 0;
}


