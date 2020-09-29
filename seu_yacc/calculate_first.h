#pragma once
#ifndef CALCULATE_FIRST_H
#define CALCULATE_FIRST_H
#include "structs.h"
using namespace std;
//������b����a
void firstunion(set<int>& a, const set<int>& b);
//�����symbol��first��
void first_symbol(set<int>& firstSet, const int& symbol, set<int>& handlingSymbol,
	map<int, set<int>>& firstMap, map<int, pair<int, int>>& indexMap, int& boundT, vector<pair<int, vector<int>>>& productionVec);
//������symbolsVec��first��
void first_string(set<int>& firstSet, const vector<int>& symbols, map<int, set<int>> &firstMap);
//��first��
void first(int& boundN, map<int, set<int>>& firstMap, map<int, pair<int, int>>& indexMap, int& boundT, vector<pair<int, vector<int>>>& productionVec);
#endif