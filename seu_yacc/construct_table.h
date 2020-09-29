#pragma once
#include "structs.h"

void construct_table(
    int boundN,
    int boundT,
    LRDFA dfa,
    vector<pair<int, vector<int>>> production_vec,
    vector<pair<int, vector<int>>> pre_rules,
    vector<vector<pair<char, int>>>& table//生成的语法表
);