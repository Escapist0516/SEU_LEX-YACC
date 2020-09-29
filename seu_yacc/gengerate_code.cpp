#include"gengerate_code.h"
#include<fstream>

using namespace std;

void generate_code(string filename,vector<string>& functions, vector<vector<pair<char, int>>>& table, map<string, int>tokensMap, vector<pair<int, vector<int>>>& productionVec,int boundT)
{
	ofstream out;
	out.open(filename);
	if (!out) cout << "open " << filename << " error!" << endl;

	out << "#include<iostream>" << endl;
	out << "#include<fstream>" << endl;
	out << "#include<vector>" << endl;
	out << "#include<string>" << endl;
	out << "#include<stack>" << endl;
	out << "#include<map>" << endl;
	out << "#include<queue>" << endl;
	out << "using namespace std;" << endl;
	out << "" << endl;
	int xx = table.size(), yy = table[0].size();
	out << "pair<char,int> table[" << xx << "][" << yy << "]={";
	for (int i = 0; i < xx; ++i) {
		out << '{';
		for (int j = 0; j < yy; ++j)
			out << "{'" << table[i][j].first << "'," << table[i][j].second << "},";
		out << "}," << endl;
	}
	out << "};" << endl;
	out << "vector<pair<string,vector<string>>>production={" << endl;
	int col = 0;
	for (auto i : productionVec) {
		bool flag = 1;
		for (auto p : tokensMap)
			if (p.second == i.first) {
				out << "{\"" << p.first << "\"," << '{';
				flag = 0;
			}
		if (flag)
			out << "{\"\",{";
		for (auto j : i.second)
			for (auto p : tokensMap)
				if (p.second == j)
					if (p.first == "\"")
						out << "\"\\\"\"";
					else
						out << '"' << p.first << "\",";
		out << "}},";
		col++;
		if (col == 10) {
			out << endl;
			col = 0;
		}
	}
	out << "};" << endl;
	out << "map<string,int>tokensMap={" << endl;
	for (auto& p : tokensMap) {
		if (p.first == "\"")
			out << "{\"\\\"\"," << p.second << "},";
		else
			out << "{\"" << p.first << "\"," << p.second << "},";
		col++;
		if (col == 10) {
			out << endl;
			col = 0;
		}
	}
	out << "};" << endl;
	out << "stack<int>state;" << endl;
	out << "stack<string>sign;" << endl;
	out << "stack<int>node;" << endl;
	out << "queue<string>input;" << endl;
	out << "ofstream out;" << endl;
	out << "ifstream in;" << endl;
	out << "pair<char,int>result;" << endl;
	out << "int cstate, leafNode = 0;" << endl;
	out << "string cinput,lef;" << endl;
	out << "vector<string>rig;" << endl;
	out << "const int dollarPos = " << boundT + 1 << ';' << endl;

	out << "" << endl;
	for (int t = 0; t < functions.size(); ++t)
		out << functions[t] << endl;

	out << "int main()" << endl;
	out << "{" << endl;
	out << "\tstring temp;" << endl;
	out << "\tin.open(\"tokens.txt\");" << endl;
	out << "\tif(!in.good()) cout<<\"open tokens.txt error!\"<<endl;" << endl;
	out << "\twhile(!in.eof()){" << endl;
	out << "\t\tgetline(in,temp);" << endl;
	out << "\t\tif (temp.find_first_of(',') == string::npos) continue;" << endl;
	out << "\t\ttemp=temp.substr(temp.find_first_of(\',\')+1);" << endl;
	out << "\t\tif (temp == \" \") continue;" << endl;
	out << "\t\tinput.push(temp);" << endl;
	out << "\t\tsyntaxTree.push_back(treeNode(temp));" << endl;
	out << "\t}" << endl;
	out << "\tinput.push(\"$\");" << endl;
	out << "\tstate.push(0);" << endl;
	out << "\tdo" << endl;
	out << "\t{" << endl;
	out << "\t\tcstate=state.top();" << endl;
	out << "\t\tcinput=input.front();" << endl;
	out << "\t\tif(cinput==\"$\")" << endl;
	out << "\t\t\tresult= table[cstate][dollarPos];" << endl;
	out << "\t\telse{" << endl;
	out << "\t\t\tif(tokensMap.find(cinput)!=tokensMap.end())" << endl;
	out << "\t\t\t\tresult=table[cstate][tokensMap.find(cinput)->second];" << endl;
	out << "\t\t\telse break;" << endl;
	out << "\t\t}" << endl;
	out << "\t\tif(result.first==\'e\')" << endl;
	out << "\t\t\tcout<<\"There is something wrong!\";" << endl;
	out << "\t\tswitch(result.first)" << endl;
	out << "\t\t{" << endl;
	out << "\t\t\tcase \'s\':" << endl;
	out << "\t\t\t\tstate.push(result.second);" << endl;
	out << "\t\t\t\tsign.push(cinput);" << endl;
	out << "\t\t\t\tnode.push(leafNode);" << endl;
	out << "\t\t\t\t++leafNode;" << endl;
	out << "\t\t\t\tinput.pop();" << endl;
	out << "\t\t\t\tbreak;" << endl;
	out << "\t\t\tcase \'r\':{" << endl;
	out << "\t\t\t\trig=production[result.second].second;" << endl;
	out << "\t\t\t\tlef=production[result.second].first;" << endl;
	out << "\t\t\t\ttreeNode tf(lef);" << endl;
	out << "\t\t\t\tsyntaxTree.push_back(tf);" << endl;
	out << "\t\t\t\tint ts = syntaxTree.size();" << endl;
	out << "\t\t\t\tfor (int i = 0; i < rig.size(); ++i) {" << endl;
	out << "\t\t\t\t\tint sn = node.top();" << endl;
	out << "\t\t\t\t\tsyntaxTree[sn].fa = ts - 1;" << endl;
	out << "\t\t\t\t\tsyntaxTree[ts - 1].sons.push_back(sn);" << endl;
	out << "\t\t\t\t\tnode.pop();" << endl;
	out << "\t\t\t\t}" << endl;
	out << "\t\t\t\treverse(syntaxTree[ts - 1].sons.begin(), syntaxTree[ts - 1].sons.end());" << endl;
	out << "\t\t\t\tfor(int t=0;t<rig.size();++t) sign.pop();" << endl;
	out << "\t\t\t\tsign.push(lef);" << endl;
	out << "\t\t\t\tnode.push(ts - 1);" << endl;
	out << "\t\t\t\tfor(int t=0;t<rig.size();++t) state.pop();" << endl;
	out << "\t\t\t\tif(tokensMap.find(lef)!=tokensMap.end())" << endl;
	out << "\t\t\t\t\tstate.push(table[state.top()][tokensMap.find(lef)->second+1].second);" << endl;
	out << "\t\t\t\telse cout<<\"error in guiyue!\"<<endl;" << endl;
	out << "\t\t\t\t" << endl;
	out << "\t\t\t\tbreak;" << endl;
	out << "\t\t\t}" << endl;
	out << "\t\t\tcase \'a\':" << endl;
	out << "\t\t\t\tdfs1(&syntaxTree[syntaxTree.size() - 1], 0);" << endl;
	out << "\t\t\t\tdfs2(&syntaxTree[syntaxTree.size() - 1]);" << endl;
	out << "\t\t\t\tprintSyntaxTree();" << endl;
	out << "\t\t\t\t" << endl;
	out << "\t\t\t\tbreak;" << endl;
	out << "\t\t\tcase \'e\':" << endl;
	out << "\t\t\t\t" << endl;
	out << "\t\t\t\tbreak;" << endl;
	out << "\t\t}" << endl;
	out << "\t}" << endl;
	out << "\twhile(result.first!=\'a\'&&result.first!=\'e\');" << endl;
	out << "\t" << endl;
	out << "}" << endl;

	out << "" << endl;
}