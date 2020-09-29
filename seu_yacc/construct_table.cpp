/*
construct_table.cpp
*/

/*
构造语法分析表
输入：一个增广文法G'
输出：G'的lr语法分析表的函数action和goto
方法：
1）构造G'的lrdfa
2）语法分析器的状态i根据Ii构造得到。状态i的语法分析动作按照下面的规则确定
    1、如果[A->x·aB，b]在Ii中，且goto（Ii，a）=Ij，那么把action[i,a]设置为"移入j"（a是一个终结符号
    2、如果[A->x·,a]在Ii中，并且A!=S'，那么action[i,a]设置为"规约A->x"
    3、如果[S'->S·，$]在Ii中，设置action[i,$]为"接受"
3）状态i相对于各个非终结符的goto函数：如果goto（Ii，A）=Ij，那么goto[i,A]=j
4）其他未设置的项为error
5）初始状态是包含[S'->S·，$]的状态
*/
#include "construct_table.h"

void construct_table(
    int boundN, //(终结符+非终结符个数-1
    int boundT, //(终结符个数-1
    LRDFA dfa,  //(文法状态自动机
    vector<pair<int, vector<int>>> production_vec,
    vector<pair<int, vector<int>>> pre_rules,
    vector<vector<pair<char, int>>> &table//生成的语法表
)
{
    //对于每个dfa里面的状态在语法表里面生成一行
    for (int i = 0; i < dfa.statesVec.size(); i++)
    {
        //当前行
        vector<pair<char, int>> temp_state;
        //然后对于每一种可能的输入，压入对应的应该执行的动作
         //应该有boundN+2列
         //boundN+1: 终结符+非终结符个数，再加一个$，所以是总共是boundN+2,下标为[0--boundN+1]

        //先把所有的空格填满错误
        pair<char, int>pair_error('e',-1);
        for (int j=0;j<=boundN+1;j++)
        {
            temp_state.push_back(pair_error);
        }

        //分别处理移进和归约的情况
        //首先处理移进的情况
        for (auto& action :  dfa.statesVec[i].edgesMap)
        {
            if (action.first <= boundT)
                //如果是终结符，则在$列之前
            {
                pair<char, int>temp_s('s', action.second);
                temp_state[action.first]=temp_s;
            }
            else
                //如果是非终结符，则在$列之后
            {
                pair<char, int>temp_s('s', action.second);
                temp_state[action.first+1] = temp_s;
            }
        }
        //然后处理归约的情况,查看当前状态的每个产生式
        for (auto& temp_item : dfa.statesVec[i].LRItemsSet)
        {

            //如果点在产生式的最后，那么有归约动作，否则不需要
            if (temp_item.Dotpos == production_vec[temp_item.Label].second.size())
            {
                
                //预测符
                int predictive_symbol = temp_item.Symbol;

                if (predictive_symbol == -2)
                {
                    predictive_symbol = boundT + 1; //找到$列

                    //如果是[S'->S·，$]，那么设置action[i,$]为"接受"
                    if (temp_item.Label == 1)
                    {
                        pair<char, int>temp_a('a', temp_item.Label);
                        temp_state[predictive_symbol] = temp_a;
                        //continue;
                    }
                }

                //如果该位置已经有非错误项
                if (temp_state[predictive_symbol].first != 'e')
                {
                    //检查是移入-归约冲突还是归约-归约冲突
                    //如果是移入-归约冲突
                    if (temp_state[predictive_symbol].first == 's')
                    {
                        int last;//产生式中最后一个终结符
                        int predPrior = -1, lastPrior = -1; //predPrior记录预测符的优先级，lastPrior记录每个item最后一个终结符的优先级
                        for (int m = production_vec[temp_item.Label].second.size() - 1; m >= 0; m--)
                        {
                            //从最后开始找，找到该产生式中最后一个终结符
                            if (production_vec[temp_item.Label].second[m] <= boundT)
                            {
                                last = production_vec[temp_item.Label].second[m];
                                break;
                            }
                        }
                        //在优先级规则表中定位最后一个终结符和预测符的优先级顺序
                        for (int p = 0; p < pre_rules.size(); p++)
                        {
                            //每一条都形如：pair<int, vector<string>>
                            //前项表示左结合/右结合，后一项是具体符号集合
                            for (int q = 0; q < pre_rules[p].second.size(); q++)
                            {
                                //找到预测符
                                if (temp_item.Symbol == pre_rules[p].second[q])
                                {
                                    //行数代表优先级，数字越大优先级越高
                                    predPrior = p; //记录预测符优先级
                                }
                                //找到最后一个终结符
                                if (last == pre_rules[p].second[q])
                                {
                                    //行数代表优先级，数字越大优先级越高
                                    lastPrior = p; //记录最后一个终结符的优先级
                                }
                            }
                        }
                        //如果有优先级规则，就按照规则，否则就做移进
                        if (predPrior != -1 && lastPrior != -1)
                        {
                            //最后一个终结符优先级高，则归约，否则移进
                            if (lastPrior >= predPrior)
                            {
                                pair<char, int>temp_r('r', temp_item.Label);
                                temp_state[predictive_symbol] = temp_r;
                            }
                        }
                    }
                    //如果是归约-归约冲突
                    if (temp_state[predictive_symbol].first == 'r')
                    {
                        //选择优先级大的规约项目进行归约
                        //就是序号在前面的产生式归约
                        if (temp_state[predictive_symbol].second > temp_item.Label)
                        {
                            temp_state[predictive_symbol].second = temp_item.Label;
                        }
                    }
                }
                //无冲突，直接填入即可
                else
                {
                    pair<char, int>temp_r('r', temp_item.Label);
                    temp_state[predictive_symbol] = temp_r;
                }
            }
        }
        //完成 压入栈中
        table.push_back(temp_state);
    }
}