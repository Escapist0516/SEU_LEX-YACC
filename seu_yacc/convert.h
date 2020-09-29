#pragma once
#ifndef CONVERT_H
#define CONVERT_H
#include"structs.h"
void convert(vector<pair<string, vector<string>>>& productionStrVec, vector<pair<int, vector<int>>>& productionVec, map<string, int>& tokensMap, vector<string>& tokensVec,
	int& boundT, int& boundN, vector<pair<int, vector<string>>>& precedenceRulesStrVec, vector<pair<int, vector<int>>>& precedenceRulesVec, map<int, pair<int, int>>& indexMap);
//将所有的token转换成正整数，char+tokens：0-boundT，非终结符：boundT+1-boundN
#endif
