#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include "GlobalData.h"
using namespace std;

//数据结构：正则表达式，nfa，dfa等

//规则
typedef struct  Rules
{
	string pattern;			//RE
	vector<string> actions; //对应动作（语句）
};

//有限自动机

//NFA内部状态
typedef struct  NFAstate
{
	int num = 0;								//状态标号
	multimap<char, int> EdgeMultiMap; //发出边,Key=边上的字符，Value=下一个状态标号
};

//NFA
typedef struct  NFA
{
	int startState = 0;						//开始状态标号
	map<int, int> endStatesMap;				//存储终态和对应的动作
	map<int, NFAstate> statesMap;           //存储标号对应状态
};

//DFA内部状态 
typedef struct  DFAstate
{
	int num = 0;					  //状态标号
	set<int> subSet;		          //状态子集
	map<char, int> EdgeMap;           //发出边,Key=边上的字符，Value=下一个状态标号
};

//DFA
typedef struct DFA
{
	int startState = 0;					  //开始状态标号
	map<int, int> endStatesMap;           //存储终态和对应的动作
	vector<DFAstate> statesVec;			  //存储标号对应状态
} ;
