#pragma once
//structs.h
//定义yacc需要使用的数据结构的文件
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <queue>
using namespace std;
/*
   lr1和lalr自动机使用一个dfa来表示
   称之为LR_DFA
*/
//存储LR(1)文法产生式标号以及此时点的位置的结构体
struct LRItem
{
    int Dotpos = 0; //点的位置
    int Label = -1; //产生式标号
    int Symbol;     //预测符，为方便后续操作，产生式和预测符设为一对一关系

    bool operator==(const LRItem& rLRItem) const
    {
        if (Dotpos != rLRItem.Dotpos)
            return false;
        else if (Label != rLRItem.Label)
            return false;
        else if (Symbol != rLRItem.Symbol)
            return false;
        else
            return true;
    }
    bool operator<(const LRItem& rLRItem) const
    {
        return Label < rLRItem.Label || Dotpos < rLRItem.Dotpos || Symbol < rLRItem.Symbol;
    }
};

//存储LR(1)文法状态的结构体
struct DFAState
{
    int Num = -1;           //状态号
    map<int, int> edgesMap; //<终结符，目标状态号>
    set<LRItem> LRItemsSet; //存储文法状态中的产生式
};

//存储LR（1）DFA的结构体
struct LRDFA
{
    int Start = 0;             //起始状态
    vector<DFAState> statesVec; //存储所有的LRState
};