/*
   nfa_dfa.cpp
*/
#include "nfa_dfa.h"
#include <stack>
#include <queue>
#include <iostream>


//��ĳ��nfa״̬����e-closure�հ�
void e_closure(set<int>& initStatesSet,NFA nfa)
{
    stack<int> stack;
    vector<bool> handledFlagVec(nfa.statesMap.size());//���״̬�Ƿ񱻴����

    //����ʼ����������״̬��ջ
    for (const auto& s : initStatesSet)
    {
        stack.push(s);
        handledFlagVec[s] = true; //true��ʾ��״̬�Ѿ������
    }

    while (!stack.empty())
    {
        //�ҵ�ջ��״̬ͨ��@�߿��Ե����״̬
        auto itsPair = nfa.statesMap.find(stack.top())->second.EdgeMultiMap.equal_range('@');
        stack.pop();
        auto beginIt = itsPair.first, endIt = itsPair.second;
        while (beginIt != endIt)
        {
            int unhandledItem = (*beginIt).second;
            //����Ѿ��������״̬������
            if (handledFlagVec[unhandledItem])
            {
                continue;
            }
            //����ѹ��ջ���������״̬������
            else
            {
                stack.push(unhandledItem);
                initStatesSet.emplace(unhandledItem);
                ++beginIt;
            }
        }
    }
}

//ͨ���Ӽ����취��nfaת����dfa
void nfa_to_dfa(NFA nfa, DFA& dfa)
{
    string edgeSet(ALLSET);
    queue<int> un_possessed_dfa_num;//��������δ��ǵ�dfa״̬��
    DFAstate dstate;
    int dfa_num = 0;//dfa״̬�ĺ���

    dstate.num = dfa_num;
    dfa_num++;
    dstate.subSet.insert(nfa.startState);
    //cout << dstate.num << endl;
    //��ʼ��dstate����ʱ��dstate��ֻ����nfa�Ŀ�ʼ״̬
    e_closure(dstate.subSet,nfa);
    //��e-closure��s0������ʱdstate����e-closure��s0��

    un_possessed_dfa_num.push(dstate.num);
    dfa.statesVec.push_back(dstate);

    while (!un_possessed_dfa_num.empty())
    //��dfa״̬����δ���״̬ʱ
    {
        int temp_dfa_num = un_possessed_dfa_num.front();
        //ȡһ��δ��ǵ�dfa״̬,(ȡ�������һ��δ���״̬)
        un_possessed_dfa_num.pop();
        //�������ϱ��,����δ���״̬����ɾ������

        for (const char& a : edgeSet)
        //����ÿ������a
        {
            set<int> tempSet;//�½�һ��set���洢 move[temp_dfa,c]
            //ע��move(T,a):�ܹ��ӵ�ǰdfa״̬��ĳ��NFA״̬s��ʼͨ�����aת�������NFA״̬�ļ���

            //�����ǰdfa״̬��Ӧ��nfa״̬���Ķ��������ַ�cû�бߵ�������״̬
            bool flag = false;
            //cout << temp_dfa_num << endl;

            for (const auto& state : dfa.statesVec[temp_dfa_num].subSet)
            {
                //��NFA״̬�������ҵ����ж�Ӧedge�ַ��ı�
                auto itsPair = nfa.statesMap.find(state)->second.EdgeMultiMap.equal_range(a);
                auto beginIt = itsPair.first, endIt = itsPair.second;
                //�����״̬û�з���edge��������
                if (beginIt == endIt)
                {
                    continue;
                }
                //����ָ���״̬��Ų��뵽�Ӽ���
                while (beginIt != endIt)
                {
                    tempSet.insert(beginIt->second);
                    ++beginIt;
                }
                flag = true;
            }
            if(flag)
            {
                //����dfa״̬U=e-closure(move(temp_dfa,a))

                int toStateNum;
                //��״̬��dfa״̬���
                e_closure(tempSet, nfa);
                //��move[temp,c]����epsilon�հ�,��ʱtempSetΪe-closure��move��T��a����
                bool is_existed = false;
                //��Ǹ��Ӽ��Ƿ��Ѿ�����

                for (const auto& s : dfa.statesVec)
                {
                    if (s.subSet == tempSet)
                    {
                        //���Ӽ��Ѿ�����
                        toStateNum = s.num; //ָ��vec�����е�״̬
                        is_existed = true;
                        break;
                    }
                }
                //���U��ǰ���ѭ����û�����ɹ�����ô�½����dfa״̬�����뵽dfa�У������뵽δ����б���
                if (!is_existed)
                {
                    DFAstate newState;
                    newState.num = dfa_num++;  //�±��
                    toStateNum = newState.num; //ָ���½������״̬
                    newState.subSet = tempSet;
                    dfa.statesVec.push_back(newState); //�½�״̬��״̬�Ӽ�vec
                    un_possessed_dfa_num.push(toStateNum);	//��״̬ѹ��δ����״̬ջ

                    //�����dfa״̬����һ��nfa��̬����ζ������һ��dfa��̬����ô�ҳ����dfa��̬��Ӧ�Ķ�������������ӵ�dfa����̬����
                    int actions;
                    bool flag = false;

                    decltype(nfa.endStatesMap.find(0)) endStateIt; //Ҫѡ�����̬
                    bool find = false;
                    //����DFA�����е�NFA״̬
                    for (const auto& nfaState : newState.subSet)
                    {
                        //�жϸ�NFA״̬�ǲ�����̬
                        auto it = nfa.endStatesMap.find(nfaState);
                        //����̬�Ļ�
                        if (it != nfa.endStatesMap.end())
                        {
                            //�������̬��Ӧ�Ķ�����.l�ļ��и���ǰ���DFA״̬��Ӧ�Ķ�������Ϊ�ö���
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
                        actions = endStateIt->second;//��ǰ����ΪҪ�ҵ���̬�Ķ���
                        flag = true;
                    }
                    else
                        flag = false;
                    if (flag)
                    {
                        //�����dfa��̬��Ӧ�Ķ�����ӵ�dfa����̬����
                        dfa.endStatesMap.insert(pair<int, int>(newState.num, actions));
                    }
                }
                //����DFA��һ����(һ����temp_dfa��U��ֵΪa�ı�)
                dfa.statesVec[temp_dfa_num].EdgeMap.insert(pair<char, int>(a, toStateNum));
                //*************************************************
            }
        }
        cout << "nfa_to_dfa: " << un_possessed_dfa_num.size() << endl;
    }
}

//dfa��С��
void dfa_mini(DFA dfa, DFA& final_dfa)
{
    int set_count = 0;//�ֻ��еļ�����

    //���컮��T
    vector<set<int> > statesSetsVec;//�洢DFA��С�������еĵ�״̬���ϣ�����T
    map<int, int> statesSetsMap;//�洢miniDFA��״̬�źͼ��ϺŵĶ�Ӧ��ϵ

    //���ȹ����ʼ���֣�����dfa�е�����״̬
    for (const auto& p : dfa.endStatesMap)
    {
        statesSetsVec.push_back(set<int>{p.first});//��ÿ����̬��������һ�������У������뻮��T��
        statesSetsMap.emplace(p.first, set_count++);//��Ӷ�Ӧ��ϵ
    }
    //���еķ���̬����ͬһ�������� ....
    set<int> tempSet;
    for (const auto& e : dfa.statesVec)
    {
        if (dfa.endStatesMap.find(e.num) == dfa.endStatesMap.end())
        {
            tempSet.insert(e.num);
            statesSetsMap.emplace(e.num, set_count);//��Ӷ�Ӧ��ϵ
        }
    }
    statesSetsVec.push_back(tempSet);
    //���뵽����T��
    ++set_count;

    //�������Ի��ֽ��в��ϵķָ�ֱ�������ٷָ�Ϊֹ
    //ÿ��ѭ��ֻ��һ�����Ϸָ����������ÿ�����ֻ�����������һ���¼���
    bool flag = false;//�Ƿ��м��ϱ���
    while (flag)
    {
        int splitSetNumber = 0;	//����ֵļ��ϱ��
        set<int> newSet; //����ֳ����¼���

        //���ڻ����е�ÿ������
        for (int k = 0; k < statesSetsVec.size(); ++k)
        {
            auto& s = statesSetsVec[k];//��ǰ����Ϊs
            if (s.size() == 1)
                continue; //���������ֻ��һ��״̬���������������̬���ϣ�����
            else
            {
                auto& standard = dfa.statesVec[*(s.begin())]; //������һ��state��Ϊ��׼
                //������׼״̬�����б�
                for (const auto& c : ALLSET)
                {
                    //��鼯���е�ÿһ��״̬
                    for (const auto& i : s)
                    {
                        const auto& state = dfa.statesVec[i];
                        if (state.num == standard.num)
                            continue;									//���⡰�Լ����Լ��Ƚϡ�
                        auto findStateIt = state.EdgeMap.find(c);		//��state����c��
                        auto findStandardIt = standard.EdgeMap.find(c); //�ڻ�׼state����c��
                        //���ȼ�����1:����һ��״̬��c�߶���һ����c��
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
                        //���ȼ�����2:����c�ߣ���Ŀ��״̬��ͬ
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
                        break; //ÿ��ֻ�ֳ���������
                }
                if (flag)
                {
                    splitSetNumber = k;
                    break;
                }
            }
        }

        //ɾ������ּ����еı����״̬
        //���ڱ���ֳ������¼����е�ÿ��״̬
        for (const auto& s : newSet)
        {
            statesSetsVec[splitSetNumber].erase(s);//�ڱ���ֵļ�����ɾ�����״̬
            statesSetsMap.insert_or_assign(s, set_count);//�����µĶ�Ӧ��ϵ
        }
        //���¼���ѹ�뻮��T��
        if (newSet.size() != 0)
            statesSetsVec.push_back(newSet);
        //�����еļ�������һ
        ++set_count;

    }
    //�������

    //��̬���=tmp_dfa��̬���ڼ��ϱ��
    final_dfa.startState = statesSetsMap.find(dfa.startState)->second;

    //��̬
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
    //dfa��С�����
    return;
}

/*  some useful comments:

   pan jia xin
   start time:2020/05/20
   finish time:2020/05/21

   ***************** nfa to dfa �㷨 ******************************************
   *�Ӽ����취��
   * ���룺һ��NFA nfa(s0��nfa�Ŀ�ʼ״̬)
   * �����һ��DFA dfa
   * ��ʼ״̬��e-closure(s0)
   * ����״̬���������ٰ�����nfa��һ������״̬��״̬��
   * ������
   * һ��ʼDstate��ֻ��e-closure(s0)������δ���
   * while(Dstate����δ���״̬)
   * {
   *    ��T���ϱ�ǣ�
   *    for��ÿ���������a��
   *    {
   *      U=e-closure��move��T��a������
   *      if��U����Dstate�У�
   *      {
   *         ��U����Dstate�У��Ҳ����
   *      }
   *      Dtran[T,a]=U;//Dtran[T,a]��Dstate��dfa��ת���������൱�ڹ���һ���ߣ�
   *    }
   * }
   *********************ע******************************************************
   NFA״̬���ϵĲ�����
   e-closure(s):�ܹ���NFA��ĳ��״̬s��ʼֻͨ��eת������NFA��״̬����
   e-closure(T):�ܹ���T��ĳ��NFA״̬s��ʼֻͨ��e�����NFA״̬����
   move(T,a):�ܹ���T��ĳ��NFA״̬s��ʼͨ�����aת�������NFA״̬�ļ���

   ********************* dfa��С���㷨 *****************************************
   *ԭ������ǰdfa��״̬���Ϸֻ��ɶ���飬ÿ����ĸ���״̬֮���໥�������֣�
   *      Ȼ���ÿ�����е�״̬�ϲ���ͬһ��״̬
   * ������
   *       ��dfa���ֳɽ���״̬�ͷֽ���״̬������;
   *          //��dfa��״̬����ΪS����ʼ״̬s0������״̬��ΪF
   *          //(����ʼ����T������ ����״̬��ΪF,�ǽ���״̬��ΪS-F
   *       while(Tnew!=T)
   *       {
   *          Tnew=T;
   *          for(T�е�ÿ����G)
   *          {
   *             ��G���ֳɸ�С���飬ʹ������״̬s��t��
   *             ͬһС���е��ҽ��������е��������a��
   *             ״̬s��t��a�ϵ�ת��������T�е�ͬһ���飻
   *             ��Tnew�а�G�滻����G�ֻ��õ���С��
   *          }
   *       }
   *       Tfinal=T��
   *       Tfinal��ÿ������ѡ��һ��״̬��Ϊ����Ĵ�������minDFA��״̬���ϣ�
   *       minDFA�Ŀ�ʼ״̬����Щ������dfa�Ŀ�ʼ״̬��Ĵ���
   *       minDFS�Ľ���״̬�ǰ�����dfa����״̬����Ĵ���
   *       Ȼ����minDFA�ıߣ�
   *       //����s��Tfinal��ĳ����Ĵ�������dfa��������a���뿪s��ת������t��
   *           ��rΪt������H�Ĵ�����ô��minDFA��һ������һ����s��r������a��
   *           ��ת����
   *       ������״̬��
   *       ��ɣ�
   ************************************ע******************************************
   1��ʵ�����ڴʷ��������У������㷨�ĳ�ʼ�ֻ�Ӧ�ð�ʶ��ĳ���ض��Ĵ˷���Ԫ������
      ״̬�ŵ���Ӧ�˴ʷ���Ԫ��һ�����У�ͬʱ�Ѳ�ʶ���κδʷ���Ԫ��״̬�ŵ���һ����
 */