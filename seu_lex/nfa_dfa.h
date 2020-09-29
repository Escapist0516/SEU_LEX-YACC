#pragma once
#include "structs.h"
using namespace std;

void nfa_to_dfa(NFA nfa,DFA& dfa);

void dfa_mini(DFA dfa,DFA& final_dfa);