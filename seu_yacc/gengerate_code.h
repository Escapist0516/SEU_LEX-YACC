#pragma once
#include"structs.h"

void generate_code(string filename,vector<string>& functions, vector<vector<pair<char, int>>>& table, map<string, int>tokensMap, vector<pair<int, vector<int>>>& productionVec,int boundT);