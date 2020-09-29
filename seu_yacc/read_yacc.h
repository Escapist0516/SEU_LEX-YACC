#pragma once
#include "structs.h"

void read_yacc(string filename,vector<string>& tokensVec,vector<pair<int,vector<string>>>& tempRules, 
	vector<pair<string,vector<string>>>& productionStrVec,vector<string> &functions);