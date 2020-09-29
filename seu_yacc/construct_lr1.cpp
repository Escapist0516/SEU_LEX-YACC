/*
  2020/05/30
  construct_lr1.cpp
*/
#include "construct_lr1.h"
#include "structs.h"
#include"calculate_first.h"
#include<queue>

//��LR(1)�հ�
void closure(set<LRItem>& lrItemSet, vector<pair<int, vector<int>>> &productionVec, map<int, pair<int, int>> &indexMap, int &boundT, map<int, set<int>> &firstMap)
{//LR��Ŀ��Ti��int�汾��<����ʽ�󲿣��Ҳ�>��ӳ�䣺symbol->productionVec���±ꡣkeyΪsymbol��valueΪ<�Ը�symbolΪ�󲿵Ĳ���ʽ��productionVec�е���ʼindex������>���ս��������ţ�ӳ�䣺symbol->first����<symbol,first��>
	queue<LRItem> q1;
	//cout << endl << "��LR�հ�" << endl;
	//�½��ն��У�����״̬I�����в���ʽ�������
	for (const auto& item : lrItemSet)
	{
		//cout << "��״̬I�����в���ʽ�������" << endl;
		//cout << "����ʽ���Ϊ" << item.Label << endl;
		q1.push(item);
	}

	pair<int, vector<int>> production;//Ҫ����Ĳ���ʽ
	int pos, symbol;//���λ�ã����ķ���
	int predictiveSymbol;//Ԥ���
	set<int> predictiveSymbolSet;//Ԥ�������

	while (!q1.empty())
	{
		//ȡ��ͷ��LR��r
		//cout << "����ʽ��productionVec�ı��Ϊ��" << q1.front().Label<<endl;
		production = productionVec[q1.front().Label];//LabelΪ����ʽ��ţ���productionVec�ҵ���Ӧ�Ĳ���ʽ
		//cout << "ȡ�õ��λ��" << q1.front().Dotpos<< endl;
		pos = q1.front().Dotpos;//ȡ�õ��λ��
		//�������ĩβ�������ò���ʽ
		if (pos == production.second.size())
		{
			//cout << "����ĩβ�������ò���ʽ" << endl;
			q1.pop();
			continue;
		}
		else
		{
			//cout << "����SymbolΪ" << production.second[pos] << endl;
			symbol = production.second[pos];//ȡ�������symbol
			//������ս������Ҫ������չ
			if (symbol <= boundT)
			{
				//cout << "���ս������Ҫ������չ" << endl;
				q1.pop();
				continue;
			}
			//�����ҵ�����symbol��Ӧ�Ĳ���ʽ
			//cout << "�ҵ�����symbol��Ӧ�Ĳ���ʽ" << endl;
			auto index = indexMap.find(symbol)->second;//pair<int,int>,�Ը�symbolΪ�󲿵Ĳ���ʽ��productionVec�е���ʼindex������
			//cout << "����ʽ��Ϊ" << symbol << endl;
			//cout << "��ʼλ��Ϊ��" << index.first << endl;
			//cout << "����Ϊ" << index.second << endl;
			//cout << "ȡ��Ԥ������Ϊ" << q1.front().Symbol << endl;
			predictiveSymbol = q1.front().Symbol;//ȡ��Ԥ���
			q1.pop();
			//����ÿ������ʽ���½�һ��LRItem r`
			for (int i = 0; i < index.second; i++)
			{
				LRItem newItem;
				newItem.Label = i + index.first;//����ʽ�ı��
				//cout << "�µĲ���ʽ���Ϊ" <<i<<"+"<< index.first<<"="<< newItem.Label << endl;
				//��follow����Ϊfirst��
				
				vector<int> next;
				//cout << "ȡ��������еĲ���ʽ" << endl;
				//cout << production.second.size() << endl;
				for (int j = pos+1; j < production.second.size(); j++)
				{
					//cout << "�ò���ʽ���Ϊ" << production.second[j] << endl;
					next.push_back(production.second[j]);//ȡ��������в���ʽ
				}
				//�½�һ��Ԥ�������
				predictiveSymbolSet.clear();
				//first(next)��Ϊ�µ�Ԥ�������
				first_string(predictiveSymbolSet, next, firstMap);
				//cout << "��Ԥ������Ͻ���" << endl;
				set<int>::iterator it;
				//����µ�Ԥ��������а����գ���ȥ�����ٲ���productionԭ�е�Ԥ���
				if (predictiveSymbolSet.find(-1) != predictiveSymbolSet.end())
				{
					//cout << "�µ�Ԥ������ϰ�����" << endl;
					//��ʾ�п�
					predictiveSymbolSet.erase(-1);
					predictiveSymbolSet.insert(predictiveSymbol);
				}
				//����Ԥ��������е����з���
				//cout << "����Ԥ��������е����з���" << endl;
				for (const auto& pS : predictiveSymbolSet)
				{
					//��p��Ϊr`��Ԥ���
					newItem.Symbol = pS;
					//cout << "�µ�Ԥ���Ϊ" << newItem.Symbol << endl;
					int flag = 0;//r`�Ƿ��Ѿ�����
					for (auto &item : lrItemSet)
					{
						if (newItem == item)
						{
							//cout<<"�Ѿ����ڣ�flag����Ϊ1"<<endl;
							flag = 1;
							break;
						}
					}
					if (flag == 1)
					{
						continue;//�Ѿ�����
					}

					//���µ�LR����״̬I
					//cout << "���µļ���" << endl;
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
//������Ŀ��
void gotonext(const set<LRItem>& lrItemSet, map<int, set<LRItem>>& newStateMap, vector<pair<int, vector<int>>> productionVec)
{//LR��Ŀ��Ii��<���ϵķ��ţ������Ŀ>��int�汾��<����ʽ�󲿣��Ҳ�>
	pair<int, vector<int>> production;//����ʽ
	LRItem newItem;
	int aa;//���ķ���
	//����״̬I������LRItem
	for (auto& item : lrItemSet)
	{
		production = productionVec[item.Label];//���ݱ��ȡ�ò���ʽ
		//�����ĩβֱ������
		if (production.second.size() == item.Dotpos)
		{//���λ��Ϊ����ʽ�Ҳ���С����Ϊ�������
			continue;
		}
		aa = production.second[item.Dotpos];//ȡ�õ�����
		newItem = item;
		newItem.Dotpos++;//�����

		auto finda = newStateMap.find(aa);
		//�����������ķ���s��������Ӧ��LR�����
		if (finda != newStateMap.end())
		{
			finda->second.insert(newItem);
		}
		//���û�д����s���½�LR��Ϻ���
		else
		{
			set<LRItem> newState;
			newState.insert(newItem);//�µļӵ�������
			newStateMap.emplace(aa, newState);
		}
	}
}

//�����������޹��ķ�����LR��1��dfa
void construct_lr1(LRDFA &C, vector<pair<int, vector<int>>> &productionVec, map<int, pair<int, int>> &indexMap, int &boundT, map<int, set<int>> &firstMap)
{
    DFAState I0;
    int stateNum = 0;

    //���ȹ��������ķ�G'
    //����S'->��S,$R
    LRItem start_item;
    start_item.Symbol = -2; //$R= -2
    start_item.Label = 1;
    //��S'->��S,$R����I0
    I0.LRItemsSet.insert(start_item);
    closure(I0.LRItemsSet, productionVec, indexMap, boundT, firstMap);
    I0.Num = stateNum++;
    //��C��ʼ��ΪCLOUSURE��{ [S'->��S,$]}��
    C.statesVec.push_back(I0);
    
    queue<int> unhandledStates; //���δ����States
    unhandledStates.push(0);//ѹ���ʼ״̬
    map<int, set<LRItem>> newStateMap;//<�ߣ������Ŀ>

	while (unhandledStates.size() != 0)
	{
		//ȡ��δ������
		int top = unhandledStates.front();
		unhandledStates.pop();
		newStateMap.clear();
		//�Ը��������Ŀ��
		gotonext(C.statesVec[top].LRItemsSet, newStateMap, productionVec);
		for (auto& p : newStateMap)//���ڵ�ǰ״̬��ÿ������
		{
			//�������ָ��״̬�ıհ�
			closure(p.second, productionVec, indexMap, boundT, firstMap);
			//Ȼ�����Ƿ������ͬ��״̬
			int edgeTo = -1;
			//����Ƿ������ͬ��״̬
			//������ǰdfa״̬����
			for (auto& s : C.statesVec)
			{
				//����������
				//�������:error1==0 && error2==0
				int error1 = 0, error2 = 0;
				//���飺p����s
				for (auto& item1 : s.LRItemsSet)
				{
					int flag1 = 0;//��¼��ǰitem�Ƿ�����ͬ
					for (auto& item2 : p.second)
					{

						if (item1 == item2)
						{
							flag1 = 1;
							break;
						}
					}
					//�в����item
					if (flag1 == 0)
					{
						error1 = 1;
						break;
					}
				}
				//���飺s����p
				for (auto& item1 : p.second)
				{
					int flag2 = 0;//��¼��ǰitem�Ƿ�����ͬ
					for (auto& item2 : s.LRItemsSet)
					{

						if (item1 == item2)
						{
							flag2 = 1;
							break;
						}
					}
					//�в����item
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
				//˵����������ͬ״̬
				//�½�״̬
				DFAState newState;
				edgeTo = stateNum;
				//��push���޸�
				C.statesVec.push_back(newState);
				unhandledStates.push(stateNum);
				C.statesVec.back().Num = stateNum++;
				C.statesVec.back().LRItemsSet = p.second;
			}

			//����Ii->Ij
			C.statesVec[top].edgesMap.emplace(p.first, edgeTo);
		}
		cout << "construct lr1: state rest:" << unhandledStates.size() << endl;

	}

}

/*
***************************************************
lr(1)���Ĺ��췽��
***************************************************
  �����ķ�G'
  setOfOtems CLOUSURE(I){
   repeat
     for(I��ÿһ��[A->x��By,a])
         for(G'�е�ÿһ������ʽB->z)
             for(First(ya)�е�ÿ���ս����b)
                  ��[B->��z,b]���뵽����I�У�
    until ������I���������
    return I��
    }
***************************************************
    setOfOtems GOTO��I,X��{
       ��J��ʼ��Ϊ�ռ���
       for(I�е�ÿһ����[A->x��Xy,a])
           ����[A->x��Xy,a]���뵽����J�У�
       retrun CLOUSURE(J);
    }
***************************************************
    dfa items(G'){
       ��C��ʼ��ΪCLOUSURE��{[S'->��S,$]}����
       repeat
           for(C�е�ÿ���I)
              for(ÿ���ķ�����X)
                 if(GOTO(I,X)�ǿ��Ҳ���C��){
                      ��GOTO(I,X)����C�У�
                      ����һ��I��C����X�ߣ�
                 }
       until �������µ������C �У�
       return C��
    }
*****************************************************
*/

/*
	ʵ�������ǻ�������һ�ַ�����
	���ǲ�����lr1 dfa��ֱ��
	��Ч�ع���lalr dfa��ͨ���ȹ�
	��lr0�ں����Ȼ���ô����ķ�
	������ǰ����������ķ�����
	�������˵��
	******************
	����lr0�����㷨
	CLOSURE(I){
	    J=I;
		for(J�е�ÿ����A->x��By)
		  for(G��ÿ������ʽ)
		     if(��B->��z����J��)
			    ��������J��
        until û���µ������J��
		return J��
	}
	GOTO(I,X){
	     J��ʼ��Ϊ�ռ���
		 for��I�е�ÿ����A->x��By��
		      ��A->xB��y����J��
		 return  CLOSURE(J)��
	}
	dfa itemss��G'��{
	     ��C��ʼ��ΪCLOUSURE��{[S'->��S]}����
		  repeat
           for(C�е�ÿ���I)
              for(ÿ���ķ�����X)
                 if(GOTO(I,X)�ǿ��Ҳ���C��){
                      ��GOTO(I,X)����C�У�
                      ����һ��I��C����X�ߣ�
                 }
       until �������µ������C �У�
       return C��
	}
	*/
/*
	******************
	ȷ����ǰ�����ŵ��㷨
	���룺һ��LR0�I���ں�K�Լ�һ���ķ�����X
	�������I�е���ΪGOTO(I,X)���ں����Է����ɵ���ǰ�����ţ��Լ�I�н�����ǰ�����Ŵ�����GOTO(I,X)���ں������
	������
	     for(K�е�ÿ����A->x��y){
		     J=CLOUSURE([A->x��y,#]);
			 if([B->z��Xp,a]��J�У���a������#)
			     ��ôGOTO(I,X)�е���B->zX��p����ǰ���������Է����ɵ�
		     if([B->z��Xp,#]��J��)
			      ��ô��ǰ�����Ŵ�I�е���A->x��y��������GOTO(I,X)�е���B->zX��p��
		          
		 }
*/
/*
	******************
	lalr��1������ں˵ĸ�Ч���㷽��
	���룺һ�������ķ�G'
	������ķ���LALR��1�������ں�
	������
	1������G��LR0�����ںˡ�����ռ䲻���ţ����
	�ķ������ȹ���LR0���Ȼ��ɾ�����еķ��ں����
	���ڴ���ţ�����ֻ�����������ں�����ڼ���һ
	���I��GOTO֮ǰ�ȼ���I�ıհ�������ζ��ֻ�ó�ʼ��
	�͵㲻�ڲ���ʽ��˵�������ʾ�
	2����ȷ����ǰ��������㷨Ӧ����ÿ��LR0����ں˺�
	ÿ���ķ�����X��ȷ��GOTO(I,X)�и��ں������Щ��ǰ��
	�������Է����ɵģ���ȷ����ǰ�����Ŵ�I���ĸ����
	����GOTO(I,X)�е��ں�����
	3����ʼ��һ����񣬱��и�����ÿ����е�ÿ���ں�
	����ص���ǰ�����š������ÿ�������ǰ������ֻ����
	��Щ�ڣ�2����ȷ��Ϊ�Է����ɵķ���
	4������ɨ����������ں��������һ����iʱ��ʹ
	�ò��裨2���еõ��ġ��ñ���ʾ����Ϣ��ȷ��i������
	��ǰ�����Ŵ���������Щ�ں����С���i�ĵ�ǰ��ǰ����
	�ż��ϱ��ӵ�����Щ���������ں������������ǰ������
	�����С��������ں�����ɨ�裬ָ��û���µ���ǰ������
	������Ϊֹ��
	******************
	Ȼ�������õ���LALR��1������
	�ں˷ֱ���հ����õ�LALR��1���
	�壬��lalr dfa
*/

//��lr0�հ��ĺ���
void lr0_closure(set<LRItem>& lrItemSet, vector<pair<int, vector<int>>>& productionVec, map<int, pair<int, int>>& indexMap, int& boundT)
{
	queue<LRItem> q1;
	//cout << endl << "��LR0�հ�" << endl;
	//�½��ն��У�����״̬I�����в���ʽ�������
	for (const auto& item : lrItemSet)
	{
		//cout << "��״̬I�����в���ʽ�������" << endl;
		//cout << "����ʽ���Ϊ" << item.Label << endl;
		q1.push(item);
	}
	//cout << q1.size();

	pair<int, vector<int>> production;//Ҫ����Ĳ���ʽ
	int pos, symbol;//���λ�ã����ķ���

	while (q1.size() != 0)
	{
		//ȡ��ͷ��LR��r

		production = productionVec[q1.front().Label];//LabelΪ����ʽ��ţ���productionVec�ҵ���Ӧ�Ĳ���ʽ

		pos = q1.front().Dotpos;//ȡ�õ��λ��
		//�������ĩβ�������ò���ʽ
		if (pos == production.second.size())
		{
			//cout << "����ĩβ�������ò���ʽ" << endl;
			q1.pop();
			//cout << "lr0: closure: unhandled size : " << q1.size() << endl;
			continue;
		}
		else
		{
			//cout << "����SymbolΪ" << production.second[pos] << endl;
			symbol = production.second[pos];//ȡ�������symbol
			//������ս������Ҫ������չ
			if (symbol <= boundT)
			{
				//cout << "���ս������Ҫ������չ" << endl;
				q1.pop();
				//cout << "lr0: closure: unhandled size : " << q1.size() << endl;
				continue;
			}
			//�����ҵ�����symbol��Ӧ�Ĳ���ʽ
			auto index = indexMap.find(symbol)->second;//pair<int,int>,�Ը�symbolΪ�󲿵Ĳ���ʽ��productionVec�е���ʼindex������

			q1.pop();
			//����ÿ������ʽ���½�һ��LRItem r`
			for (int i = 0; i < index.second; i++)
			{
				LRItem newItem;
				newItem.Label = i + index.first;//����ʽ�ı��
				newItem.Symbol = -3;
				int flag = 0;//r`�Ƿ��Ѿ�����
				for (auto& item : lrItemSet)
				{
					if (newItem == item)
					{
						//cout<<"�Ѿ����ڣ�flag����Ϊ1"<<endl;
						flag = 1;
						break;
					}
				}
				if (flag == 1)
				{
					continue;//�Ѿ�����
				}
				q1.push(newItem);
				lrItemSet.insert(newItem);

			}
		}
		//cout << "lr0: closure: unhandled size : " << q1.size() << endl;
	}

}

//��lr0 dfa�ĺ���
void construct_lr0(LRDFA &lr0, vector<pair<int, vector<int>>> productionVec, map<int, pair<int, int>> indexMap, int boundT, map<int, set<int>> firstMap)
{
	DFAState I0;
	int num = 0;

	//�ȹ��������ķ�S'->��S,#
	LRItem start_item;
	start_item.Label = 1;
	start_item.Symbol = -3;
	I0.LRItemsSet.insert(start_item);
	lr0_closure(I0.LRItemsSet, productionVec, indexMap, boundT);

}

//���촫����ĺ���
void construct_trans_table_and_init_lalr(LRDFA &lr0,LRDFA &lalr, vector<map<int, vector<pair<int, int>>>> &trans_table, vector<pair<int, vector<int>>> productionVec, map<int, pair<int, int>> indexMap, int boundT, map<int, set<int>> firstMap)
{
	//����lr0״̬����ÿ���
	for (auto& temp_state : lr0.statesVec)
	{
		//��Ҫ�ڱ��й���һ��
		map<int, vector<pair<int, int>>> temp_line;
		//����������ÿ���ķ����ţ�Ӧ��ȷ����ǰ�����ŵ��㷨
		//���ڵ�ǰ״̬���ÿ���ں���
		for (auto& temp_core_item : temp_state.LRItemsSet)
		{
			vector<pair<int, int>>current_target_item;
			set<LRItem> closure_temp_core_item;
			closure_temp_core_item.insert(temp_core_item);
			//��ǰ�ں���ıհ�
			closure(closure_temp_core_item, productionVec, indexMap, boundT, firstMap);
			//Ȼ����ڱհ����ÿ����
			for (auto& temp_core_item_in_closure : closure_temp_core_item)
			{
				LRItem t1 = temp_core_item_in_closure;
				int t2 = temp_core_item_in_closure.Label;
				int t3 = temp_core_item_in_closure.Dotpos;
				int t4 = temp_core_item_in_closure.Symbol;
				//LRItem temp_core_item_in_closure_i = temp_core_item_in_closure;
				//������ڲ���ʽ���
				if (temp_core_item_in_closure.Dotpos == productionVec[temp_core_item_in_closure.Label].second.size())
				{
					continue;
				}
				//�鿴������ǰ������
				//�����# -3����ô�϶���ǰ�����Ŵӵ�ǰ���� temp_core_item ��������goto��I,X���е��ں���temp_core_item_in_closure�����ǰѵ�����ƶ�һλ����
				if (temp_core_item_in_closure.Symbol == -3)
				{
					int x = productionVec[temp_core_item_in_closure.Label].second[temp_core_item_in_closure.Dotpos];
					//Ѱ��Ŀ��״̬goto��i��x��
					map<int, int>::iterator lt_t;
					lt_t = temp_state.edgesMap.find(x);
					//����ҵ�����ô��ǰ�ں����Ŀ�괫����������Ŀ����
					if (lt_t != temp_state.edgesMap.end())
					{
						pair<int, int>temp_pair(lt_t->second,
							temp_core_item_in_closure.Label * 10 + temp_core_item_in_closure.Dotpos + 1);
						current_target_item.push_back(temp_pair);
						
					}
				}
				//�����ǰ�����Ų���# -3����ô�϶�goto��I,X���е��ں���temp_core_item_in_closure�����ǰѵ�����ƶ�һλ�����Է����ɵ�
				else if (temp_core_item_in_closure.Symbol != -3)
				{
					int x = productionVec[temp_core_item_in_closure.Label].second[temp_core_item_in_closure.Dotpos];
					//Ѱ��Ŀ��״̬goto��i��x��
					map<int, int>::iterator lt_t;
					lt_t = temp_state.edgesMap.find(x);
					//����ҵ�����ôĿ�������ǰ���������һ��
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
	//��ʼ��'S->��S����ǰ������$���Է����ɵ�
	LRItem start_item;
	start_item.Dotpos = 0;
	start_item.Label = 1;
	start_item.Symbol = -2;
	lr0.statesVec[lr0.Start].LRItemsSet.insert(start_item);

	//Ȼ����һ����ʼ��lalr dfa ��ÿ�����������Щ�Է�������ǰ�����ŵ���
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


//���ݴ������ϵ���������ǰ�����ŵĺ���
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
	//���ȹ���һ��lr0 dfa
	LRDFA lr0;
	DFAState I0;
	int stateNum = 0;

	//���ȹ��������ķ�G'
	//����S'->��S
	LRItem start_item;
	start_item.Label = 1;
	start_item.Symbol = -3;
	//��S'->��S ����I0
	I0.LRItemsSet.insert(start_item);
	lr0_closure(I0.LRItemsSet, productionVec, indexMap, boundT);
	I0.Num = stateNum++;
	//��C��ʼ��ΪCLOUSURE��{ [S'->��S,$]}��
	lr0.statesVec.push_back(I0);

	queue<int> unhandledStates; //���δ����States
	unhandledStates.push(0);//ѹ���ʼ״̬
	map<int, set<LRItem>> newStateMap;//<�ߣ������Ŀ>

	while (unhandledStates.size() != 0)
	{
		//ȡ��δ������
		int top = unhandledStates.front();
		unhandledStates.pop();
		newStateMap.clear();
		//�Ը��������Ŀ��
		gotonext(lr0.statesVec[top].LRItemsSet, newStateMap, productionVec);
		for (auto& p : newStateMap)//���ڵ�ǰ״̬��ÿ������
		{
			//�������ָ��״̬�ıհ�
			lr0_closure(p.second, productionVec, indexMap, boundT);
			//Ȼ�����Ƿ������ͬ��״̬
			int edgeTo = -1;
			//����Ƿ������ͬ��״̬
			//������ǰdfa״̬����
			for (auto& s : lr0.statesVec)
			{
				//����������
				//�������:error1==0 && error2==0
				int error1 = 0, error2 = 0;
				//���飺p����s
				for (auto& item1 : s.LRItemsSet)
				{
					int flag1 = 0;//��¼��ǰitem�Ƿ�����ͬ
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
					//�в����item
					if (flag1 == 0)
					{
						error1 = 1;
						break;
					}
				}
				//���飺s����p
				for (auto& item1 : p.second)
				{
					int flag2 = 0;//��¼��ǰitem�Ƿ�����ͬ
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
					//�в����item
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
				//˵����������ͬ״̬
				//�½�״̬
				DFAState newState;
				edgeTo = stateNum;
				//��push���޸�
				lr0.statesVec.push_back(newState);
				unhandledStates.push(stateNum);
				lr0.statesVec.back().Num = stateNum++;
				lr0.statesVec.back().LRItemsSet = p.second;
			}

			//����Ii->Ij
			lr0.statesVec[top].edgesMap.emplace(p.first, edgeTo);
		}
		//cout << "quick_construct_lalr: construct lr0: state rest:" << unhandledStates.size() << endl;
	}
	//cout << "quick_construct_lalr: construct lr0 dfa finished! " << endl;
	//lr0 dfa �������
	//cout << "quick_construct_lalr: start to construct lr0 core dfa " << endl;
	//Ȼ����lr0���ں������ʾlr0 dfa 
	//��ֻ�ó�ʼ��͵㲻�ڲ���ʽ��˵�������ʾ���
	//ɾ��ÿ��dfa״̬�еķ��ں���
	for (auto& temp_state : lr0.statesVec)
	{
		for (auto& temp_item : temp_state.LRItemsSet)
		{
			//�����ǰ���Ƿ��ں����ôɾ������
			//(����ζ�� ɾ�� �Ȳ��� ��ʼ�� Ҳ���� ���ڲ���ʽ��� ����)
			//������ǳ�ʼ���ҵ��ڲ���ʽ���
			if (temp_item.Label != 1 && temp_item.Dotpos == 0)
			{
				//ɾ�������
				temp_state.LRItemsSet.erase(temp_state.LRItemsSet.find(temp_item));
			}
		}
	}
	//���ڵõ�һ��lr0 �ں�� dfa
	//cout << "quick_construct_lalr: construct lr0 core dfa finished! " << lr0.statesVec.size() << endl;

	//��ʼ��һ���������洫������ǰ�����Ź���
	vector<map<int, vector<pair<int, int>>>> trans_table;
	//��һ���±�Ϊ״̬�ţ������map�ǵ�ǰ��������������ָ�����ļ��� �����map�������Ǳ��������ĵ������ڵ�״̬�ź����־��(����*10+��λ��)��ɵ�pair

	//��ʼ���� ת���� �� �Է�������ǰ������ �Լ� ��ʼ��lalr
	construct_trans_table_and_init_lalr(lr0,lalr_dfa, trans_table,productionVec,indexMap,boundT,firstMap);

	//���ݴ��������lalr
	cal_lalr_core(lalr_dfa, trans_table);

	//Ȼ���ÿ��dfa״̬��հ�

	for (int i = 0; i < lalr_dfa.statesVec.size(); i++)
	{
		closure(lalr_dfa.statesVec[i].LRItemsSet, productionVec, indexMap, boundT, firstMap);
	}
	//������ɣ��õ�lalr dfa
	        
	//cout << lr0.statesVec.size();
}