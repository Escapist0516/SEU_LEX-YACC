#pragma once
//structs.h
//����yacc��Ҫʹ�õ����ݽṹ���ļ�
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <queue>
using namespace std;
/*
   lr1��lalr�Զ���ʹ��һ��dfa����ʾ
   ��֮ΪLR_DFA
*/
//�洢LR(1)�ķ�����ʽ����Լ���ʱ���λ�õĽṹ��
struct LRItem
{
    int Dotpos = 0; //���λ��
    int Label = -1; //����ʽ���
    int Symbol;     //Ԥ�����Ϊ�����������������ʽ��Ԥ�����Ϊһ��һ��ϵ

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

//�洢LR(1)�ķ�״̬�Ľṹ��
struct DFAState
{
    int Num = -1;           //״̬��
    map<int, int> edgesMap; //<�ս����Ŀ��״̬��>
    set<LRItem> LRItemsSet; //�洢�ķ�״̬�еĲ���ʽ
};

//�洢LR��1��DFA�Ľṹ��
struct LRDFA
{
    int Start = 0;             //��ʼ״̬
    vector<DFAState> statesVec; //�洢���е�LRState
};