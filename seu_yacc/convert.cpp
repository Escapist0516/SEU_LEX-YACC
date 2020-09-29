#include <iostream>
#include <vector>
#include <fstream>
#include <set>
#include <string>
#include"convert.h"
#include"structs.h"
using namespace std;

void convert(vector<pair<string, vector<string>>> &productionStrVec, vector<pair<int, vector<int>>> &productionVec, map<string, int> &tokensMap, vector<string> &tokensVec,
	int &boundT, int &boundN, vector<pair<int, vector<string>>> &precedenceRulesStrVec, vector<pair<int, vector<int>>> &precedenceRulesVec, map<int, pair<int, int>> &indexMap)
{//参数：productionStrVec：字符版本，<产生式左部，右部>；productionVec：int版本，<产生式左部，右部>（右部为序号）；tokensMap：映射：tokens -> tokensVec的序号，<tokens,count>；
//tokensVec：存储所有的token；boundT boundN 将所有的token转换成正整数，char+tokens：0-boundT，非终结符：boundT+1-boundN；precedenceRulesStrVec：前面0表示左，1表示右，后面是规则，字符串版本
//precedenceRulesVec：前面0表示左，1表示右，后面是规则；indexMap：映射：symbol->productionVec的下标。key为symbol，value为<以该symbol为左部的产生式在productionVec中的起始index，数量>
	int count = 0;

	//给字符编号
	//cout << "开始给字符编号" << endl;
	for (auto& p : productionStrVec)
	{
		//在所有的产生式的右部中找字符
		for (auto& item : p.second)
		{
			if (item.size() == 3 && item[0] == '\'' && item[2] == '\'')
			{
				item = item[1];
				//如果在tokensMap中已经找到了这个字符，就不再将其插入tokensMap
				//如果map中没有要查找的数据，他返回迭代器等于end函数返回的迭代器
				if (tokensMap.find(item) != tokensMap.end())
				{
					//找到了,进行下一个
					continue;
				}
				tokensMap.emplace(item, count);
				count++;
			}
		}
	}

	//给token编号
	//cout << "开始给token编号" << endl;
	for (auto& token : tokensVec)
	{
		tokensMap.emplace(token, count);
		count++;
	}
	//终结符的结束位置
	boundT = count - 1;
	//cout << "终结符结束的位置为" << boundT << endl;

	//给非终结符编号
	//cout << "开始给非终结符编号" << endl;
	for (auto& p : productionStrVec)
	{
		if (tokensMap.find(p.first) != tokensMap.end())
		{
			//找到了，下一个
			continue;
		}
		tokensMap.emplace(p.first, count);
		count++;
	}
	//非终结符的结束位置
	boundN = count - 1;
	//cout << "非终结符的位置为" << boundN << endl;

	//构建左右结合运算符优先级列表
	//cout << "构建左右结合运算符优先级列表" << endl;
	for (auto& s : precedenceRulesStrVec)
	{
		//前面0表示左，1表示右，后面是规则，字符串版本
		//vector<pair<int, vector<string>>> precedenceRulesStrVec;
		vector<int> newset;
		//将该规则中的每个token翻译成数字
		for (auto& token : s.second)
		{
			//在tokensmap中寻找该token，存储其序号
			newset.push_back(tokensMap.find(token)->second);
		}
		//前面0表示左，1表示右，后面是规则在tokensmap中的位置
		//vector<pair<int, vector<int>>> precedenceRulesVec;
		precedenceRulesVec.push_back(make_pair(s.first, newset));
	}

	//为了产生式标号从1开始，现在产生式数组中加入一条空白产生式
	productionVec.push_back(pair<int, vector<int>>(-1, vector<int>()));
	//S'->S产生式，产生式右部为第一个非终结符
	productionVec.push_back(pair<int, vector<int>>(-1, vector<int>({ boundT + 1 })));

	//处理产生式
	int proleftnum = boundT + 1;//第一个非终结符的序号
	int counter = 0;
	int templeft;//记录当前处理产生式左部
	vector<int> tempright;//记录当前处理产生式右部
	for (auto& p : productionStrVec)//处理每个产生式
	{
		//处理产生式右部
		tempright.clear();
		for (auto& item : p.second)
		{
			//cout << "产生式右部为：" << tokensMap[item] << endl;
			tempright.push_back(tokensMap[item]);//存的为序号
		}
		//cout << endl;
		//处理产生式左部
		//cout << "产生式左部为：" << tokensMap[p.first] << endl;
		templeft = tokensMap[p.first];
		//如果是新的产生式左部
		if (templeft != proleftnum)
		{
			//cout << "出现新的产生式左部" << endl;
			//记录上一个产生式左部序号，以及在productionVec中的起始位置和长度
			indexMap.emplace(proleftnum, pair<int, int>(productionVec.size() - counter, counter));
			//为记录下一个产生式而将counter清零
			counter = 0;
			//更新当前产生式左部序号。
			proleftnum = templeft;
		}
		//当前产生式加入productionVec
		productionVec.push_back(pair<int, vector<int>>(templeft, tempright));
		//同一个非终结符的产生式数量++
		counter++;
	}
	//记录最后一条产生式左部，以及在productionVec中的起始位置和长度
	indexMap.emplace(proleftnum, pair<int, int>(productionVec.size() - counter, counter));
}