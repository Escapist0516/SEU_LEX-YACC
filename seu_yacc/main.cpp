/*
  main.cpp

  seu_yacc
  start time:2020/05/29
*/

#include "convert.h"
#include "structs.h"
#include "calculate_first.h"
#include "construct_lr1.h"
#include "lr1_to_lalr.h"
#include "read_yacc.h"
#include "gengerate_code.h"
#include "gengerate_code.h"
#include "construct_table.h"
#include <iostream>
#include <ctime>
using namespace std;

int main()
{
	//*****************************参数*************************************
	string in_filename = "yacc.txt";
	string out_filename = "yacc_lr_1_construct_way.cpp";
	string out_filename_2 = "D:\\Coding\\CompileMethod\\test\\test\\test\\yacc.cpp";

	vector<pair<int, vector<int>>> production_vec;
	//产生式 int：左边，vector<int>：右边 int为tokens所对应的序号

	vector<pair<string, vector<string>>> production_str_vec;
	//产生式 string：左边，vector<string>：右边

	vector<string> tokensVec;
	//存储所有的token
	vector<pair<int, vector<string>>> tempRules;
	//前面0表示左，1表示右，后面是规则，字符串版本

	vector<pair<int, vector<int>>> tempRulesInt;
	//前面0表示左，1表示右，后面是规则

	map<int, pair<int, int>> indexMap;
	//映射：symbol->productionVec的下标。key为symbol，value为<以该symbol为左部的产生式在productionVec中的起始index，数量>

	map<string, int> tokensMap;
	//映射：tokens -> 标号，<tokens,count>

	map<int, set<int>> firstMap;
	//映射：symbol->first集，<symbol,first集>

	vector<string>functions;
	//用户函数

	// char + tokens：0-boundT,非终结符:boundT+1-boundN
	// startInt 初始状态号
	int boundT = -1;
	int boundN = -1;
	int startInt;

	LRDFA lr1_dfa;
	//lr dfa
	LRDFA lalr_dfa,lalr_dfa_2;
	//lalr dfa
	vector<vector<pair<char, int>>>table,table_2;
	//语法分析表 char：r s e a 

	//***************************开始运行yacc***************************************

	cout << "\nmain: seu yacc start running!" << endl;
	//开始运行
	clock_t startTime = clock();
	//启动计时器

	cout << "\nmain: reading yacc file..." << endl;
	read_yacc(in_filename,tokensVec,tempRules, production_str_vec,functions);
	//读取yacc文件

	cout << "\nmain: doing some convert work..." << endl;
	convert(production_str_vec, production_vec, tokensMap, tokensVec, boundT, boundN, tempRules, tempRulesInt, indexMap);
	//对文法的处理将产生式的的string形式映射到int形式

	cout << "\nmain: calculating first set..." << endl;
	first(boundN, firstMap, indexMap, boundT, production_vec);
	//求所有非终结符的first集

	//cout << "\nmain: constructing lr1 dfa... may cost a long time, please wait..." << endl;
	//construct_lr1(lr1_dfa, production_vec, indexMap, boundT, firstMap);
	//构造LR(1)

	//cout << "\nmain: trans lr1 dfa to lalr dfa..." << endl;
	//lr1_to_lalr(lr1_dfa,lalr_dfa);
	//LR(1)转换成LALR

	cout << "\nmain: constructing lalr..." << endl;
	quick_construct_lalr(lalr_dfa_2, production_vec, indexMap, boundT, firstMap);
	//快速构造lalr1的方法


	/*cout << endl;
	for (int i = 0; i < lalr_dfa.statesVec.size(); i++)
	{
		cout << i << " " << lalr_dfa.statesVec[i].LRItemsSet.size() << " " << lalr_dfa_2.statesVec[i].LRItemsSet.size() << endl;
	}*/

	cout << "\nmain: constructing table..." << endl;
	//construct_table(boundN,boundT, lalr_dfa, production_vec,tempRulesInt,table);
	construct_table(boundN,boundT, lalr_dfa_2, production_vec,tempRulesInt,table_2);
	//构造语法分析表

	cout << "\nmain: generating code now..." << endl;
	//generate_code(out_filename, functions, table, tokensMap, production_vec, boundT);
	generate_code(out_filename_2, functions, table_2, tokensMap, production_vec, boundT);
	//代码生成

	clock_t endTime = clock();
	cout << "\nmain: all finished!  Total time = " << ((endTime - startTime) / CLOCKS_PER_SEC) * 1000 << "ms\n" << endl;

	return 0;
}