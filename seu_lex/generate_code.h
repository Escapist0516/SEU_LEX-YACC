#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include "structs.h"
using namespace std;

void generate_code(string filename, vector<string>& supDef, DFA final_dfa, vector<Rules> rules, vector<string> functions);