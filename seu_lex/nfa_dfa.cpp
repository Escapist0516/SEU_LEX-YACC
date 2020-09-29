/*
   nfa_dfa.cpp
*/
#include "nfa_dfa.h"
#include <stack>
#include <queue>
#include <iostream>


//求某个nfa状态集的e-closure闭包
void e_closure(set<int>& initStatesSet,NFA nfa)
{
    stack<int> stack;
    vector<bool> handledFlagVec(nfa.statesMap.size());//标记状态是否被处理过

    //将初始集合中所有状态入栈
    for (const auto& s : initStatesSet)
    {
        stack.push(s);
        handledFlagVec[s] = true; //true表示该状态已经处理过
    }

    while (!stack.empty())
    {
        //找到栈顶状态通过@边可以到达的状态
        auto itsPair = nfa.statesMap.find(stack.top())->second.EdgeMultiMap.equal_range('@');
        stack.pop();
        auto beginIt = itsPair.first, endIt = itsPair.second;
        while (beginIt != endIt)
        {
            int unhandledItem = (*beginIt).second;
            //如果已经处理过该状态则跳过
            if (handledFlagVec[unhandledItem])
            {
                continue;
            }
            //否则压入栈并将其插入状态集合中
            else
            {
                stack.push(unhandledItem);
                initStatesSet.emplace(unhandledItem);
                ++beginIt;
            }
        }
    }
}

//通过子集构造法将nfa转换成dfa
void nfa_to_dfa(NFA nfa, DFA& dfa)
{
    string edgeSet(ALLSET);
    queue<int> un_possessed_dfa_num;//用来保存未标记的dfa状态号
    DFAstate dstate;
    int dfa_num = 0;//dfa状态的号码

    dstate.num = dfa_num;
    dfa_num++;
    dstate.subSet.insert(nfa.startState);
    //cout << dstate.num << endl;
    //初始化dstate，此时的dstate里只含有nfa的开始状态
    e_closure(dstate.subSet,nfa);
    //求e-closure（s0），此时dstate包括e-closure（s0）

    un_possessed_dfa_num.push(dstate.num);
    dfa.statesVec.push_back(dstate);

    while (!un_possessed_dfa_num.empty())
    //当dfa状态中有未标记状态时
    {
        int temp_dfa_num = un_possessed_dfa_num.front();
        //取一个未标记的dfa状态,(取列中最后一个未标记状态)
        un_possessed_dfa_num.pop();
        //给它加上标记,（从未标记状态列中删除它）

        for (const char& a : edgeSet)
        //对于每个输入a
        {
            set<int> tempSet;//新建一个set来存储 move[temp_dfa,c]
            //注：move(T,a):能够从当前dfa状态中某个NFA状态s开始通过标号a转换到达的NFA状态的集合

            //如果当前dfa状态对应的nfa状态集的对于输入字符c没有边到达其他状态
            bool flag = false;
            //cout << temp_dfa_num << endl;

            for (const auto& state : dfa.statesVec[temp_dfa_num].subSet)
            {
                //从NFA状态集合中找到所有对应edge字符的边
                auto itsPair = nfa.statesMap.find(state)->second.EdgeMultiMap.equal_range(a);
                auto beginIt = itsPair.first, endIt = itsPair.second;
                //如果该状态没有发出edge边则跳过
                if (beginIt == endIt)
                {
                    continue;
                }
                //将边指向的状态编号插入到子集中
                while (beginIt != endIt)
                {
                    tempSet.insert(beginIt->second);
                    ++beginIt;
                }
                flag = true;
            }
            if(flag)
            {
                //对于dfa状态U=e-closure(move(temp_dfa,a))

                int toStateNum;
                //新状态的dfa状态编号
                e_closure(tempSet, nfa);
                //对move[temp,c]进行epsilon闭包,此时tempSet为e-closure（move（T，a））
                bool is_existed = false;
                //标记该子集是否已经存在

                for (const auto& s : dfa.statesVec)
                {
                    if (s.subSet == tempSet)
                    {
                        //该子集已经存在
                        toStateNum = s.num; //指向vec中已有的状态
                        is_existed = true;
                        break;
                    }
                }
                //如果U在前面的循环里没有生成过，那么新建这个dfa状态并加入到dfa中，并加入到未标记列表里
                if (!is_existed)
                {
                    DFAstate newState;
                    newState.num = dfa_num++;  //新标号
                    toStateNum = newState.num; //指向新建的这个状态
                    newState.subSet = tempSet;
                    dfa.statesVec.push_back(newState); //新建状态入状态子集vec
                    un_possessed_dfa_num.push(toStateNum);	//新状态压入未处理状态栈

                    //如果该dfa状态包含一个nfa终态，意味着这是一个dfa终态，那么找出这个dfa终态对应的动作，并把它添加到dfa的终态表中
                    int actions;
                    bool flag = false;

                    decltype(nfa.endStatesMap.find(0)) endStateIt; //要选择的终态
                    bool find = false;
                    //遍历DFA中所有的NFA状态
                    for (const auto& nfaState : newState.subSet)
                    {
                        //判断该NFA状态是不是终态
                        auto it = nfa.endStatesMap.find(nfaState);
                        //是终态的话
                        if (it != nfa.endStatesMap.end())
                        {
                            //如果该终态对应的动作在.l文件中更靠前则该DFA状态对应的动作更新为该动作
                            if (find)
                            {
                                if (endStateIt->first > it->first)
                                    endStateIt = it;
                            }
                            else
                            {
                                endStateIt = it;
                                find = true;
                            }
                        }
                    }
                   
                    if (find)
                    {
                        actions = endStateIt->second;//当前动作为要找的终态的动作
                        flag = true;
                    }
                    else
                        flag = false;
                    if (flag)
                    {
                        //把这个dfa终态对应的动作添加到dfa的终态表中
                        dfa.endStatesMap.insert(pair<int, int>(newState.num, actions));
                    }
                }
                //构造DFA的一条边(一条从temp_dfa到U的值为a的边)
                dfa.statesVec[temp_dfa_num].EdgeMap.insert(pair<char, int>(a, toStateNum));
                //*************************************************
            }
        }
        cout << "nfa_to_dfa: " << un_possessed_dfa_num.size() << endl;
    }
}

//dfa最小化
void dfa_mini(DFA dfa, DFA& final_dfa)
{
    int set_count = 0;//分划中的集合数

    //构造划分T
    vector<set<int> > statesSetsVec;//存储DFA最小化过程中的的状态集合（划分T
    map<int, int> statesSetsMap;//存储miniDFA的状态号和集合号的对应关系

    //首先构造初始划分，遍历dfa中的所有状态
    for (const auto& p : dfa.endStatesMap)
    {
        statesSetsVec.push_back(set<int>{p.first});//将每个终态单独存入一个集合中，并加入划分T里
        statesSetsMap.emplace(p.first, set_count++);//添加对应关系
    }
    //所有的非终态存在同一个集合中 ....
    set<int> tempSet;
    for (const auto& e : dfa.statesVec)
    {
        if (dfa.endStatesMap.find(e.num) == dfa.endStatesMap.end())
        {
            tempSet.insert(e.num);
            statesSetsMap.emplace(e.num, set_count);//添加对应关系
        }
    }
    statesSetsVec.push_back(tempSet);
    //加入到划分T中
    ++set_count;

    //接下来对划分进行不断的分割直到不能再分割为止
    //每次循环只把一个集合分割成两个，即每次最多只往划分中添加一个新集合
    bool flag = false;//是否有集合被拆开
    while (flag)
    {
        int splitSetNumber = 0;	//被拆分的集合标号
        set<int> newSet; //被拆分出的新集合

        //对于划分中的每个集合
        for (int k = 0; k < statesSetsVec.size(); ++k)
        {
            auto& s = statesSetsVec[k];//当前集合为s
            if (s.size() == 1)
                continue; //如果集合中只有一个状态则跳过（如果是终态集合，跳过
            else
            {
                auto& standard = dfa.statesVec[*(s.begin())]; //将其中一个state作为基准
                //遍历基准状态的所有边
                for (const auto& c : ALLSET)
                {
                    //检查集合中的每一个状态
                    for (const auto& i : s)
                    {
                        const auto& state = dfa.statesVec[i];
                        if (state.num == standard.num)
                            continue;									//避免“自己和自己比较”
                        auto findStateIt = state.EdgeMap.find(c);		//在state中找c边
                        auto findStandardIt = standard.EdgeMap.find(c); //在基准state中找c边
                        //不等价条件1:其中一个状态无c边而另一个有c边
                        if (findStateIt == state.EdgeMap.end() && findStandardIt != standard.EdgeMap.end())
                        {
                            flag = true;
                            newSet.insert(i);
                        }
                        else if ((findStateIt != state.EdgeMap.end()) && (findStandardIt == standard.EdgeMap.end()))
                        {
                            flag = true;
                            newSet.insert(i);
                        }
                        //不等价条件2:都有c边，但目的状态不同
                        else if ((findStateIt != state.EdgeMap.end()) && (findStandardIt != standard.EdgeMap.end()))
                        {
                            if (*(statesSetsMap.find(findStandardIt->second)) != *(statesSetsMap.find(findStateIt->second)))
                            {
                                flag = true;
                                newSet.insert(i);
                            }
                        }
                    }
                    if (flag)
                        break; //每次只分成两个集合
                }
                if (flag)
                {
                    splitSetNumber = k;
                    break;
                }
            }
        }

        //删除被拆分集合中的被拆分状态
        //对于被拆分出来的新集合中的每个状态
        for (const auto& s : newSet)
        {
            statesSetsVec[splitSetNumber].erase(s);//在被拆分的集合中删除这个状态
            statesSetsMap.insert_or_assign(s, set_count);//加入新的对应关系
        }
        //将新集合压入划分T中
        if (newSet.size() != 0)
            statesSetsVec.push_back(newSet);
        //划分中的集合数加一
        ++set_count;

    }
    //划分完成

    //初态标号=tmp_dfa初态所在集合标号
    final_dfa.startState = statesSetsMap.find(dfa.startState)->second;

    //终态
    for (const auto& p : dfa.endStatesMap)
    {
        final_dfa.endStatesMap.emplace(statesSetsMap.find(p.first)->second, p.second);
    }

   /* for (int k = 0; k < statesSetsVec.size(); ++k)
    {
        DFAstate newState;
        newState.num = k;
        set<int>::const_iterator cit;
        for (cit = statesSetsVec[k].cbegin(); cit != statesSetsVec[k].cend(); ++cit)
        {
             auto& pivotState = dfa.statesVec[*cit];
             for (const auto& p : pivotState.EdgeMap)
             {
                 newState.EdgeMap.emplace(p.first, statesSetsMap.find(p.second)->second);
             }
        }
        final_dfa.statesVec.push_back(newState);
    }*/

    for (int k = 0; k < statesSetsVec.size(); ++k)
    {
        auto& pivotState = dfa.statesVec[*statesSetsVec[k].cbegin()];
        DFAstate newState;
        newState.num = k;
        for (const auto& p : pivotState.EdgeMap)
        {
            newState.EdgeMap.emplace(p.first, statesSetsMap.find(p.second)->second);
        }
        final_dfa.statesVec.push_back(newState);
    }
    //dfa最小化完成
    return;
}

/*  some useful comments:

   pan jia xin
   start time:2020/05/20
   finish time:2020/05/21

   ***************** nfa to dfa 算法 ******************************************
   *子集构造法：
   * 输入：一个NFA nfa(s0是nfa的开始状态)
   * 输出：一个DFA dfa
   * 开始状态：e-closure(s0)
   * 接收状态：所有至少包含了nfa的一个接收状态的状态集
   * 方法：
   * 一开始Dstate中只有e-closure(s0)，且它未标记
   * while(Dstate中有未标记状态)
   * {
   *    给T加上标记；
   *    for（每个输入符号a）
   *    {
   *      U=e-closure（move（T，a））；
   *      if（U不在Dstate中）
   *      {
   *         将U加入Dstate中，且不标记
   *      }
   *      Dtran[T,a]=U;//Dtran[T,a]是Dstate和dfa的转换函数（相当于构造一条边）
   *    }
   * }
   *********************注******************************************************
   NFA状态集上的操作：
   e-closure(s):能够从NFA中某个状态s开始只通过e转换到的NFA的状态集合
   e-closure(T):能够从T中某个NFA状态s开始只通过e到达的NFA状态集合
   move(T,a):能够从T中某个NFA状态s开始通过标号a转换到达的NFA状态的集合

   ********************* dfa最小化算法 *****************************************
   *原理：将当前dfa的状态集合分划成多个组，每个组的各个状态之间相互不可区分，
   *      然后把每个组中的状态合并成同一个状态
   * 方法：
   *       把dfa划分成接收状态和分接受状态两个组;
   *          //（dfa的状态集合为S，开始状态s0，接受状态集为F
   *          //(即初始划分T包括： 接受状态组为F,非接收状态组为S-F
   *       while(Tnew!=T)
   *       {
   *          Tnew=T;
   *          for(T中的每个组G)
   *          {
   *             将G划分成更小的组，使得两个状态s和t在
   *             同一小组中当且仅当对所有的输入符号a，
   *             状态s和t在a上的转换都到达T中的同一个组；
   *             在Tnew中把G替换成由G分划得到的小组
   *          }
   *       }
   *       Tfinal=T；
   *       Tfinal的每个组中选择一个状态作为该组的代表，构成minDFA的状态集合；
   *       minDFA的开始状态是那些包含了dfa的开始状态组的代表；
   *       minDFS的接收状态是包含了dfa接收状态的组的代表；
   *       然后构造minDFA的边；
   *       //（令s是Tfinal中某个组的代表，并令dfa中在输入a上离开s的转换到达t，
   *           令r为t所在组H的代表。那么在minDFA上一定存在一个从s到r在输入a上
   *           的转换。
   *       消除死状态；
   *       完成；
   ************************************注******************************************
   1、实际上在词法分析器中，上面算法的初始分划应该把识别某个特定的此法单元的所有
      状态放到对应此词法单元的一个组中，同时把不识别任何词法单元的状态放到另一组中
 */