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


//��ʼ�淶��������ʽ
//��������
void handle_quote(string& pattern)
{
	//������ʽΪ"""�����
	if (pattern == "\"\"\"")
	{
		pattern = "\"";
	}
	//�Ƴ�pattern�ַ��������е����ţ�Ҫ��������������Ϊ��������erase����
	//��һ����������remove�������أ�ɾ��ָ��Ԫ�أ�Ȼ�󽫸�Ԫ�غ����Ԫ���Ƶ�ǰ��
	//remove��������ָ���ַ���������һ����ЧԪ�صĺ���λ��
	//�ڶ������������ַ���ԭʼ״̬�Ľ�����������������������ָ����Χ��Ԫ�ػᱻɾ��
	else
	{
		pattern.erase(remove(pattern.begin(), pattern.end(), '"'), pattern.end());
	}
}

//�滻{X}������{letter}����[a-zA-Z]
void replace_brace(string& pattern, map<string, string>& head)
{
	string name;//������ʽ����
	string newPattern;//�淶����ı��ʽ
	if (pattern.length() == 1)
	{
		return;
	}

	for (int i = 0; i < pattern.length(); i++)
	{
		if (pattern[i] == '{')
		{
			//��ȡ���ʽ����
			//�ӵ�i+1��λ�ÿ�ʼ��}
			//}��λ����x����ô}ǰ���λ��Ϊx-1����һ����Ҫ��Ԫ�ظ���Ϊ��x-1��-��i+1��+1=x-i-1
			name = pattern.substr(i + 1, pattern.find_first_of('}', i + 1) - i - 1);//�洢������ʽ�����֣�find_first_of����}��λ��x
			//�ڸ�����������ҵ�����Ҫ�ı��ʽ
			auto find = head.find(name);//�������ҵ��������ҵ���Ӧ��head
			//�����淶��������ʽ
			newPattern += find->second;
			i = pattern.find_first_of('}', i + 1);
		}
		else
		{
			//����û�д����ŵı��ʽ
			newPattern += pattern[i];
		}
	}
	pattern = newPattern;
}

//�滻[X]������[a-zA-Z]->��a|b|c|d...|Z��
void replace_square_brackets(string& pattern)
{
	string newPattern;//�淶����ı��ʽ
	if (pattern.length() == 1)
	{
		return;
	}

	for (int i = 0; i < pattern.length(); i++)
	{
		//����[a-zA-Z]
		if (pattern[i] == '[' && pattern[i + 1] == 'a')
		{
			//�����ʽ��չ��������
			newPattern += '(';
			for (int j = ALLSET.find_first_of('a'); j < ALLSET.find_first_of('Z'); j++)
			{//��a-Y���
				newPattern += ALLSET[j];
				newPattern += '|';
			}
			newPattern += "Z)";
			i = pattern.find_first_of(']', i + 1);
		}

		//����[0-9]
		else if (pattern[i] == '[' && pattern[i + 1] == '0')
		{
			newPattern += '(';
			for (int j = ALLSET.find_first_of('0'); j < ALLSET.find_first_of('9'); j++)
			{//��0-8����
				newPattern += ALLSET[j];
				newPattern += '|';
			}
			newPattern += "9)";
			i = pattern.find_first_of(']', i + 1);
		}

		//����[+-]
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

//�滻����+
void replace_question_add(string& pattern)
{
	string newPattern;
	vector<int> left;//�洢������λ��
	for (int i = 0; i < pattern.length(); i++)
	{
		newPattern += pattern[i];
		if (pattern[i] == '(')
		{
			//���������ţ�����λ��
			left.push_back(newPattern.length() - 1);
		}
		else if (pattern[i] == ')' && i + 1 < pattern.length() && pattern[i + 1] == '?')
		{
			//����x? �ʺŲ�������ʾ����������0�λ�һ��
			//�滻��ʽΪx? -> (@|x)
			//@��ʾepsilon
			string temp;
			temp += "(@|";
			//��ԭ�����ʽ�д����һ�������ſ�ʼ�ı��ʽ���ݼ�¼������������������
			temp += newPattern.substr(left.back());
			temp += ')';
			//��ԭ�����ʽ�����һ�������ſ�ʼ�ı��ʽȫ��ɾ��
			newPattern.erase(left.back(), 100);
			//�滻���µı��ʽ
			newPattern += temp;
			//�������һ��������λ��
			left.pop_back();
			i = i + 1;
		}
		else if (pattern[i] == ')' && i + 1 < pattern.length() && pattern[i + 1] == '+')
		{
			//����x+ �滻��xx*
			string temp;
			//�����ʽ�д����һ�������ſ�ʼ�ı��ʽ���ݼ�¼������������������
			temp += newPattern.substr(left.back());
			temp += newPattern.substr(left.back());
			temp += '*';
			//��ԭ�����ʽ�����һ�������ſ�ʼ�ı��ʽȫ��ɾ��
			newPattern.erase(left.back(), 100);
			//�滻���µı��ʽ
			newPattern += temp;
			//�������һ��������λ��
			left.pop_back();
			i = i + 1;
		}
	}
	pattern = newPattern;
}

//��$�����void ����v$o$i$d,�ָ��ַ���������ת����NFA
void add_dot(string& pattern)
{
	string newPattern;
	for (int i = 0; i < pattern.length(); i++)
	{
		newPattern += pattern[i];
		if (i == pattern.length() - 1)
			continue;//���һ���ַ�������
		else if (pattern[i] == '(')
			continue;//��ǰ�ַ�Ϊ��������
		else if (i + 1 < pattern.length() && pattern[i] == '|' && pattern[i + 1] == '|')
			newPattern += '$';//||�����Ҫ��
		else if (i + 1 < pattern.length() && pattern[i] == '|' && pattern[i + 1] != '|')
			continue;//�ַ�Ϊ|��������||,����
		else if (i + 1 < pattern.length() && pattern[i] == '/' && pattern[i + 1] == '*')
			newPattern += '$';// /*�����Ҫ��
		else if (i + 1 < pattern.length() && (pattern[i + 1] == '|' || pattern[i + 1] == ')' || pattern[i + 1] == '*'))
			continue; //���಻���������һ���ַ�Ϊ��������������
		else
			newPattern += '$';//�ӵ�
	}

	pattern = newPattern;
}

//����head��value ����rules
void re_standardize(vector<Rules>& Rules, map<string, string>& head)
{
	//�ȴ���������map����Ҳ�
	for (auto& map : head)
	{
		//�滻������
		replace_brace(map.second, head);
		//�滻������
		replace_square_brackets(map.second);
		//�����ʺźͼӺ�
		replace_question_add(map.second);
	}

	//�ٴ�����ʽvector�����
	for (auto& rule : Rules)
	{
		//��������
		handle_quote(rule.pattern);
		//�滻�����ţ�����{}�ڵĲ���ȥ����map���ң���ʱ��map��������ѱ��滻
		replace_brace(rule.pattern, head);
		//��$
		add_dot(rule.pattern);
	}
}
//�淶��������ʽ����

//��ʼ���淶��������ʽת��׺���ʽ �õ���׺���ʽ����
void re_to_postfix(vector<Rules>& rules)
{
	for (auto& rule : rules)
	{
		string pattern = rule.pattern;//��pattern���в���
		string temp;//������ʱװ����ȷpattern
		queue<char> result;//������ȷ�Ķ���
		stack<char> s;//������ջ

		if (pattern.length() == 1)
			continue;
		for (int i = 0; i < pattern.length(); i++)
		{
			char currentrent = pattern[i];
			//������������ţ��������ջ��
			if (currentrent == '(')
			{
				s.push(currentrent);
			}
			//������������ţ���ջ��Ԫ�ص������������Ĳ��������ֱ��������Ϊֹ
			//ע�⣬������ֻ���������
			else if (currentrent == ')')
			{
				while (!s.empty() && s.top() != '(')
				{
					result.push(s.top());
					s.pop();
				}
				s.pop();//����������
			}
			else if (currentrent == '|')
			{
				//�����|$|�������Ϊ������ѹջ
				if (pattern[i + 1] == '$' && pattern[i + 2] == '|')
				{

					s.push('$');
					s.push('|');
					s.push('|');
					i = i + 2;
					continue;
				}

				//��������κ������Ĳ���������ջ�е���Ԫ��ֱ���������ָ������ȼ���Ԫ��(����ջΪ��)Ϊֹ
				//��������ЩԪ�غ󣬲Ž������Ĳ�����ѹ�뵽ջ��
				//���ȼ�˳��*>$>|
				while (!s.empty() && (s.top() == '$' || s.top() == '|' || s.top() == '*'))
				{
					result.push(s.top());
					s.pop();
				}
				s.push(currentrent);
			}
			else if (currentrent == '$')
			{
				//ջ�����������ȼ������ڵ�ǰ���������ȼ�����ջ�е�����ЩԪ�أ��ٽ���ǰ������ѹջ
				while (!s.empty() && (s.top() == '$' || s.top() == '*'))
				{
					result.push(s.top());
					s.pop();
				}
				s.push(currentrent);
			}
			else
			{
				//���������������ֱ�����
				result.push(currentrent);
			}
		}

		//ջ�л��в�������ֱ�ӳ�ջ
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

//����׺���ʽת��Ϊnfa
int num = 0;
stack<NFA> postfix_to_nfa(vector<Rules>& postfixRules)
{//��׺���ʽ���� ����Ҫ�õ���NFA
	stack<NFA> NFAstack;//�洢NFA��ջ
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
				//�ǻ�|��������������||'������ʽ
				//ȡ���������NFA���в���
				up = NFAstack.top();
				NFAstack.pop();
				down = NFAstack.top();
				NFAstack.pop();
				//�½�����״̬
				start.num = num;
				++num;
				end.num = num;
				++num;//״̬�������
				//��down����̬��up����̬�����ӵ�end
				up.statesMap.find(up.endStatesMap.begin()->first)->second//�õ���̬
					//��down�ı�Ŷ�Ӧ״̬�ĸ�����̬���Ѱ����̬���õ���״̬���ڲ�״̬�ṹ
					//��ѹ������ģ�Ҳ���Ǹ�ǰ��ģ�NFA��@�����Ľ�������
					.EdgeMultiMap.insert(pair<char, int>('@', end.num));//��up��״̬������̬�ı�ţ��ҵ�״̬������value
				down.statesMap.find(down.endStatesMap.begin()->first)->second
					.EdgeMultiMap.insert(pair<char, int>('@', end.num));
				//��start���ӵ�down��up�ĳ�̬
				start.EdgeMultiMap.insert(pair<char, int>('@', up.startState));
				start.EdgeMultiMap.insert(pair<char, int>('@', down.startState));
				//���ӹ�ϵ����ú����map
				down.statesMap.insert(pair<int, NFAstate>(start.num, start));
				down.statesMap.insert(pair<int, NFAstate>(end.num, end));
				//��up��stateMap������down��
				down.statesMap.insert(up.statesMap.begin(), up.statesMap.end());
				//�޸�down�ĳ�̬
				down.startState = start.num;
				//�޸�down����̬
				down.endStatesMap.clear();
				down.endStatesMap.insert(pair<int, int>(end.num, tempActionInt));
				NFAstack.push(down);
			}
			else if (j - 1 > 0 && j + 1 < pattern.length() && current == '*' && pattern.length() != 1 && pattern[j - 1] != '/' && pattern[j + 1] != '=')
			{
				//ȡ���������NFA���в���
				up = NFAstack.top();
				NFAstack.pop();
				//�½�����״̬
				start.num = num;
				++num;
				end.num = num;
				++num;
				//��start��up��̬����
				start.EdgeMultiMap.insert(pair<char, int>('@', up.startState));
				//��start��end����
				start.EdgeMultiMap.insert(pair<char, int>('@', end.num));
				//��up����̬�ͳ�̬����
				up.statesMap.find(up.endStatesMap.begin()->first)->second.
					EdgeMultiMap.insert(pair<char, int>('@', up.startState));
				up.statesMap.find(up.endStatesMap.begin()->first)->second.
					EdgeMultiMap.insert(pair<char, int>('@', end.num));
				//���ĳ�̬
				up.startState = start.num;
				//���ӹ�ϵ����ú����map
				up.statesMap.insert(pair<int, NFAstate>(start.num, start));
				up.statesMap.insert(pair<int, NFAstate>(end.num, end));
				//������̬
				up.endStatesMap.clear();
				up.endStatesMap.insert(pair<int, int>(end.num, tempActionInt));
				NFAstack.push(up);
			}
			else if (current == '$')
			{
				//���磺0-v-1 2-o-3 ����$������NFA����
				//��up��Ϊ2-0-3 down��Ϊ0-v-1 
				//ȡ��0-v-1����̬��������̬����һ��״̬����Ϊ2-o-3���ٽ�0-v-1����̬ɾ������Ϊ2-o-3����̬
				//�ַ��ָ����ȡ��ջ������NFA������NFA����
				//ȡ��ջ��������NFA
				up = NFAstack.top();
				NFAstack.pop();
				down = NFAstack.top();
				NFAstack.pop();
				//ȡ��down����̬
				down.statesMap.find(down.endStatesMap.begin()->first)->second //�õ���̬
					//��ѹ������ģ�Ҳ���Ǹ�ǰ��ģ�NFA��@�����Ľ�������
					.EdgeMultiMap.insert(pair<char, int>('@', up.startState));
				//������̬
				down.endStatesMap.clear();
				down.endStatesMap = up.endStatesMap;
				//��up��״̬map������down�У�
				down.statesMap.insert(up.statesMap.begin(), up.statesMap.end());
				NFAstack.push(down);
			}
			else
			{
				//�����ַ�������һ��NFA��ѹջ
				NFA push; //����ѹջ��NFA
				//�½�һ����ʼ״̬
				start.num = num;
				//���ĳ�̬
				push.startState = start.num;
				++num;
				//�½�һ����ֹ״̬
				end.num = num;
				++num;
				//����ʼ״̬����ֹ״̬����
				start.EdgeMultiMap.insert(pair<char, int>(current, end.num));
				//���ӹ�ϵ����ú����map
				push.statesMap.insert(pair<int, NFAstate>(start.num, start));
				push.statesMap.insert(pair<int, NFAstate>(end.num, end));
				//��ʶ��ǰ��̬������ȴ����vector��
				push.endStatesMap.insert(pair<int, int>(end.num, tempActionInt));
				//ѹջ
				NFAstack.push(push);
			}
		}
		//��action����ջ����NFA����̬
		NFAstack.top().endStatesMap.begin()->second = i;//��̬�洢���������ݴ洢��rules����ţ���rules������Ѱ����Ӧ�Ķ���
	}

	return NFAstack;
}
//����׺���ʽת��ΪNFA����

//��NFA��ϳ�һ�����NFA
void nfa_combination(stack<NFA> NFAstack, NFA& finalNFA)
{
	//cout << "\n" << NFAstack.size() << endl;
	//�õ�NFAջ����ʼ�ϲ�
	finalNFA = NFAstack.top();
	NFA down;
	NFAstack.pop();
	while (!NFAstack.empty())
	{
		NFAstate start;
		//���Σ���ջ��NFA���NFA�ϲ�
		down = NFAstack.top();
		NFAstack.pop();
		//�½�һ��start״̬
		start.num = num;
		num++;
		//��start���ӵ�finalNFA��dowNFA�ĳ�̬
		start.EdgeMultiMap.insert(pair<char, int>('@', finalNFA.startState));
		start.EdgeMultiMap.insert(pair<char, int>('@', down.startState));
		//�޸�finalNFA����ʼ״̬
		finalNFA.startState = start.num;
		finalNFA.statesMap.insert(pair<int, NFAstate>(start.num, start));
		//���finalNFA����ֹ״̬
		finalNFA.endStatesMap.insert(down.endStatesMap.begin(), down.endStatesMap.end());
		//��down��״̬map������finalNFA��
		finalNFA.statesMap.insert(down.statesMap.begin(), down.statesMap.end());
	}
}
