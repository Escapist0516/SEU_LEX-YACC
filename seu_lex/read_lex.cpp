#include "read_lex.h"

vector<string> split(const string& aim, const string& s) {
	vector<string> re;
	if (aim == "") return re;
	string::size_type pos1 = 0, pos2 = aim.find(s);
	while (pos2 != string::npos) {
		re.push_back(aim.substr(pos1, pos2 - pos1));
		pos1 = aim.find_first_not_of(s, pos2);
		pos2 = aim.find(s, pos1);
	}
	if (pos1 != aim.length())
		re.push_back(aim.substr(pos1));
	return re;
}

void clean(string& aim) {
	if (aim.empty()) return;
	aim.erase(0, aim.find_first_not_of(' '));
	aim.erase(0, aim.find_first_not_of('\t'));
	aim.erase(aim.find_last_not_of(' ') + 1);
	aim.erase(aim.find_last_not_of('\r') + 1);
	aim.erase(aim.find_last_not_of('\n') + 1);
	aim.erase(aim.find_last_not_of('\t') + 1);
}

void read_lex_from_file(string filename, vector<string>& supDef, map<string, string>& head, vector<Rules>& rules, vector<string>& functions)
{
	ifstream in;
	in.open(filename.c_str(), ios::in);
	if (!in)
	{
		cout << "\nread_lex_from_file: Failed to open lex.txt!" << endl;
		exit(1);
	}

	string curlin;//存储当前行
	int state = 0;//表示当前处于的l文件的部份
				  //-1表示l文件不符合规范，0为打开文件状态，1为辅助定义部份，
				  //2为正则表达式部份,3为识别规则部份,4为子程序部分
	vector<string>splitRe;//存储分割后的head键值对，或者存储action组
	string lef, rig;
	int count = 0;//

	while (!in.eof() && state != -1) {
		getline(in, curlin);
		if (curlin.empty()) continue;
		switch (state) {
		case 0:
			if (curlin.find("%{") != string::npos) state = 1;
			else state = -1;
			break;
		case 1:
			if (curlin.find("%}") != string::npos) state = 2;
			else supDef.push_back(curlin);
			break;
		case 2:
			if (curlin.find("%%") != string::npos) state = 3;
			else {
				splitRe = split(curlin, " ");
				if (splitRe.size() == 2) {
					head.insert(pair<string, string>(splitRe[0], splitRe[1]));
					splitRe.clear();
				}
				else state = -1;
			}
			break;
		case 3:
			if (curlin.find("%%") != string::npos) state = 4;
			else {
				if (curlin[0] == '"') {
					int p = 1;
					while (curlin[p] != '"') ++p;
					if (p == 1) ++p;//针对"""的情况
					lef = curlin.substr(0, p + 1);
					rig = curlin.substr(p + 1);
					clean(rig);
					if (rig[0] == '{') {
						splitRe.push_back(rig.substr(1, rig.find_first_of(' ')));
						splitRe.push_back(rig.substr(rig.find_first_of(' ') + 1, rig.size() - rig.find_first_of(' ') - 2));
					}
					else splitRe.push_back(rig);
				}
				else if (curlin[0] == '{') {
					int p = 1;
					while (curlin[p] != '}') ++p;
					lef = curlin.substr(0, p + 1);
					rig = curlin.substr(p + 1);
					clean(rig);
					splitRe.push_back(rig);
				}
				Rules r;
				r.pattern = lef;
				r.actions = splitRe;
				rules.push_back(r);
				splitRe.clear();
			}
			break;
		case 4:
			functions.push_back(curlin);
			break;
		}
	}
	return;
}