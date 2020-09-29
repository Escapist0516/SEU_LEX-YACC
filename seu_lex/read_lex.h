#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include "structs.h"
using namespace std;

vector<string> split(const string& aim,const string& s);
void clean(string& aim);
void read_lex_from_file(string filename,
		vector<string>& supDef,map<string,string>& head,
		vector<Rules>& rules,vector<string>& functions);