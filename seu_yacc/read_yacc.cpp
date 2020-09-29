#include "read_yacc.h"
#include<fstream>
#include<iostream>
#include<string>

using namespace std;


void read_yacc(string filename,vector<string>& tokensVec,vector<pair<int,vector<string>>>& tempRules, 
	vector<pair<string,vector<string>>>& productionStrVec,vector<string>& functions)
{
	ifstream in;
	in.open(filename.c_str(),ios::in);
	if(!in){
		cout << "\nread_lex_from_file: Failed to open yacc.txt!" << endl;
		exit(1);
	}

	string curpar;//存储当前词组，以空格为分隔符
	int state = 0; //表示当前处于yacc.txt的文件的位置
					//1表示在token段，2表示在结合规则段
					//3表示在产生式段，4表示在用户定义函数段
					//-1表示yacc.txt的形式不对
	int lor;
	vector<string> temp,rig;
	string lef;

	while (!in.eof())
		switch (state)
		{
			case 0:
				in >> curpar;
				if  (curpar  !=  "%token") state  =  -1;
				else {
					state = 1;
					in>>curpar;
				}
				break;
			case 1:
				while (curpar != "%right" && curpar != "%left") {
					while (curpar != "%token"&& curpar != "%right" && curpar != "%left") {
						tokensVec.push_back(curpar);
						in >> curpar;
					}
					if (curpar == "%right" || curpar == "%left") {
						//in >> curpar;
						break;
					}
					in >> curpar;
				}
				curpar == "%right" ? lor = 1 : lor = 0;
				state = 2;
				in >> curpar;
				break;
			case 2:
				while (curpar != "%%") {
					while (curpar != "%right" && curpar != "%left" && curpar != "%%") {
						temp.push_back(curpar);
						in >> curpar;
					}
					if (curpar == "%%") {
						tempRules.push_back(pair<int, vector<string>>(lor, temp));
						curpar == "%right" ? lor = 1 : lor = 0;
						temp.clear();
						break;
					}
					tempRules.push_back(pair<int, vector<string>>(lor, temp));
					curpar == "%right" ? lor = 1 : lor = 0;
					temp.clear();
					in >> curpar;
				}
				state = 3;
				in >> curpar;
				break;
			case 3:
				while (curpar != "%%") {
					lef = curpar;
					in >> curpar;
					in >> curpar;
					while (curpar != ";") {
						while (curpar != ";" && curpar != "|") {
							temp.push_back(curpar);
							in >> curpar;
						}
						productionStrVec.push_back(pair<string, vector<string>>(lef, temp));
						temp.clear();
						if (curpar == ";") break;
						in >> curpar;
					}
					in >> curpar;
				}
				state = 4;
				break;
			case 4:
				getline(in, curpar);
				functions.push_back(curpar);
				break;
		}
}