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

void re_standardize(vector<Rules>& Rules, map<string, string>& head);//�淶��������ʽ
void re_to_postfix(vector<Rules>& rules);//ǰ׺���ʽת��׺���ʽ

										
stack<NFA> postfix_to_nfa(vector<Rules>& postfixRules); //��׺���ʽתNFA
void nfa_combination(stack<NFA> NFAstack,NFA& finalNFA);//�ϲ�NFA
