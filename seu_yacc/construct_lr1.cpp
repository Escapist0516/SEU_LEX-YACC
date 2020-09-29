/*
  2020/05/30
  construct_lr1.cpp
*/
#include "construct_lr1.h"
#include "structs.h"
#include"calculate_first.h"
#include<queue>

//求LR(1)闭包
void closure(set<LRItem>& lrItemSet, vector<pair<int, vector<int>>> &productionVec, map<int, pair<int, int>> &indexMap, int &boundT, map<int, set<int>> &firstMap)
{//LR项目集Ti；int版本，<产生式左部，右部>；映射：symbol->productionVec的下标。key为symbol，value为<以该symbol为左部的产生式在productionVec中的起始index，数量>；终结符结束序号；映射：symbol->first集，<symbol,first集>
	queue<LRItem> q1;
	//cout << endl << "求LR闭包" << endl;
	//新建空队列，并将状态I的所有产生式加入队列
	for (const auto& item : lrItemSet)
	{
		//cout << "将状态I的所有产生式加入队列" << endl;
		//cout << "产生式标号为" << item.Label << endl;
		q1.push(item);
	}

	pair<int, vector<int>> production;//要处理的产生式
	int pos, symbol;//点的位置，点后的符号
	int predictiveSymbol;//预测符
	set<int> predictiveSymbolSet;//预测符集合

	while (!q1.empty())
	{
		//取出头部LR项r
		//cout << "产生式在productionVec的标号为：" << q1.front().Label<<endl;
		production = productionVec[q1.front().Label];//Label为产生式标号，在productionVec找到对应的产生式
		//cout << "取得点的位置" << q1.front().Dotpos<< endl;
		pos = q1.front().Dotpos;//取得点的位置
		//如果点在末尾则跳过该产生式
		if (pos == production.second.size())
		{
			//cout << "点在末尾则跳过该产生式" << endl;
			q1.pop();
			continue;
		}
		else
		{
			//cout << "点后的Symbol为" << production.second[pos] << endl;
			symbol = production.second[pos];//取出·后的symbol
			//如果是终结符则不需要进行扩展
			if (symbol <= boundT)
			{
				//cout << "是终结符则不需要进行扩展" << endl;
				q1.pop();
				continue;
			}
			//否则，找到所有symbol对应的产生式
			//cout << "找到所有symbol对应的产生式" << endl;
			auto index = indexMap.find(symbol)->second;//pair<int,int>,以该symbol为左部的产生式在productionVec中的起始index，数量
			//cout << "产生式左部为" << symbol << endl;
			//cout << "起始位置为：" << index.first << endl;
			//cout << "数量为" << index.second << endl;
			//cout << "取出预测符标号为" << q1.front().Symbol << endl;
			predictiveSymbol = q1.front().Symbol;//取出预测符
			q1.pop();
			//对于每个产生式都新建一个LRItem r`
			for (int i = 0; i < index.second; i++)
			{
				LRItem newItem;
				newItem.Label = i + index.first;//产生式的标号
				//cout << "新的产生式标号为" <<i<<"+"<< index.first<<"="<< newItem.Label << endl;
				//求follow，即为first集
				
				vector<int> next;
				//cout << "取出点后所有的产生式" << endl;
				//cout << production.second.size() << endl;
				for (int j = pos+1; j < production.second.size(); j++)
				{
					//cout << "该产生式序号为" << production.second[j] << endl;
					next.push_back(production.second[j]);//取出点后所有产生式
				}
				//新建一个预测符集合
				predictiveSymbolSet.clear();
				//first(next)即为新的预测符集合
				first_string(predictiveSymbolSet, next, firstMap);
				//cout << "求预测符集合结束" << endl;
				set<int>::iterator it;
				//如果新的预测符集合中包含空，则去掉空再插入production原有的预测符
				if (predictiveSymbolSet.find(-1) != predictiveSymbolSet.end())
				{
					//cout << "新的预测符集合包含空" << endl;
					//表示有空
					predictiveSymbolSet.erase(-1);
					predictiveSymbolSet.insert(predictiveSymbol);
				}
				//遍历预测符集合中的所有符号
				//cout << "遍历预测符集合中的所有符号" << endl;
				for (const auto& pS : predictiveSymbolSet)
				{
					//将p设为r`的预测符
					newItem.Symbol = pS;
					//cout << "新的预测符为" << newItem.Symbol << endl;
					int flag = 0;//r`是否已经存在
					for (auto &item : lrItemSet)
					{
						if (newItem == item)
						{
							//cout<<"已经存在，flag设置为1"<<endl;
							flag = 1;
							break;
						}
					}
					if (flag == 1)
					{
						continue;//已经存在
					}

					//将新的LR加入状态I
					//cout << "将新的加入" << endl;
					q1.push(newItem);

					lrItemSet.insert(newItem);
					//cout << "--------------------------------------------" << endl;
				}
				//cout << "-----------------------------------------------" << endl;
			}
		}
		//cout << "---------------------------------------------------" << endl;
		//cout << "closure: unhandled size : " << q1.size() << endl;
	}
}
//求后继项目集
void gotonext(const set<LRItem>& lrItemSet, map<int, set<LRItem>>& newStateMap, vector<pair<int, vector<int>>> productionVec)
{//LR项目集Ii，<边上的符号，后继项目>；int版本，<产生式左部，右部>
	pair<int, vector<int>> production;//产生式
	LRItem newItem;
	int aa;//点后的符号
	//遍历状态I的所有LRItem
	for (auto& item : lrItemSet)
	{
		production = productionVec[item.Label];//根据标号取得产生式
		//如果在末尾直接跳过
		if (production.second.size() == item.Dotpos)
		{//点的位置为产生式右部大小，即为点在最后
			continue;
		}
		aa = production.second[item.Dotpos];//取得点后符号
		newItem = item;
		newItem.Dotpos++;//点后移

		auto finda = newStateMap.find(aa);
		//如果处理过点后的符号s，并入相应的LR项集合中
		if (finda != newStateMap.end())
		{
			finda->second.insert(newItem);
		}
		//如果没有处理过s则新建LR项集合后并入
		else
		{
			set<LRItem> newState;
			newState.insert(newItem);//新的加到集合里
			newStateMap.emplace(aa, newState);
		}
	}
}

//根据上下文无关文法构造LR（1）dfa
void construct_lr1(LRDFA &C, vector<pair<int, vector<int>>> &productionVec, map<int, pair<int, int>> &indexMap, int &boundT, map<int, set<int>> &firstMap)
{
    DFAState I0;
    int stateNum = 0;

    //首先构造增广文法G'
    //构造S'->·S,$R
    LRItem start_item;
    start_item.Symbol = -2; //$R= -2
    start_item.Label = 1;
    //将S'->·S,$R插入I0
    I0.LRItemsSet.insert(start_item);
    closure(I0.LRItemsSet, productionVec, indexMap, boundT, firstMap);
    I0.Num = stateNum++;
    //将C初始化为CLOUSURE（{ [S'->·S,$]}）
    C.statesVec.push_back(I0);
    
    queue<int> unhandledStates; //存放未处理States
    unhandledStates.push(0);//压入初始状态
    map<int, set<LRItem>> newStateMap;//<边，后继项目>

	while (unhandledStates.size() != 0)
	{
		//取出未处理项
		int top = unhandledStates.front();
		unhandledStates.pop();
		newStateMap.clear();
		//对该项集求后继项目集
		gotonext(C.statesVec[top].LRItemsSet, newStateMap, productionVec);
		for (auto& p : newStateMap)//对于当前状态的每条出边
		{
			//首先求边指向状态的闭包
			closure(p.second, productionVec, indexMap, boundT, firstMap);
			//然后检查是否存在相同的状态
			int edgeTo = -1;
			//检查是否存在相同的状态
			//遍历当前dfa状态集合
			for (auto& s : C.statesVec)
			{
				//存在则连接
				//相等条件:error1==0 && error2==0
				int error1 = 0, error2 = 0;
				//检验：p包含s
				for (auto& item1 : s.LRItemsSet)
				{
					int flag1 = 0;//记录当前item是否有相同
					for (auto& item2 : p.second)
					{

						if (item1 == item2)
						{
							flag1 = 1;
							break;
						}
					}
					//有不相等item
					if (flag1 == 0)
					{
						error1 = 1;
						break;
					}
				}
				//检验：s包含p
				for (auto& item1 : p.second)
				{
					int flag2 = 0;//记录当前item是否有相同
					for (auto& item2 : s.LRItemsSet)
					{

						if (item1 == item2)
						{
							flag2 = 1;
							break;
						}
					}
					//有不相等item
					if (flag2 == 0)
					{
						error2 = 1;
						break;
					}
				}
				if (error1 == 0 && error2 == 0)
				{
					edgeTo = s.Num;
					break;
				}
			}
			if (edgeTo == -1)
			{
				//说明不存在相同状态
				//新建状态
				DFAState newState;
				edgeTo = stateNum;
				//先push再修改
				C.statesVec.push_back(newState);
				unhandledStates.push(stateNum);
				C.statesVec.back().Num = stateNum++;
				C.statesVec.back().LRItemsSet = p.second;
			}

			//连接Ii->Ij
			C.statesVec[top].edgesMap.emplace(p.first, edgeTo);
		}
		cout << "construct lr1: state rest:" << unhandledStates.size() << endl;

	}

}

/*
***************************************************
lr(1)项集族的构造方法
***************************************************
  增广文法G'
  setOfOtems CLOUSURE(I){
   repeat
     for(I中每一项[A->x·By,a])
         for(G'中的每一个产生式B->z)
             for(First(ya)中的每个终结符号b)
                  将[B->·z,b]加入到集合I中；
    until 不能向I加入更多的项；
    return I；
    }
***************************************************
    setOfOtems GOTO（I,X）{
       将J初始化为空集；
       for(I中的每一个项[A->x·Xy,a])
           将项[A->x·Xy,a]加入到集合J中；
       retrun CLOUSURE(J);
    }
***************************************************
    dfa items(G'){
       将C初始化为CLOUSURE（{[S'->·S,$]}）；
       repeat
           for(C中的每个项集I)
              for(每个文法符号X)
                 if(GOTO(I,X)非空且不在C中){
                      将GOTO(I,X)加入C中；
                      构造一条I到C，的X边；
                 }
       until 不再有新的项加入C 中；
       return C；
    }
*****************************************************
*/

/*
	实际上我们还有另外一种方法，
	就是不构造lr1 dfa，直接
	高效地构造lalr dfa，通过先构
	造lr0内核项集，然后用传播的方
	法求向前看算符这样的方法。
	具体地来说：
	******************
	构造lr0项集族的算法
	CLOSURE(I){
	    J=I;
		for(J中的每个项A->x·By)
		  for(G的每个产生式)
		     if(项B->·z不在J中)
			    把他加入J中
        until 没有新的项加入J中
		return J；
	}
	GOTO(I,X){
	     J初始化为空集；
		 for（I中的每个项A->x·By）
		      将A->xB·y加入J中
		 return  CLOSURE(J)；
	}
	dfa itemss（G'）{
	     将C初始化为CLOUSURE（{[S'->·S]}）；
		  repeat
           for(C中的每个项集I)
              for(每个文法符号X)
                 if(GOTO(I,X)非空且不在C中){
                      将GOTO(I,X)加入C中；
                      构造一条I到C，的X边；
                 }
       until 不再有新的项加入C 中；
       return C；
	}
	*/
/*
	******************
	确定向前看符号的算法
	输入：一个LR0项集I的内核K以及一个文法符号X
	输出：由I中的项为GOTO(I,X)中内核项自发生成的向前看符号，以及I中将其向前看符号传播到GOTO(I,X)中内核项的项
	方法：
	     for(K中的每个项A->x·y){
		     J=CLOUSURE([A->x·y,#]);
			 if([B->z·Xp,a]在J中，且a不等于#)
			     那么GOTO(I,X)中的项B->zX·p的向前看符号是自发生成的
		     if([B->z·Xp,#]在J中)
			      那么向前看符号从I中的项A->x·y传播到了GOTO(I,X)中的项B->zX·p上
		          
		 }
*/
/*
	******************
	lalr（1）项集族内核的高效计算方法
	输入：一个增广文法G'
	输出：文法的LALR（1）项集族的内核
	方法：
	1）构造G的LR0项集族的内核。如果空间不紧张，最简单
	的方法是先构造LR0项集，然后删除其中的非内核项。如
	果内存紧张，可以只保存各个项集的内核项，并在计算一
	个项集I的GOTO之前先计算I的闭包（这意味这只用初始项
	和点不在产生式左端的项来表示项集
	2）将确定向前看算符的算法应用于每个LR0项集的内核和
	每个文法符号X，确定GOTO(I,X)中各内核项的哪些向前看
	符号是自发生成的，并确定向前看符号从I中哪个项被传
	播到GOTO(I,X)中的内核项上
	3）初始化一个表格，表中给出了每个项集中的每个内核
	项相关的向前看符号。最初，每个项的向前看符号只包括
	那些在（2）中确定为自发生成的符号
	4）不断扫描所有项集的内核项。当访问一个项i时，使
	用步骤（2）中得到的、用表格表示的信息，确定i将他的
	向前看符号传播到了哪些内核项中。项i的当前向前看符
	号集合被加到和这些被传播的内核项相关联的向前看符号
	集合中。继续在内核项上扫描，指导没有新的向前看符号
	被传播为止。
	******************
	然后对上面得到的LALR（1）项集族的
	内核分别求闭包，得到LALR（1）项集
	族，即lalr dfa
*/

//求lr0闭包的函数
void lr0_closure(set<LRItem>& lrItemSet, vector<pair<int, vector<int>>>& productionVec, map<int, pair<int, int>>& indexMap, int& boundT)
{
	queue<LRItem> q1;
	//cout << endl << "求LR0闭包" << endl;
	//新建空队列，并将状态I的所有产生式加入队列
	for (const auto& item : lrItemSet)
	{
		//cout << "将状态I的所有产生式加入队列" << endl;
		//cout << "产生式标号为" << item.Label << endl;
		q1.push(item);
	}
	//cout << q1.size();

	pair<int, vector<int>> production;//要处理的产生式
	int pos, symbol;//点的位置，点后的符号

	while (q1.size() != 0)
	{
		//取出头部LR项r

		production = productionVec[q1.front().Label];//Label为产生式标号，在productionVec找到对应的产生式

		pos = q1.front().Dotpos;//取得点的位置
		//如果点在末尾则跳过该产生式
		if (pos == production.second.size())
		{
			//cout << "点在末尾则跳过该产生式" << endl;
			q1.pop();
			//cout << "lr0: closure: unhandled size : " << q1.size() << endl;
			continue;
		}
		else
		{
			//cout << "点后的Symbol为" << production.second[pos] << endl;
			symbol = production.second[pos];//取出·后的symbol
			//如果是终结符则不需要进行扩展
			if (symbol <= boundT)
			{
				//cout << "是终结符则不需要进行扩展" << endl;
				q1.pop();
				//cout << "lr0: closure: unhandled size : " << q1.size() << endl;
				continue;
			}
			//否则，找到所有symbol对应的产生式
			auto index = indexMap.find(symbol)->second;//pair<int,int>,以该symbol为左部的产生式在productionVec中的起始index，数量

			q1.pop();
			//对于每个产生式都新建一个LRItem r`
			for (int i = 0; i < index.second; i++)
			{
				LRItem newItem;
				newItem.Label = i + index.first;//产生式的标号
				newItem.Symbol = -3;
				int flag = 0;//r`是否已经存在
				for (auto& item : lrItemSet)
				{
					if (newItem == item)
					{
						//cout<<"已经存在，flag设置为1"<<endl;
						flag = 1;
						break;
					}
				}
				if (flag == 1)
				{
					continue;//已经存在
				}
				q1.push(newItem);
				lrItemSet.insert(newItem);

			}
		}
		//cout << "lr0: closure: unhandled size : " << q1.size() << endl;
	}

}

//求lr0 dfa的函数
void construct_lr0(LRDFA &lr0, vector<pair<int, vector<int>>> productionVec, map<int, pair<int, int>> indexMap, int boundT, map<int, set<int>> firstMap)
{
	DFAState I0;
	int num = 0;

	//先构造增广文法S'->·S,#
	LRItem start_item;
	start_item.Label = 1;
	start_item.Symbol = -3;
	I0.LRItemsSet.insert(start_item);
	lr0_closure(I0.LRItemsSet, productionVec, indexMap, boundT);

}

//构造传播表的函数
void construct_trans_table_and_init_lalr(LRDFA &lr0,LRDFA &lalr, vector<map<int, vector<pair<int, int>>>> &trans_table, vector<pair<int, vector<int>>> productionVec, map<int, pair<int, int>> indexMap, int boundT, map<int, set<int>> firstMap)
{
	//对于lr0状态集的每个项集
	for (auto& temp_state : lr0.statesVec)
	{
		//需要在表中构建一行
		map<int, vector<pair<int, int>>> temp_line;
		//接下来对于每个文法符号，应用确定向前看符号的算法
		//对于当前状态里的每个内核项
		for (auto& temp_core_item : temp_state.LRItemsSet)
		{
			vector<pair<int, int>>current_target_item;
			set<LRItem> closure_temp_core_item;
			closure_temp_core_item.insert(temp_core_item);
			//求当前内核项的闭包
			closure(closure_temp_core_item, productionVec, indexMap, boundT, firstMap);
			//然后对于闭包里的每个项
			for (auto& temp_core_item_in_closure : closure_temp_core_item)
			{
				LRItem t1 = temp_core_item_in_closure;
				int t2 = temp_core_item_in_closure.Label;
				int t3 = temp_core_item_in_closure.Dotpos;
				int t4 = temp_core_item_in_closure.Symbol;
				//LRItem temp_core_item_in_closure_i = temp_core_item_in_closure;
				//如果点在产生式最后
				if (temp_core_item_in_closure.Dotpos == productionVec[temp_core_item_in_closure.Label].second.size())
				{
					continue;
				}
				//查看他的向前看符号
				//如果是# -3，那么断定向前看符号从当前的项 temp_core_item 传播到了goto（I,X）中的内核项temp_core_item_in_closure（但是把点向后移动一位）上
				if (temp_core_item_in_closure.Symbol == -3)
				{
					int x = productionVec[temp_core_item_in_closure.Label].second[temp_core_item_in_closure.Dotpos];
					//寻找目标状态goto（i，x）
					map<int, int>::iterator lt_t;
					lt_t = temp_state.edgesMap.find(x);
					//如果找到，那么当前内核项的目标传播项中增加目标项
					if (lt_t != temp_state.edgesMap.end())
					{
						pair<int, int>temp_pair(lt_t->second,
							temp_core_item_in_closure.Label * 10 + temp_core_item_in_closure.Dotpos + 1);
						current_target_item.push_back(temp_pair);
						
					}
				}
				//如果向前看符号不是# -3，那么断定goto（I,X）中的内核项temp_core_item_in_closure（但是把点向后移动一位）是自发生成的
				else if (temp_core_item_in_closure.Symbol != -3)
				{
					int x = productionVec[temp_core_item_in_closure.Label].second[temp_core_item_in_closure.Dotpos];
					//寻找目标状态goto（i，x）
					map<int, int>::iterator lt_t;
					lt_t = temp_state.edgesMap.find(x);
					//如果找到，那么目标项的向前看算符设置一下
					if (lt_t != temp_state.edgesMap.end())
					{
						LRItem temp_core_item_in_closure_new;
						temp_core_item_in_closure_new.Dotpos = temp_core_item_in_closure.Dotpos + 1;
						temp_core_item_in_closure_new.Label = temp_core_item_in_closure.Label;
						temp_core_item_in_closure_new.Symbol = temp_core_item_in_closure.Symbol;
						lr0.statesVec[lt_t->second].LRItemsSet.insert(temp_core_item_in_closure_new);
					}
				}
			}
			pair<int, vector<pair<int, int>>>current_item_to_target(temp_core_item.Label * 10 + temp_core_item.Dotpos, current_target_item);
			temp_line.emplace(current_item_to_target);
		}
		trans_table.push_back(temp_line);
	}
	//初始项'S->·S的向前看符号$是自发生成的
	LRItem start_item;
	start_item.Dotpos = 0;
	start_item.Label = 1;
	start_item.Symbol = -2;
	lr0.statesVec[lr0.Start].LRItemsSet.insert(start_item);

	//然后建立一个初始的lalr dfa ，每个项集仅包括那些自发生成向前看符号的项
	for (int i = 0; i < lr0.statesVec.size(); i++)
	{
		DFAState current_init_lalr_state;
		current_init_lalr_state.Num = i;
		current_init_lalr_state.edgesMap = lr0.statesVec[i].edgesMap;
		for (auto& ci : lr0.statesVec[i].LRItemsSet)
		{
			if (ci.Symbol != -3)
			{
				current_init_lalr_state.LRItemsSet.insert(ci);
			}
		}
		lalr.statesVec.push_back(current_init_lalr_state);
	}
}


//根据传播表不断迭代计算向前看符号的函数
void cal_lalr_core(LRDFA& lalr, vector<map<int, vector<pair<int, int>>>>& trans_table)
{
	bool is_transf = true;
	while (is_transf)
	{
		is_transf = false;
		for (int i = 0; i < lalr.statesVec.size(); i++)
		{
			for (auto& item : lalr.statesVec[i].LRItemsSet)
			{
				map<int, vector<pair<int, int>>>::iterator lt_t;
				lt_t = trans_table[i].find(item.Label*10+item.Dotpos);
				if (lt_t != trans_table[i].end())
				{
					vector<pair<int, int>> target_set = lt_t->second;
					for (auto& p : target_set)
					{
						int target_state = p.first;
						int target_item = p.second;
						LRItem new_item;
						new_item.Symbol = item.Symbol;
						new_item.Label = target_item / 10;
						new_item.Dotpos = target_item % 10;
						bool flag = false;
						for (auto& check : lalr.statesVec[target_state].LRItemsSet)
						{
							if (check == new_item)
							{
								flag = true;
								break;
							}
						}
						if (!flag)
						{
							lalr.statesVec[target_state].LRItemsSet.insert(new_item);
							is_transf = true;
						}
					}
				}
			}
		}
	}
}

void quick_construct_lalr(LRDFA& lalr_dfa, vector<pair<int, vector<int>>> productionVec, map<int, pair<int, int>> indexMap, int boundT, map<int, set<int>> firstMap)
{
	//首先构造一个lr0 dfa
	LRDFA lr0;
	DFAState I0;
	int stateNum = 0;

	//首先构造增广文法G'
	//构造S'->·S
	LRItem start_item;
	start_item.Label = 1;
	start_item.Symbol = -3;
	//将S'->·S 插入I0
	I0.LRItemsSet.insert(start_item);
	lr0_closure(I0.LRItemsSet, productionVec, indexMap, boundT);
	I0.Num = stateNum++;
	//将C初始化为CLOUSURE（{ [S'->·S,$]}）
	lr0.statesVec.push_back(I0);

	queue<int> unhandledStates; //存放未处理States
	unhandledStates.push(0);//压入初始状态
	map<int, set<LRItem>> newStateMap;//<边，后继项目>

	while (unhandledStates.size() != 0)
	{
		//取出未处理项
		int top = unhandledStates.front();
		unhandledStates.pop();
		newStateMap.clear();
		//对该项集求后继项目集
		gotonext(lr0.statesVec[top].LRItemsSet, newStateMap, productionVec);
		for (auto& p : newStateMap)//对于当前状态的每条出边
		{
			//首先求边指向状态的闭包
			lr0_closure(p.second, productionVec, indexMap, boundT);
			//然后检查是否存在相同的状态
			int edgeTo = -1;
			//检查是否存在相同的状态
			//遍历当前dfa状态集合
			for (auto& s : lr0.statesVec)
			{
				//存在则连接
				//相等条件:error1==0 && error2==0
				int error1 = 0, error2 = 0;
				//检验：p包含s
				for (auto& item1 : s.LRItemsSet)
				{
					int flag1 = 0;//记录当前item是否有相同
					for (auto& item2 : p.second)
					{

						if (item1.Dotpos == item2.Dotpos)
						{
							if (item1.Label == item2.Label)
							{
								flag1 = 1;
								break;
							}
						}
					}
					//有不相等item
					if (flag1 == 0)
					{
						error1 = 1;
						break;
					}
				}
				//检验：s包含p
				for (auto& item1 : p.second)
				{
					int flag2 = 0;//记录当前item是否有相同
					for (auto& item2 : s.LRItemsSet)
					{
						if (item1.Dotpos == item2.Dotpos)
						{
							if (item1.Label == item2.Label)
							{
								flag2 = 1;
								break;
							}
						}
					}
					//有不相等item
					if (flag2 == 0)
					{
						error2 = 1;
						break;
					}
				}
				if (error1 == 0 && error2 == 0)
				{
					edgeTo = s.Num;
					break;
				}
			}
			if (edgeTo == -1)
			{
				//说明不存在相同状态
				//新建状态
				DFAState newState;
				edgeTo = stateNum;
				//先push再修改
				lr0.statesVec.push_back(newState);
				unhandledStates.push(stateNum);
				lr0.statesVec.back().Num = stateNum++;
				lr0.statesVec.back().LRItemsSet = p.second;
			}

			//连接Ii->Ij
			lr0.statesVec[top].edgesMap.emplace(p.first, edgeTo);
		}
		//cout << "quick_construct_lalr: construct lr0: state rest:" << unhandledStates.size() << endl;
	}
	//cout << "quick_construct_lalr: construct lr0 dfa finished! " << endl;
	//lr0 dfa 构造完成
	//cout << "quick_construct_lalr: start to construct lr0 core dfa " << endl;
	//然后用lr0的内核项集来表示lr0 dfa 
	//（只用初始项和点不在产生式左端的项来表示项集）
	//删除每个dfa状态中的非内核项
	for (auto& temp_state : lr0.statesVec)
	{
		for (auto& temp_item : temp_state.LRItemsSet)
		{
			//如果当前项是非内核项，那么删除掉它
			//(这意味着 删除 既不是 初始项 也不是 点在产生式左端 的项)
			//如果不是初始项且点在产生式左端
			if (temp_item.Label != 1 && temp_item.Dotpos == 0)
			{
				//删除这个项
				temp_state.LRItemsSet.erase(temp_state.LRItemsSet.find(temp_item));
			}
		}
	}
	//现在得到一个lr0 内核项集 dfa
	//cout << "quick_construct_lalr: construct lr0 core dfa finished! " << lr0.statesVec.size() << endl;

	//初始化一个表来保存传播的向前看符号规则
	vector<map<int, vector<pair<int, int>>>> trans_table;
	//第一个下标为状态号，外面的map是当前项特征和它传播指向的项的集合 里面的map里面存的是被传播到的的项所在的状态号和项标志码(项编号*10+点位置)组成的pair

	//开始构造 转换表 与 自发生成向前看符号 以及 初始化lalr
	construct_trans_table_and_init_lalr(lr0,lalr_dfa, trans_table,productionVec,indexMap,boundT,firstMap);

	//根据传播表计算lalr
	cal_lalr_core(lalr_dfa, trans_table);

	//然后对每个dfa状态求闭包

	for (int i = 0; i < lalr_dfa.statesVec.size(); i++)
	{
		closure(lalr_dfa.statesVec[i].LRItemsSet, productionVec, indexMap, boundT, firstMap);
	}
	//最终完成，得到lalr dfa
	        
	//cout << lr0.statesVec.size();
}