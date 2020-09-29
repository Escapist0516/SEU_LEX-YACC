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
{//������productionStrVec���ַ��汾��<����ʽ�󲿣��Ҳ�>��productionVec��int�汾��<����ʽ�󲿣��Ҳ�>���Ҳ�Ϊ��ţ���tokensMap��ӳ�䣺tokens -> tokensVec����ţ�<tokens,count>��
//tokensVec���洢���е�token��boundT boundN �����е�tokenת������������char+tokens��0-boundT�����ս����boundT+1-boundN��precedenceRulesStrVec��ǰ��0��ʾ��1��ʾ�ң������ǹ����ַ����汾
//precedenceRulesVec��ǰ��0��ʾ��1��ʾ�ң������ǹ���indexMap��ӳ�䣺symbol->productionVec���±ꡣkeyΪsymbol��valueΪ<�Ը�symbolΪ�󲿵Ĳ���ʽ��productionVec�е���ʼindex������>
	int count = 0;

	//���ַ����
	//cout << "��ʼ���ַ����" << endl;
	for (auto& p : productionStrVec)
	{
		//�����еĲ���ʽ���Ҳ������ַ�
		for (auto& item : p.second)
		{
			if (item.size() == 3 && item[0] == '\'' && item[2] == '\'')
			{
				item = item[1];
				//�����tokensMap���Ѿ��ҵ�������ַ����Ͳ��ٽ������tokensMap
				//���map��û��Ҫ���ҵ����ݣ������ص���������end�������صĵ�����
				if (tokensMap.find(item) != tokensMap.end())
				{
					//�ҵ���,������һ��
					continue;
				}
				tokensMap.emplace(item, count);
				count++;
			}
		}
	}

	//��token���
	//cout << "��ʼ��token���" << endl;
	for (auto& token : tokensVec)
	{
		tokensMap.emplace(token, count);
		count++;
	}
	//�ս���Ľ���λ��
	boundT = count - 1;
	//cout << "�ս��������λ��Ϊ" << boundT << endl;

	//�����ս�����
	//cout << "��ʼ�����ս�����" << endl;
	for (auto& p : productionStrVec)
	{
		if (tokensMap.find(p.first) != tokensMap.end())
		{
			//�ҵ��ˣ���һ��
			continue;
		}
		tokensMap.emplace(p.first, count);
		count++;
	}
	//���ս���Ľ���λ��
	boundN = count - 1;
	//cout << "���ս����λ��Ϊ" << boundN << endl;

	//�������ҽ����������ȼ��б�
	//cout << "�������ҽ����������ȼ��б�" << endl;
	for (auto& s : precedenceRulesStrVec)
	{
		//ǰ��0��ʾ��1��ʾ�ң������ǹ����ַ����汾
		//vector<pair<int, vector<string>>> precedenceRulesStrVec;
		vector<int> newset;
		//���ù����е�ÿ��token���������
		for (auto& token : s.second)
		{
			//��tokensmap��Ѱ�Ҹ�token���洢�����
			newset.push_back(tokensMap.find(token)->second);
		}
		//ǰ��0��ʾ��1��ʾ�ң������ǹ�����tokensmap�е�λ��
		//vector<pair<int, vector<int>>> precedenceRulesVec;
		precedenceRulesVec.push_back(make_pair(s.first, newset));
	}

	//Ϊ�˲���ʽ��Ŵ�1��ʼ�����ڲ���ʽ�����м���һ���հײ���ʽ
	productionVec.push_back(pair<int, vector<int>>(-1, vector<int>()));
	//S'->S����ʽ������ʽ�Ҳ�Ϊ��һ�����ս��
	productionVec.push_back(pair<int, vector<int>>(-1, vector<int>({ boundT + 1 })));

	//�������ʽ
	int proleftnum = boundT + 1;//��һ�����ս�������
	int counter = 0;
	int templeft;//��¼��ǰ�������ʽ��
	vector<int> tempright;//��¼��ǰ�������ʽ�Ҳ�
	for (auto& p : productionStrVec)//����ÿ������ʽ
	{
		//�������ʽ�Ҳ�
		tempright.clear();
		for (auto& item : p.second)
		{
			//cout << "����ʽ�Ҳ�Ϊ��" << tokensMap[item] << endl;
			tempright.push_back(tokensMap[item]);//���Ϊ���
		}
		//cout << endl;
		//�������ʽ��
		//cout << "����ʽ��Ϊ��" << tokensMap[p.first] << endl;
		templeft = tokensMap[p.first];
		//������µĲ���ʽ��
		if (templeft != proleftnum)
		{
			//cout << "�����µĲ���ʽ��" << endl;
			//��¼��һ������ʽ����ţ��Լ���productionVec�е���ʼλ�úͳ���
			indexMap.emplace(proleftnum, pair<int, int>(productionVec.size() - counter, counter));
			//Ϊ��¼��һ������ʽ����counter����
			counter = 0;
			//���µ�ǰ����ʽ����š�
			proleftnum = templeft;
		}
		//��ǰ����ʽ����productionVec
		productionVec.push_back(pair<int, vector<int>>(templeft, tempright));
		//ͬһ�����ս���Ĳ���ʽ����++
		counter++;
	}
	//��¼���һ������ʽ�󲿣��Լ���productionVec�е���ʼλ�úͳ���
	indexMap.emplace(proleftnum, pair<int, int>(productionVec.size() - counter, counter));
}