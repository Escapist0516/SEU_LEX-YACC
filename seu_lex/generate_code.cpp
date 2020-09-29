#include "generate_code.h"
#include "GlobalData.h"

void generate_code(string filename, vector<string>& supDef, DFA final_dfa, vector<Rules> rules, vector<string> functions)
{
	vector<vector<int>>dfaTable;
	for (int t = 0; t < final_dfa.statesVec.size(); ++t) {
		vector<int>temp;
		for (int t1 = 0; t1 < ALLSET.size(); ++t1)
			temp.push_back(-1);
		for (auto p : final_dfa.statesVec[t].EdgeMap)
			temp[ALLSET.find_first_of(p.first)] = p.second;
		dfaTable.push_back(temp);
	}
	vector<int>sva;
	for (int t = 0; t < final_dfa.statesVec.size(); ++t)
		sva.push_back(-1);
	for (auto p : final_dfa.endStatesMap)
		sva[p.first] = p.second;
	ofstream out;
	out.open(filename, ios::out);
	out << "#include<iostream>" << endl;
	out << "#include<fstream>" << endl;
	out << "#include<string>" << endl;
	for (int t = 0; t < supDef.size(); t++)
		out << supDef[t] << endl;
	out << "using namespace std;" << endl;
	out << "ofstream out;" << endl;
	out << "ifstream in;" << endl;
	out << "char c;" << endl;
	out << "int state1;" << endl;
	out << "int state2;" << endl;
	out << "int dfaTable [" << dfaTable.size() << "][" << ALLSET.size() << "]={" << endl;
	for (int i = 0; i < dfaTable.size(); ++i) {
		for (int j = 0; j < ALLSET.size(); ++j)
			out << dfaTable[i][j] << ',';
		out << endl;
	}
	out << "};" << endl;
	out << "int endS[" << final_dfa.statesVec.size() << "]={";
	for (int t = 0; t < sva.size(); ++t)
		out << sva[t] << ',';
	out << "};" << endl;
	out << "const string ALLSET(\"abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!\\\"#%'()*+,-./:;<=>\?[\\\\]^{|} \\n\\t\\v\\f~&\");" << endl;

	out << "string act(int p)" << endl;
	out << "{" << endl;
	out << "\tswitch(p)" << endl;
	out << "\t{" << endl;
	for (int t = 0; t < rules.size(); ++t) {
		out << "\t\tcase " << t << ':' << endl;
		for (int t1 = 0; t1 < rules[t].actions.size(); ++t1)
			out << "\t\t" << rules[t].actions[t1] << endl;
		out << "\t\tbreak;" << endl;
	}
	out << "\t}" << endl;
	out << "\treturn \"\";" << endl;
	out << "}" << endl;

	out << "string analysis(char& before,char& next,int& state)" << endl;
	out << "{" << endl;
	out << "\tstate1=dfaTable[state][ALLSET.find_first_of(before)];" << endl;
	out << "\tif(state1!=-1)" << endl;
	out << "\t{" << endl;
	out << "\t\tstate2=dfaTable[state1][ALLSET.find_first_of(next)];" << endl;
	out << "\t\tif(state2==-1)" << endl;
	out << "\t\t{" << endl;
	out << "\t\t\tstate = " << final_dfa.startState << ";" << endl;
	out << "\t\t\tout<<before << ',';" << endl;
	out << "\t\t\treturn(act(endS[state1]));" << endl;
	out << "\t\t}" << endl;
	out << "\t\tstate=state1;" << endl;
	out << "\t\tout<<before;" << endl;
	out << "\t\treturn \"\";" << endl;
	out << "\t}" << endl;
	out << "\telse return\"fffffff\";" << endl;
	out << "}" << endl;


	out << "" << endl;
	out << "int main()" << endl;
	out << "{" << endl;
	out << "\tout.open(\"tokens.txt\", ios::out);" << endl;
	out << "\tin.open(\"test.c\", ios::in);" << endl;
	//这里的filename为待解析的源代码的文件名
	out << "\tif(!in.good())" << endl;
	out << "\tcout<<\"open file error!\"<<endl;" << endl;

	//这里的filename为token表的文件目录与文件名
	out << "\tint state=" << final_dfa.startState << ";" << endl;
	
	out << "\tchar next,before=in.get();"<<endl;
	out << "\twhile((c=in.get())!=EOF)" << endl;
	out << "\t{" << endl;
	out << "\t\tnext=c;" << endl;
	out << "\t\tif(c==\'\\n\') continue;" << endl;
	out << "\t\tif(c==\'\\t\') continue;" << endl;
	out << "\t\tstring t = analysis(before, next, state);" << endl;
	out << "\t\tif (t != \"\") out << t << endl;" << endl;
	out << "\t\tbefore=c;" << endl;
	out << "\t}" << endl;
	out << "\tchar ee = 0;" << endl;
	out << "\tstring t = analysis(before, ee, state);" << endl;
	out << "\tif (t != \"\") out << t << endl;" << endl;
	out << "\tout.close();" << endl;
	out << "}" << endl;

	for (int t = 0; t < functions.size(); t++)
		out << functions[t] << endl;

	out << "" << endl;
	out.close();
	return;
}