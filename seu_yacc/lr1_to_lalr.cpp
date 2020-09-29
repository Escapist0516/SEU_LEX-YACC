/*
LR1_TO_LALR.CPP
*/
/*
 lr1 dfa 转换为 lalr dfa
 方法： 把所有具有同核心的状态替换为他们的并集
*/
#include "lr1_to_lalr.h"

void lr1_to_lalr(LRDFA lr1_dfa, LRDFA &lalr_dfa) 
{
    //用来 保存所有的集合的vector
    //pair的 前项 表示核心，后项 表示这个核心可能包含的lr1dfa的状态号
    //set<pair<int,int>>里的 前面 一个 int 表示产生式号，后面 一个 int 表示点的位置
    vector<pair<set<pair<int, int>>, set<int>>> core_vector;


    //对每个lr1状态求核心，然后将核心相同的状态匹配到一起
    for (auto& state : lr1_dfa.statesVec)
    {
        set<pair<int, int>> temp_Core;//当前状态的核心
        //构造当前状态的核心
        for (auto& temp : state.LRItemsSet)
        {
            temp_Core.insert(make_pair(temp.Label, temp.Dotpos));
        }
        //然后看看当前核心是否已经存在
        bool exist = false; 
        for (auto& core : core_vector)
        {
            //当前状态的core已存在于coreVec中，则将当前状态的标号加入同心集
            if (core.first == temp_Core)
            {
                //如果存在的话，那么就把当前状态号加入到已存在核心的后项中
                exist = true;
                core.second.insert(state.Num);
            }
        }
        //当前核心不存在
        if (exist==false)
        {
            //加入到核心集合中
            set<int> new_state_num;
            new_state_num.insert(state.Num);
            core_vector.push_back(make_pair(temp_Core, new_state_num));
        }
    }
    //核心集合构造完成
    //下面 可以开始 构造lalr dfa了
    int count = 0;//lalr dfa的状态号

    map<int, int> state_map; //从lr1 dfa 状态号 到lalr dfa 状态号 的映射
    //建立lalrdfa的状态集合
    //对于每个核心
    for (auto& core : core_vector)
    {
        DFAState new_state;
        new_state.Num = count;//状态号
        count++;
        //更新 同心集 状态的动作，将 当前core的同心集里的 第一个状态 的动作 作为新状态 的动作
        new_state.edgesMap = lr1_dfa.statesVec[*core.second.begin()].edgesMap;
        //对于当前核心的lr1状态集合里面每个lr1状态
        for (auto& state : core.second)
        {
            //对于当前lr1状态的每个产生式（lritem
            for (auto& lritem : lr1_dfa.statesVec[state].LRItemsSet)
            {
                //
                new_state.LRItemsSet.insert(lritem);
            }
            //把lr1 dfa状态号和 lalr dfa 状态号 的对应关系 添加到 映射表里
            state_map.emplace(state, new_state.Num);
        }
        lalr_dfa.statesVec.push_back(new_state);//将新状态压入lalrdfa中
    }
    //写入 lalr dfa 的 初始状态
    lalr_dfa.Start = state_map.find(lr1_dfa.Start)->second;


    //然后 重新构造 lalr dfa的 边的 指向
    //lalr dfa中的每个 状态
    for (auto& state : lalr_dfa.statesVec) {
        //每个 状态的每条 出边
        for (auto& edge : state.edgesMap) {
            //在indexMap里找到 旧的状态，然后更新成新的同心集状态
            edge.second = state_map.find(edge.second)->second;
        }
    }
    //完成，得到一个lalr dfa
}