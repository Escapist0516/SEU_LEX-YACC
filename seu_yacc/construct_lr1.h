#pragma once
#include"structs.h"
void construct_lr1(LRDFA& C, vector<pair<int, vector<int>>>& productionVec, map<int, pair<int, int>>& indexMap, int& boundT, map<int, set<int>>& firstMap);

void quick_construct_lalr(LRDFA& lalr_dfa, vector<pair<int, vector<int>>> productionVec, map<int, pair<int, int>> indexMap, int boundT, map<int, set<int>> firstMap);
