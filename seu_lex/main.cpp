/*
seu_lex
2020/05/19
*/
#include <iostream>
#include <ctime>
#include "structs.h"
#include "read_lex.h"
#include "generate_code.h"
#include "re_nfa.h"
#include "nfa_dfa.h"

using namespace std;

int main()
{
	vector<Rules> rules;//���򣨵�3���֣�
	map<string, string> head;
	vector<string> supdef;
	vector<string> functions;


	stack<NFA> nfa;
	NFA final_nfa;
	DFA dfa,final_dfa;

	cout << "seu_lex 2020/05/23 running...." << endl;
	clock_t start = clock();
	//��������������ʱ��

	string filename="lex.txt";

	cout << "\nmain: reading lex file named:" << filename << "..." << endl;
	read_lex_from_file(filename,supdef,head,rules,functions);
	//��lex�ļ��ж�ȡ ���塢������û��ӳ���
	
	//cout << head.begin()->second << endl;
	//cout <<supdef.size() << endl;
	//cout << functions.size() << endl;
	cout << "\nmain: there are  " << rules.size() << " rules" << endl;

	cout << "\nmain: regular experession standardizing..." << endl;
	re_standardize(rules,head);
	//�淶��������ʽ 

	re_to_postfix(rules);//ǰ׺���ʽת��׺���ʽ

	cout << "\nmain: regular experession to nfa ..." << endl;
	nfa=postfix_to_nfa(rules);
	//��׺���ʽת����NFA

	cout << nfa.size() << endl;

	cout << "\nmain: nfa combination..." << endl;
	nfa_combination(nfa,final_nfa);
	//���NFA�ϲ�

	map<int, NFAstate>::iterator it;
	int num = 0;
	it = final_nfa.statesMap.begin();
	while (it != final_nfa.statesMap.end())
	{
		it++;
		num++;
	}
	cout << "\nmain: final nfa's states num: "<<num << endl;

	cout << "\nmain: nfa to dfa..." << endl;
	nfa_to_dfa(final_nfa,dfa);
	//NFAת��ΪDFA

	cout << "\nmain: dfa's states num:  " << dfa.statesVec.size() << endl;

	cout << "\nmain: dfa minimize..." << endl;
	dfa_mini(dfa,final_dfa);
	//DFA��С��

	cout << "\nmain: minisized dfa's states num: " << final_dfa.statesVec.size() << endl;

	string output_filename = "D:\\Coding\\CompileMethod\\test\\test\\test\\lex.cpp";
	cout << "\nmain: generating code to " << output_filename << "..." << endl;
	generate_code(output_filename, supdef, dfa, rules, functions);
	//������뵽���

	cout << "\nmain: all work done! " << endl;
	//�����������
	clock_t end = clock();
	cout << "\nmain: total time: " << 1000 * (end - start) / CLOCKS_PER_SEC << "ms" << endl;
	//��ֹ��ʱ�������ʱ��
	return 0;
}