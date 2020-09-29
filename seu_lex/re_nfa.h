#pragma once
#include"GlobalData.h"
#include"structs.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <stack>
using namespace std;

void re_standardize(vector<Rules>& Rules, map<string, string>& head);//规范化正则表达式
void re_to_postfix(vector<Rules>& rules);//前缀表达式转后缀表达式

										
stack<NFA> postfix_to_nfa(vector<Rules>& postfixRules); //后缀表达式转NFA
void nfa_combination(stack<NFA> NFAstack,NFA& finalNFA);//合并NFA
