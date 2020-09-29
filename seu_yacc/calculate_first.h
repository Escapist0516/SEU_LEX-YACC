#pragma once
#ifndef CALCULATE_FIRST_H
#define CALCULATE_FIRST_H
#include "structs.h"
using namespace std;
//将集合b并入a
void firstunion(set<int>& a, const set<int>& b);
//求符号symbol的first集
void first_symbol(set<int>& firstSet, const int& symbol, set<int>& handlingSymbol,
	map<int, set<int>>& firstMap, map<int, pair<int, int>>& indexMap, int& boundT, vector<pair<int, vector<int>>>& productionVec);
//求序列symbolsVec的first集
void first_string(set<int>& firstSet, const vector<int>& symbols, map<int, set<int>> &firstMap);
//求first集
void first(int& boundN, map<int, set<int>>& firstMap, map<int, pair<int, int>>& indexMap, int& boundT, vector<pair<int, vector<int>>>& productionVec);
#endif