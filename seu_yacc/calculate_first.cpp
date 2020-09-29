#include"structs.h"
#include"calculate_first.h"
using namespace std;

void firstunion(set<int>& a, const set<int>& b)
{
	a.insert(b.cbegin(), b.cend());
}

//求符号symbol(其实为符号的序号)的first集，递归算法
void first_symbol(set<int> &firstSet, const int &symbol, set<int> &handlingSymbol, 
map<int, set<int>> &firstMap, map<int, pair<int, int>> &indexMap, int &boundT, vector<pair<int, vector<int>>> &productionVec)
{//参数：first集，对应的symbol，记录函数激活路径上出现过的非终结符，防止左递归导致栈溢出；
//映射：symbol->first集，<symbol,first集>；映射：symbol->productionVec的下标。key为symbol，value为<以该symbol为左部的产生式在productionVec中的起始index，数量>；终结符结束序号;int版本产生式，用于编号下标寻址

	//已经求过first集
	if (firstMap.find(symbol) != firstMap.end())
	{
		//已经存在直接合并
		firstunion(firstSet, firstMap[symbol]);
		return;
	}

	if (symbol == -1)
	{
		//符号为空，直接返回
		return;
	}
	else if(symbol<=boundT)
	{//0-boundT为终结符，插入symbol并直接返回
		firstSet.insert(symbol);
		return;
	}
	else
	{//非终结符
		handlingSymbol.insert(symbol);//存储已经出现过的非终结符
		
		auto indexPair = indexMap[symbol];//取得以symbol为左部的产生式
		//遍历所有symbol为左部的产生式
		for (int i = indexPair.first; i < indexPair.first + indexPair.second; ++i)
		{
			auto p = productionVec[i];
			//symbol为空
			if (p.second.size() == 1 && p.second[0] == -1)
			{
				firstSet.insert(-1);
				continue;
			}

			int j = 1;
			set<int> tempset;
			//遍历产生式右部
			while (j <= p.second.size())
			{
				tempset.clear();
				//不能为handlingSymbol，防止左递归
				if (handlingSymbol.find(p.second[j - 1]) != handlingSymbol.end())
				{
					//找到一个，跳过，防止左递归
					break;
				}
				//对于产生式右部求first,递归
				first_symbol(tempset, p.second[j - 1], handlingSymbol, firstMap, indexMap, boundT, productionVec);

				//tempset包含空，但未遍历到最后一项
				if (tempset.find(-1) != tempset.end() && j != p.second.size())
				{
					tempset.erase(-1);//删除空
					firstunion(firstSet, tempset);
					++j;//求下一个的first集
					continue;
				}

				//tempset不包含空,直接并入
				if (tempset.find(-1) == tempset.end())
				{
					firstunion(firstSet, tempset);
					break;
				}

				//tempset包含空，且遍历到最后一项
				if (tempset.find(-1) != tempset.end() && j == p.second.size())
				{
					firstSet.insert(-1);
					break;
				}
			}
		}
	}
}

//求序列symbolVec的first集
void first_string(set<int>& firstSet, const vector<int>& symbols, map<int, set<int>> &firstMap)
{//first集 symbol序列 映射：symbol->first集，<symbol,first集>

	int i = 1;
	//序列为空
	if (symbols.size() == 0)
	{
		//cout << "序列为空" << endl;
		firstSet.insert(-1);
		return;
	}
	while (i <= symbols.size())
	{
		//cout << "symbol为" << symbols[i - 1] << endl;
		auto& temp = firstMap[symbols[i - 1]];

		//temps包含空，但未遍历到最后一项
		if (temp.find(-1) != temp.end() && i != symbols.size())
		{
			//cout << "包含空但是未遍历到最后一项" << endl;
			temp.erase(-1);//删除空
			firstunion(firstSet, temp);
			++i;//求下一个的first集
			continue;
		}

		//temp不包含空,直接并入
		if (temp.find(-1) == temp.end())
		{
			//cout << "不包含空，直接并入" << endl;
			firstunion(firstSet, temp);
			break;
		}

		//temp包含空，且遍历到最后一项
		if (temp.find(-1) != temp.end() && i == symbols.size())
		{
			//cout << "包含空，且遍历到最后一项" << endl;
			firstSet.insert(-1);
			break;
		}
	}
}

//求first集
void first(int &boundN, map<int, set<int>> &firstMap, map<int, pair<int, int>> &indexMap, int &boundT, vector<pair<int, vector<int>>> &productionVec)
{
	set<int> firstSet, stackSet;

	for (int symbol = -1; symbol <= boundN; symbol++)
	{
		firstSet.clear();
		stackSet.clear();
		//求symbol的first集
		first_symbol(firstSet, symbol, stackSet, firstMap, indexMap, boundT, productionVec);
		//将symbol的first集写入firstMap
		firstMap.emplace(symbol, firstSet);
	}
}