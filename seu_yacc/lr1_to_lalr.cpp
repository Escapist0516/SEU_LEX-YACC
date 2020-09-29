/*
LR1_TO_LALR.CPP
*/
/*
 lr1 dfa ת��Ϊ lalr dfa
 ������ �����о���ͬ���ĵ�״̬�滻Ϊ���ǵĲ���
*/
#include "lr1_to_lalr.h"

void lr1_to_lalr(LRDFA lr1_dfa, LRDFA &lalr_dfa) 
{
    //���� �������еļ��ϵ�vector
    //pair�� ǰ�� ��ʾ���ģ����� ��ʾ������Ŀ��ܰ�����lr1dfa��״̬��
    //set<pair<int,int>>��� ǰ�� һ�� int ��ʾ����ʽ�ţ����� һ�� int ��ʾ���λ��
    vector<pair<set<pair<int, int>>, set<int>>> core_vector;


    //��ÿ��lr1״̬����ģ�Ȼ�󽫺�����ͬ��״̬ƥ�䵽һ��
    for (auto& state : lr1_dfa.statesVec)
    {
        set<pair<int, int>> temp_Core;//��ǰ״̬�ĺ���
        //���쵱ǰ״̬�ĺ���
        for (auto& temp : state.LRItemsSet)
        {
            temp_Core.insert(make_pair(temp.Label, temp.Dotpos));
        }
        //Ȼ�󿴿���ǰ�����Ƿ��Ѿ�����
        bool exist = false; 
        for (auto& core : core_vector)
        {
            //��ǰ״̬��core�Ѵ�����coreVec�У��򽫵�ǰ״̬�ı�ż���ͬ�ļ�
            if (core.first == temp_Core)
            {
                //������ڵĻ�����ô�Ͱѵ�ǰ״̬�ż��뵽�Ѵ��ں��ĵĺ�����
                exist = true;
                core.second.insert(state.Num);
            }
        }
        //��ǰ���Ĳ�����
        if (exist==false)
        {
            //���뵽���ļ�����
            set<int> new_state_num;
            new_state_num.insert(state.Num);
            core_vector.push_back(make_pair(temp_Core, new_state_num));
        }
    }
    //���ļ��Ϲ������
    //���� ���Կ�ʼ ����lalr dfa��
    int count = 0;//lalr dfa��״̬��

    map<int, int> state_map; //��lr1 dfa ״̬�� ��lalr dfa ״̬�� ��ӳ��
    //����lalrdfa��״̬����
    //����ÿ������
    for (auto& core : core_vector)
    {
        DFAState new_state;
        new_state.Num = count;//״̬��
        count++;
        //���� ͬ�ļ� ״̬�Ķ������� ��ǰcore��ͬ�ļ���� ��һ��״̬ �Ķ��� ��Ϊ��״̬ �Ķ���
        new_state.edgesMap = lr1_dfa.statesVec[*core.second.begin()].edgesMap;
        //���ڵ�ǰ���ĵ�lr1״̬��������ÿ��lr1״̬
        for (auto& state : core.second)
        {
            //���ڵ�ǰlr1״̬��ÿ������ʽ��lritem
            for (auto& lritem : lr1_dfa.statesVec[state].LRItemsSet)
            {
                //
                new_state.LRItemsSet.insert(lritem);
            }
            //��lr1 dfa״̬�ź� lalr dfa ״̬�� �Ķ�Ӧ��ϵ ��ӵ� ӳ�����
            state_map.emplace(state, new_state.Num);
        }
        lalr_dfa.statesVec.push_back(new_state);//����״̬ѹ��lalrdfa��
    }
    //д�� lalr dfa �� ��ʼ״̬
    lalr_dfa.Start = state_map.find(lr1_dfa.Start)->second;


    //Ȼ�� ���¹��� lalr dfa�� �ߵ� ָ��
    //lalr dfa�е�ÿ�� ״̬
    for (auto& state : lalr_dfa.statesVec) {
        //ÿ�� ״̬��ÿ�� ����
        for (auto& edge : state.edgesMap) {
            //��indexMap���ҵ� �ɵ�״̬��Ȼ����³��µ�ͬ�ļ�״̬
            edge.second = state_map.find(edge.second)->second;
        }
    }
    //��ɣ��õ�һ��lalr dfa
}