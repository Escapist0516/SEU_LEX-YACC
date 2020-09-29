#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include "GlobalData.h"
using namespace std;

//���ݽṹ��������ʽ��nfa��dfa��

//����
typedef struct  Rules
{
	string pattern;			//RE
	vector<string> actions; //��Ӧ��������䣩
};

//�����Զ���

//NFA�ڲ�״̬
typedef struct  NFAstate
{
	int num = 0;								//״̬���
	multimap<char, int> EdgeMultiMap; //������,Key=���ϵ��ַ���Value=��һ��״̬���
};

//NFA
typedef struct  NFA
{
	int startState = 0;						//��ʼ״̬���
	map<int, int> endStatesMap;				//�洢��̬�Ͷ�Ӧ�Ķ���
	map<int, NFAstate> statesMap;           //�洢��Ŷ�Ӧ״̬
};

//DFA�ڲ�״̬ 
typedef struct  DFAstate
{
	int num = 0;					  //״̬���
	set<int> subSet;		          //״̬�Ӽ�
	map<char, int> EdgeMap;           //������,Key=���ϵ��ַ���Value=��һ��״̬���
};

//DFA
typedef struct DFA
{
	int startState = 0;					  //��ʼ״̬���
	map<int, int> endStatesMap;           //�洢��̬�Ͷ�Ӧ�Ķ���
	vector<DFAstate> statesVec;			  //�洢��Ŷ�Ӧ״̬
} ;
