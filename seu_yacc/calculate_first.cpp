#include"structs.h"
#include"calculate_first.h"
using namespace std;

void firstunion(set<int>& a, const set<int>& b)
{
	a.insert(b.cbegin(), b.cend());
}

//�����symbol(��ʵΪ���ŵ����)��first�����ݹ��㷨
void first_symbol(set<int> &firstSet, const int &symbol, set<int> &handlingSymbol, 
map<int, set<int>> &firstMap, map<int, pair<int, int>> &indexMap, int &boundT, vector<pair<int, vector<int>>> &productionVec)
{//������first������Ӧ��symbol����¼��������·���ϳ��ֹ��ķ��ս������ֹ��ݹ鵼��ջ�����
//ӳ�䣺symbol->first����<symbol,first��>��ӳ�䣺symbol->productionVec���±ꡣkeyΪsymbol��valueΪ<�Ը�symbolΪ�󲿵Ĳ���ʽ��productionVec�е���ʼindex������>���ս���������;int�汾����ʽ�����ڱ���±�Ѱַ

	//�Ѿ����first��
	if (firstMap.find(symbol) != firstMap.end())
	{
		//�Ѿ�����ֱ�Ӻϲ�
		firstunion(firstSet, firstMap[symbol]);
		return;
	}

	if (symbol == -1)
	{
		//����Ϊ�գ�ֱ�ӷ���
		return;
	}
	else if(symbol<=boundT)
	{//0-boundTΪ�ս��������symbol��ֱ�ӷ���
		firstSet.insert(symbol);
		return;
	}
	else
	{//���ս��
		handlingSymbol.insert(symbol);//�洢�Ѿ����ֹ��ķ��ս��
		
		auto indexPair = indexMap[symbol];//ȡ����symbolΪ�󲿵Ĳ���ʽ
		//��������symbolΪ�󲿵Ĳ���ʽ
		for (int i = indexPair.first; i < indexPair.first + indexPair.second; ++i)
		{
			auto p = productionVec[i];
			//symbolΪ��
			if (p.second.size() == 1 && p.second[0] == -1)
			{
				firstSet.insert(-1);
				continue;
			}

			int j = 1;
			set<int> tempset;
			//��������ʽ�Ҳ�
			while (j <= p.second.size())
			{
				tempset.clear();
				//����ΪhandlingSymbol����ֹ��ݹ�
				if (handlingSymbol.find(p.second[j - 1]) != handlingSymbol.end())
				{
					//�ҵ�һ������������ֹ��ݹ�
					break;
				}
				//���ڲ���ʽ�Ҳ���first,�ݹ�
				first_symbol(tempset, p.second[j - 1], handlingSymbol, firstMap, indexMap, boundT, productionVec);

				//tempset�����գ���δ���������һ��
				if (tempset.find(-1) != tempset.end() && j != p.second.size())
				{
					tempset.erase(-1);//ɾ����
					firstunion(firstSet, tempset);
					++j;//����һ����first��
					continue;
				}

				//tempset��������,ֱ�Ӳ���
				if (tempset.find(-1) == tempset.end())
				{
					firstunion(firstSet, tempset);
					break;
				}

				//tempset�����գ��ұ��������һ��
				if (tempset.find(-1) != tempset.end() && j == p.second.size())
				{
					firstSet.insert(-1);
					break;
				}
			}
		}
	}
}

//������symbolVec��first��
void first_string(set<int>& firstSet, const vector<int>& symbols, map<int, set<int>> &firstMap)
{//first�� symbol���� ӳ�䣺symbol->first����<symbol,first��>

	int i = 1;
	//����Ϊ��
	if (symbols.size() == 0)
	{
		//cout << "����Ϊ��" << endl;
		firstSet.insert(-1);
		return;
	}
	while (i <= symbols.size())
	{
		//cout << "symbolΪ" << symbols[i - 1] << endl;
		auto& temp = firstMap[symbols[i - 1]];

		//temps�����գ���δ���������һ��
		if (temp.find(-1) != temp.end() && i != symbols.size())
		{
			//cout << "�����յ���δ���������һ��" << endl;
			temp.erase(-1);//ɾ����
			firstunion(firstSet, temp);
			++i;//����һ����first��
			continue;
		}

		//temp��������,ֱ�Ӳ���
		if (temp.find(-1) == temp.end())
		{
			//cout << "�������գ�ֱ�Ӳ���" << endl;
			firstunion(firstSet, temp);
			break;
		}

		//temp�����գ��ұ��������һ��
		if (temp.find(-1) != temp.end() && i == symbols.size())
		{
			//cout << "�����գ��ұ��������һ��" << endl;
			firstSet.insert(-1);
			break;
		}
	}
}

//��first��
void first(int &boundN, map<int, set<int>> &firstMap, map<int, pair<int, int>> &indexMap, int &boundT, vector<pair<int, vector<int>>> &productionVec)
{
	set<int> firstSet, stackSet;

	for (int symbol = -1; symbol <= boundN; symbol++)
	{
		firstSet.clear();
		stackSet.clear();
		//��symbol��first��
		first_symbol(firstSet, symbol, stackSet, firstMap, indexMap, boundT, productionVec);
		//��symbol��first��д��firstMap
		firstMap.emplace(symbol, firstSet);
	}
}