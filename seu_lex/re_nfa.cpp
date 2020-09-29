#include "re_nfa.h"
#include"structs.h"
#include"GlobalData.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <stack>
using namespace std;


//开始规范化正则表达式
//处理引号
void handle_quote(string& pattern)
{
	//处理表达式为"""的情况
	if (pattern == "\"\"\"")
	{
		pattern = "\"";
	}
	//移除pattern字符串中所有的引号，要用两个迭代器作为参数调用erase（）
	//第一个迭代器由remove（）返回，删除指定元素，然后将该元素后面的元素移到前面
	//remove（）返回指向字符串的最有一个有效元素的后面位置
	//第二个迭代器是字符串原始状态的结束迭代器。这两个迭代器指定范围的元素会被删除
	else
	{
		pattern.erase(remove(pattern.begin(), pattern.end(), '"'), pattern.end());
	}
}

//替换{X}，比如{letter}—》[a-zA-Z]
void replace_brace(string& pattern, map<string, string>& head)
{
	string name;//正则表达式名字
	string newPattern;//规范化后的表达式
	if (pattern.length() == 1)
	{
		return;
	}

	for (int i = 0; i < pattern.length(); i++)
	{
		if (pattern[i] == '{')
		{
			//提取表达式名字
			//从第i+1个位置开始找}
			//}的位置是x，那么}前面的位置为x-1，则一共需要的元素个数为（x-1）-（i+1）+1=x-i-1
			name = pattern.substr(i + 1, pattern.find_first_of('}', i + 1) - i - 1);//存储正则表达式的名字，find_first_of返回}的位置x
			//在辅助定义表中找到所需要的表达式
			auto find = head.find(name);//将根据找到的名字找到对应的head
			//构建规范化正则表达式
			newPattern += find->second;
			i = pattern.find_first_of('}', i + 1);
		}
		else
		{
			//处理没有大括号的表达式
			newPattern += pattern[i];
		}
	}
	pattern = newPattern;
}

//替换[X]，比如[a-zA-Z]->（a|b|c|d...|Z）
void replace_square_brackets(string& pattern)
{
	string newPattern;//规范化后的表达式
	if (pattern.length() == 1)
	{
		return;
	}

	for (int i = 0; i < pattern.length(); i++)
	{
		//处理[a-zA-Z]
		if (pattern[i] == '[' && pattern[i + 1] == 'a')
		{
			//将表达式扩展成完整的
			newPattern += '(';
			for (int j = ALLSET.find_first_of('a'); j < ALLSET.find_first_of('Z'); j++)
			{//把a-Y添加
				newPattern += ALLSET[j];
				newPattern += '|';
			}
			newPattern += "Z)";
			i = pattern.find_first_of(']', i + 1);
		}

		//处理[0-9]
		else if (pattern[i] == '[' && pattern[i + 1] == '0')
		{
			newPattern += '(';
			for (int j = ALLSET.find_first_of('0'); j < ALLSET.find_first_of('9'); j++)
			{//把0-8加入
				newPattern += ALLSET[j];
				newPattern += '|';
			}
			newPattern += "9)";
			i = pattern.find_first_of(']', i + 1);
		}

		//处理[+-]
		else if (pattern[i] == '[' && pattern[i + 1] == '+')
		{
			newPattern += "(+|-)";
			i = pattern.find_first_of(']', i + 1);
		}
		else
		{
			newPattern += pattern[i];
		}
	}
	pattern = newPattern;
}

//替换？和+
void replace_question_add(string& pattern)
{
	string newPattern;
	vector<int> left;//存储左括号位置
	for (int i = 0; i < pattern.length(); i++)
	{
		newPattern += pattern[i];
		if (pattern[i] == '(')
		{
			//读到左括号，保存位置
			left.push_back(newPattern.length() - 1);
		}
		else if (pattern[i] == ')' && i + 1 < pattern.length() && pattern[i + 1] == '?')
		{
			//处理x? 问号操作符表示操作数出现0次或一次
			//替换形式为x? -> (@|x)
			//@表示epsilon
			string temp;
			temp += "(@|";
			//将原来表达式中从最后一个左括号开始的表达式内容记录下来，包括左右括号
			temp += newPattern.substr(left.back());
			temp += ')';
			//将原来表达式内最后一个左括号开始的表达式全部删除
			newPattern.erase(left.back(), 100);
			//替换成新的表达式
			newPattern += temp;
			//弹出最后一个左括号位置
			left.pop_back();
			i = i + 1;
		}
		else if (pattern[i] == ')' && i + 1 < pattern.length() && pattern[i + 1] == '+')
		{
			//处理x+ 替换成xx*
			string temp;
			//将表达式中从最后一个左括号开始的表达式内容记录下来，包括左右括号
			temp += newPattern.substr(left.back());
			temp += newPattern.substr(left.back());
			temp += '*';
			//将原来表达式内最后一个左括号开始的表达式全部删除
			newPattern.erase(left.back(), 100);
			//替换成新的表达式
			newPattern += temp;
			//弹出最后一个左括号位置
			left.pop_back();
			i = i + 1;
		}
	}
	pattern = newPattern;
}

//加$，如果void 则变成v$o$i$d,分割字符串，便于转换成NFA
void add_dot(string& pattern)
{
	string newPattern;
	for (int i = 0; i < pattern.length(); i++)
	{
		newPattern += pattern[i];
		if (i == pattern.length() - 1)
			continue;//最后一个字符，不加
		else if (pattern[i] == '(')
			continue;//当前字符为（，不加
		else if (i + 1 < pattern.length() && pattern[i] == '|' && pattern[i + 1] == '|')
			newPattern += '$';//||情况需要加
		else if (i + 1 < pattern.length() && pattern[i] == '|' && pattern[i + 1] != '|')
			continue;//字符为|，而不是||,不加
		else if (i + 1 < pattern.length() && pattern[i] == '/' && pattern[i + 1] == '*')
			newPattern += '$';// /*情况需要加
		else if (i + 1 < pattern.length() && (pattern[i + 1] == '|' || pattern[i + 1] == ')' || pattern[i + 1] == '*'))
			continue; //其余不加情况，下一个字符为操作符或右括号
		else
			newPattern += '$';//加点
	}

	pattern = newPattern;
}

//处理head的value 处理rules
void re_standardize(vector<Rules>& Rules, map<string, string>& head)
{
	//先处理辅助定义map里的右部
	for (auto& map : head)
	{
		//替换大括号
		replace_brace(map.second, head);
		//替换中括号
		replace_square_brackets(map.second);
		//处理问号和加号
		replace_question_add(map.second);
	}

	//再处理表达式vector里的左部
	for (auto& rule : Rules)
	{
		//处理引号
		handle_quote(rule.pattern);
		//替换大括号，利用{}内的部分去定义map里找，此时该map里的内容已被替换
		replace_brace(rule.pattern, head);
		//加$
		add_dot(rule.pattern);
	}
}
//规范化正则表达式结束

//开始将规范化正则表达式转后缀表达式 得到后缀表达式规则
void re_to_postfix(vector<Rules>& rules)
{
	for (auto& rule : rules)
	{
		string pattern = rule.pattern;//对pattern进行操作
		string temp;//用于临时装载正确pattern
		queue<char> result;//最终正确的队列
		stack<char> s;//操作符栈

		if (pattern.length() == 1)
			continue;
		for (int i = 0; i < pattern.length(); i++)
		{
			char currentrent = pattern[i];
			//如果遇到左括号，将其放入栈中
			if (currentrent == '(')
			{
				s.push(currentrent);
			}
			//如果遇到右括号，则将栈顶元素弹出，将弹出的操作符输出直到左括号为止
			//注意，左括号只弹出不输出
			else if (currentrent == ')')
			{
				while (!s.empty() && s.top() != '(')
				{
					result.push(s.top());
					s.pop();
				}
				s.pop();//弹出左括号
			}
			else if (currentrent == '|')
			{
				//如果是|$|情况，作为操作符压栈
				if (pattern[i + 1] == '$' && pattern[i + 2] == '|')
				{

					s.push('$');
					s.push('|');
					s.push('|');
					i = i + 2;
					continue;
				}

				//如果遇到任何其他的操作符，从栈中弹出元素直到遇到发现更低优先级的元素(或者栈为空)为止
				//弹出完这些元素后，才将遇到的操作符压入到栈中
				//优先级顺序：*>$>|
				while (!s.empty() && (s.top() == '$' || s.top() == '|' || s.top() == '*'))
				{
					result.push(s.top());
					s.pop();
				}
				s.push(currentrent);
			}
			else if (currentrent == '$')
			{
				//栈顶操作符优先级不低于当前操作符优先级，从栈中弹出这些元素，再将当前操作符压栈
				while (!s.empty() && (s.top() == '$' || s.top() == '*'))
				{
					result.push(s.top());
					s.pop();
				}
				s.push(currentrent);
			}
			else
			{
				//如果遇到操作数，直接输出
				result.push(currentrent);
			}
		}

		//栈中还有操作符，直接出栈
		while (!s.empty())
		{
			result.push(s.top());
			s.pop();
		}
		while (!result.empty())
		{
			temp += result.front();
			result.pop();
		}
		rule.pattern = temp;
	}
}

//将后缀表达式转换为nfa
int num = 0;
stack<NFA> postfix_to_nfa(vector<Rules>& postfixRules)
{//后缀表达式规则 最终要得到的NFA
	stack<NFA> NFAstack;//存储NFA的栈
	for (int i = 0; i < postfixRules.size(); i++)
	{
		string pattern = postfixRules[i].pattern;
		int tempActionInt = -1;
		for (int j = 0; j < pattern.length(); j++)
		{
			NFA up, down;
			NFAstate start, end;
			char current = pattern[j];
			if (current == '|' && j != 0 && j != 1)
			{
				//是或|操作，但不属于||'这个表达式
				//取出最上面的NFA进行操作
				up = NFAstack.top();
				NFAstack.pop();
				down = NFAstack.top();
				NFAstack.pop();
				//新建两个状态
				start.num = num;
				++num;
				end.num = num;
				++num;//状态编号增加
				//将down的终态和up的终态都连接到end
				up.statesMap.find(up.endStatesMap.begin()->first)->second//得到终态
					//在down的标号对应状态的根据终态标号寻找终态，得到该状态的内部状态结构
					//将压在下面的（也就是更前面的）NFA用@与后面的进行连接
					.EdgeMultiMap.insert(pair<char, int>('@', end.num));//在up的状态里找终态的标号，找到状态，根据value
				down.statesMap.find(down.endStatesMap.begin()->first)->second
					.EdgeMultiMap.insert(pair<char, int>('@', end.num));
				//将start连接到down和up的初态
				start.EdgeMultiMap.insert(pair<char, int>('@', up.startState));
				start.EdgeMultiMap.insert(pair<char, int>('@', down.startState));
				//连接关系定义好后存入map
				down.statesMap.insert(pair<int, NFAstate>(start.num, start));
				down.statesMap.insert(pair<int, NFAstate>(end.num, end));
				//将up的stateMap拷贝到down中
				down.statesMap.insert(up.statesMap.begin(), up.statesMap.end());
				//修改down的初态
				down.startState = start.num;
				//修改down的终态
				down.endStatesMap.clear();
				down.endStatesMap.insert(pair<int, int>(end.num, tempActionInt));
				NFAstack.push(down);
			}
			else if (j - 1 > 0 && j + 1 < pattern.length() && current == '*' && pattern.length() != 1 && pattern[j - 1] != '/' && pattern[j + 1] != '=')
			{
				//取出最上面的NFA进行操作
				up = NFAstack.top();
				NFAstack.pop();
				//新建两个状态
				start.num = num;
				++num;
				end.num = num;
				++num;
				//将start和up初态连接
				start.EdgeMultiMap.insert(pair<char, int>('@', up.startState));
				//将start和end连接
				start.EdgeMultiMap.insert(pair<char, int>('@', end.num));
				//将up的终态和初态连接
				up.statesMap.find(up.endStatesMap.begin()->first)->second.
					EdgeMultiMap.insert(pair<char, int>('@', up.startState));
				up.statesMap.find(up.endStatesMap.begin()->first)->second.
					EdgeMultiMap.insert(pair<char, int>('@', end.num));
				//更改初态
				up.startState = start.num;
				//连接关系定义好后存入map
				up.statesMap.insert(pair<int, NFAstate>(start.num, start));
				up.statesMap.insert(pair<int, NFAstate>(end.num, end));
				//更改终态
				up.endStatesMap.clear();
				up.endStatesMap.insert(pair<int, int>(end.num, tempActionInt));
				NFAstack.push(up);
			}
			else if (current == '$')
			{
				//例如：0-v-1 2-o-3 遇到$将两个NFA相连
				//则up中为2-0-3 down中为0-v-1 
				//取出0-v-1的终态，将他终态的下一个状态设置为2-o-3，再将0-v-1的终态删除设置为2-o-3的终态
				//字符分割符，取出栈顶两个NFA将两个NFA相连
				//取出栈顶的两个NFA
				up = NFAstack.top();
				NFAstack.pop();
				down = NFAstack.top();
				NFAstack.pop();
				//取出down的终态
				down.statesMap.find(down.endStatesMap.begin()->first)->second //得到终态
					//将压在下面的（也就是更前面的）NFA用@与后面的进行连接
					.EdgeMultiMap.insert(pair<char, int>('@', up.startState));
				//更改终态
				down.endStatesMap.clear();
				down.endStatesMap = up.endStatesMap;
				//把up的状态map拷贝到down中，
				down.statesMap.insert(up.statesMap.begin(), up.statesMap.end());
				NFAstack.push(down);
			}
			else
			{
				//遇到字符，构造一个NFA，压栈
				NFA push; //用于压栈的NFA
				//新建一个起始状态
				start.num = num;
				//更改初态
				push.startState = start.num;
				++num;
				//新建一个终止状态
				end.num = num;
				++num;
				//将起始状态和终止状态连接
				start.EdgeMultiMap.insert(pair<char, int>(current, end.num));
				//连接关系定义好后存入map
				push.statesMap.insert(pair<int, NFAstate>(start.num, start));
				push.statesMap.insert(pair<int, NFAstate>(end.num, end));
				//标识当前终态，因此先传入空vector。
				push.endStatesMap.insert(pair<int, int>(end.num, tempActionInt));
				//压栈
				NFAstack.push(push);
			}
		}
		//将action赋给栈顶的NFA的终态
		NFAstack.top().endStatesMap.begin()->second = i;//终态存储动作，根据存储的rules的序号，在rules数组中寻找相应的动作
	}

	return NFAstack;
}
//将后缀表达式转化为NFA结束

//将NFA组合成一个大的NFA
void nfa_combination(stack<NFA> NFAstack, NFA& finalNFA)
{
	//cout << "\n" << NFAstack.size() << endl;
	//得到NFA栈，开始合并
	finalNFA = NFAstack.top();
	NFA down;
	NFAstack.pop();
	while (!NFAstack.empty())
	{
		NFAstate start;
		//依次，把栈顶NFA与大NFA合并
		down = NFAstack.top();
		NFAstack.pop();
		//新建一个start状态
		start.num = num;
		num++;
		//将start连接到finalNFA和dowNFA的初态
		start.EdgeMultiMap.insert(pair<char, int>('@', finalNFA.startState));
		start.EdgeMultiMap.insert(pair<char, int>('@', down.startState));
		//修改finalNFA的起始状态
		finalNFA.startState = start.num;
		finalNFA.statesMap.insert(pair<int, NFAstate>(start.num, start));
		//添加finalNFA的终止状态
		finalNFA.endStatesMap.insert(down.endStatesMap.begin(), down.endStatesMap.end());
		//把down的状态map拷贝到finalNFA中
		finalNFA.statesMap.insert(down.statesMap.begin(), down.statesMap.end());
	}
}
