/*
construct_table.cpp
*/

/*
�����﷨������
���룺һ�������ķ�G'
�����G'��lr�﷨������ĺ���action��goto
������
1������G'��lrdfa
2���﷨��������״̬i����Ii����õ���״̬i���﷨����������������Ĺ���ȷ��
    1�����[A->x��aB��b]��Ii�У���goto��Ii��a��=Ij����ô��action[i,a]����Ϊ"����j"��a��һ���ս����
    2�����[A->x��,a]��Ii�У�����A!=S'����ôaction[i,a]����Ϊ"��ԼA->x"
    3�����[S'->S����$]��Ii�У�����action[i,$]Ϊ"����"
3��״̬i����ڸ������ս����goto���������goto��Ii��A��=Ij����ôgoto[i,A]=j
4������δ���õ���Ϊerror
5����ʼ״̬�ǰ���[S'->S����$]��״̬
*/
#include "construct_table.h"

void construct_table(
    int boundN, //(�ս��+���ս������-1
    int boundT, //(�ս������-1
    LRDFA dfa,  //(�ķ�״̬�Զ���
    vector<pair<int, vector<int>>> production_vec,
    vector<pair<int, vector<int>>> pre_rules,
    vector<vector<pair<char, int>>> &table//���ɵ��﷨��
)
{
    //����ÿ��dfa�����״̬���﷨����������һ��
    for (int i = 0; i < dfa.statesVec.size(); i++)
    {
        //��ǰ��
        vector<pair<char, int>> temp_state;
        //Ȼ�����ÿһ�ֿ��ܵ����룬ѹ���Ӧ��Ӧ��ִ�еĶ���
         //Ӧ����boundN+2��
         //boundN+1: �ս��+���ս���������ټ�һ��$���������ܹ���boundN+2,�±�Ϊ[0--boundN+1]

        //�Ȱ����еĿո���������
        pair<char, int>pair_error('e',-1);
        for (int j=0;j<=boundN+1;j++)
        {
            temp_state.push_back(pair_error);
        }

        //�ֱ����ƽ��͹�Լ�����
        //���ȴ����ƽ������
        for (auto& action :  dfa.statesVec[i].edgesMap)
        {
            if (action.first <= boundT)
                //������ս��������$��֮ǰ
            {
                pair<char, int>temp_s('s', action.second);
                temp_state[action.first]=temp_s;
            }
            else
                //����Ƿ��ս��������$��֮��
            {
                pair<char, int>temp_s('s', action.second);
                temp_state[action.first+1] = temp_s;
            }
        }
        //Ȼ�����Լ�����,�鿴��ǰ״̬��ÿ������ʽ
        for (auto& temp_item : dfa.statesVec[i].LRItemsSet)
        {

            //������ڲ���ʽ�������ô�й�Լ������������Ҫ
            if (temp_item.Dotpos == production_vec[temp_item.Label].second.size())
            {
                
                //Ԥ���
                int predictive_symbol = temp_item.Symbol;

                if (predictive_symbol == -2)
                {
                    predictive_symbol = boundT + 1; //�ҵ�$��

                    //�����[S'->S����$]����ô����action[i,$]Ϊ"����"
                    if (temp_item.Label == 1)
                    {
                        pair<char, int>temp_a('a', temp_item.Label);
                        temp_state[predictive_symbol] = temp_a;
                        //continue;
                    }
                }

                //�����λ���Ѿ��зǴ�����
                if (temp_state[predictive_symbol].first != 'e')
                {
                    //���������-��Լ��ͻ���ǹ�Լ-��Լ��ͻ
                    //���������-��Լ��ͻ
                    if (temp_state[predictive_symbol].first == 's')
                    {
                        int last;//����ʽ�����һ���ս��
                        int predPrior = -1, lastPrior = -1; //predPrior��¼Ԥ��������ȼ���lastPrior��¼ÿ��item���һ���ս�������ȼ�
                        for (int m = production_vec[temp_item.Label].second.size() - 1; m >= 0; m--)
                        {
                            //�����ʼ�ң��ҵ��ò���ʽ�����һ���ս��
                            if (production_vec[temp_item.Label].second[m] <= boundT)
                            {
                                last = production_vec[temp_item.Label].second[m];
                                break;
                            }
                        }
                        //�����ȼ�������ж�λ���һ���ս����Ԥ��������ȼ�˳��
                        for (int p = 0; p < pre_rules.size(); p++)
                        {
                            //ÿһ�������磺pair<int, vector<string>>
                            //ǰ���ʾ����/�ҽ�ϣ���һ���Ǿ�����ż���
                            for (int q = 0; q < pre_rules[p].second.size(); q++)
                            {
                                //�ҵ�Ԥ���
                                if (temp_item.Symbol == pre_rules[p].second[q])
                                {
                                    //�����������ȼ�������Խ�����ȼ�Խ��
                                    predPrior = p; //��¼Ԥ������ȼ�
                                }
                                //�ҵ����һ���ս��
                                if (last == pre_rules[p].second[q])
                                {
                                    //�����������ȼ�������Խ�����ȼ�Խ��
                                    lastPrior = p; //��¼���һ���ս�������ȼ�
                                }
                            }
                        }
                        //��������ȼ����򣬾Ͱ��չ��򣬷�������ƽ�
                        if (predPrior != -1 && lastPrior != -1)
                        {
                            //���һ���ս�����ȼ��ߣ����Լ�������ƽ�
                            if (lastPrior >= predPrior)
                            {
                                pair<char, int>temp_r('r', temp_item.Label);
                                temp_state[predictive_symbol] = temp_r;
                            }
                        }
                    }
                    //����ǹ�Լ-��Լ��ͻ
                    if (temp_state[predictive_symbol].first == 'r')
                    {
                        //ѡ�����ȼ���Ĺ�Լ��Ŀ���й�Լ
                        //���������ǰ��Ĳ���ʽ��Լ
                        if (temp_state[predictive_symbol].second > temp_item.Label)
                        {
                            temp_state[predictive_symbol].second = temp_item.Label;
                        }
                    }
                }
                //�޳�ͻ��ֱ�����뼴��
                else
                {
                    pair<char, int>temp_r('r', temp_item.Label);
                    temp_state[predictive_symbol] = temp_r;
                }
            }
        }
        //��� ѹ��ջ��
        table.push_back(temp_state);
    }
}