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
	//*****************************����*************************************
	string in_filename = "yacc.txt";
	string out_filename = "yacc_lr_1_construct_way.cpp";
	string out_filename_2 = "D:\\Coding\\CompileMethod\\test\\test\\test\\yacc.cpp";

	vector<pair<int, vector<int>>> production_vec;
	//����ʽ int����ߣ�vector<int>���ұ� intΪtokens����Ӧ�����

	vector<pair<string, vector<string>>> production_str_vec;
	//����ʽ string����ߣ�vector<string>���ұ�

	vector<string> tokensVec;
	//�洢���е�token
	vector<pair<int, vector<string>>> tempRules;
	//ǰ��0��ʾ��1��ʾ�ң������ǹ����ַ����汾

	vector<pair<int, vector<int>>> tempRulesInt;
	//ǰ��0��ʾ��1��ʾ�ң������ǹ���

	map<int, pair<int, int>> indexMap;
	//ӳ�䣺symbol->productionVec���±ꡣkeyΪsymbol��valueΪ<�Ը�symbolΪ�󲿵Ĳ���ʽ��productionVec�е���ʼindex������>

	map<string, int> tokensMap;
	//ӳ�䣺tokens -> ��ţ�<tokens,count>

	map<int, set<int>> firstMap;
	//ӳ�䣺symbol->first����<symbol,first��>

	vector<string>functions;
	//�û�����

	// char + tokens��0-boundT,���ս��:boundT+1-boundN
	// startInt ��ʼ״̬��
	int boundT = -1;
	int boundN = -1;
	int startInt;

	LRDFA lr1_dfa;
	//lr dfa
	LRDFA lalr_dfa,lalr_dfa_2;
	//lalr dfa
	vector<vector<pair<char, int>>>table,table_2;
	//�﷨������ char��r s e a 

	//***************************��ʼ����yacc***************************************

	cout << "\nmain: seu yacc start running!" << endl;
	//��ʼ����
	clock_t startTime = clock();
	//������ʱ��

	cout << "\nmain: reading yacc file..." << endl;
	read_yacc(in_filename,tokensVec,tempRules, production_str_vec,functions);
	//��ȡyacc�ļ�

	cout << "\nmain: doing some convert work..." << endl;
	convert(production_str_vec, production_vec, tokensMap, tokensVec, boundT, boundN, tempRules, tempRulesInt, indexMap);
	//���ķ��Ĵ�������ʽ�ĵ�string��ʽӳ�䵽int��ʽ

	cout << "\nmain: calculating first set..." << endl;
	first(boundN, firstMap, indexMap, boundT, production_vec);
	//�����з��ս����first��

	//cout << "\nmain: constructing lr1 dfa... may cost a long time, please wait..." << endl;
	//construct_lr1(lr1_dfa, production_vec, indexMap, boundT, firstMap);
	//����LR(1)

	//cout << "\nmain: trans lr1 dfa to lalr dfa..." << endl;
	//lr1_to_lalr(lr1_dfa,lalr_dfa);
	//LR(1)ת����LALR

	cout << "\nmain: constructing lalr..." << endl;
	quick_construct_lalr(lalr_dfa_2, production_vec, indexMap, boundT, firstMap);
	//���ٹ���lalr1�ķ���


	/*cout << endl;
	for (int i = 0; i < lalr_dfa.statesVec.size(); i++)
	{
		cout << i << " " << lalr_dfa.statesVec[i].LRItemsSet.size() << " " << lalr_dfa_2.statesVec[i].LRItemsSet.size() << endl;
	}*/

	cout << "\nmain: constructing table..." << endl;
	//construct_table(boundN,boundT, lalr_dfa, production_vec,tempRulesInt,table);
	construct_table(boundN,boundT, lalr_dfa_2, production_vec,tempRulesInt,table_2);
	//�����﷨������

	cout << "\nmain: generating code now..." << endl;
	//generate_code(out_filename, functions, table, tokensMap, production_vec, boundT);
	generate_code(out_filename_2, functions, table_2, tokensMap, production_vec, boundT);
	//��������

	clock_t endTime = clock();
	cout << "\nmain: all finished!  Total time = " << ((endTime - startTime) / CLOCKS_PER_SEC) * 1000 << "ms\n" << endl;

	return 0;
}