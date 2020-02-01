// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68768FC9

#include <getopt.h>
#include <stdarg.h>
#include <string>
#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

class Dword {
public:
	Dword(string a) : word(a), diffIndex(-1),
		isIn(false), diffChar('\0'), cids('\0') {}
	bool operator==(const Dword &rhs) const {
		return word == rhs.word;
	}
	string word;
	int prev;
	int diffIndex;
	bool isIn;
	char diffChar;
	char cids;
};

class letter {
public:
	letter() {
		beginI = -1;
		endI = -1;
		change = false;
		swap = false;
		length = false;
		WM = true;
	}
	void getMode(int argc, char * argv[]) {
		string outMode1;
		int numQS = 0;
		opterr = true;
		int choice;
		int option_index = 0;
		option long_options[] = {
			{ "stack",    no_argument,       nullptr, 's' },
			{ "queue",    no_argument,       nullptr, 'q' },
			{ "change",    no_argument,       nullptr, 'c' },
			{ "swap",    no_argument,       nullptr, 'p' },
			{ "length",    no_argument,       nullptr, 'l' },
			{ "output",    required_argument,       nullptr, 'o' },
			{ "begin",    required_argument,       nullptr, 'b' },
			{ "end",    required_argument,       nullptr, 'e' },
			{ "help",    no_argument,       nullptr, 'h' },
			{ nullptr, 0,                 nullptr, '\0' }
		};

		while ((choice = getopt_long(argc, argv, "hqscplo:b:e:",
			long_options, &option_index)) != -1) {
			switch (choice) {
			case 'h':
				cout << "check project prompt for details" << endl;
				exit(0);

			case 'q':
				QS = true;
				numQS++;
				break;

			case 's':
				QS = false;
				numQS++;
				break;

			case 'c':
				change = true;
				break;

			case 'p':
				swap = true;
				break;

			case 'l':
				length = true;
				break;

			case 'o':
				outMode1 = optarg;
				if (outMode1 == "W") {
					WM = true;
				}
				else if (outMode1 == "M") {
					WM = false;
				}
				else {
					cerr << "No outMode";
					exit(1);
				}
				break;

			case 'b':
				if (optarg[0] == '\0') {
					cerr << "no begin word" << endl;
					exit(1);
				}
				begin = optarg;
				break;

			case 'e':
				if (optarg[0] == '\0') {
					cerr << "no end word" << endl;
					exit(1);
				}
				end = optarg;
				break;

			default:
				cerr << "Error: invalid option" << endl;
				exit(1);
			}
		}

		if ((!change && !swap && !length) || numQS != 1) {
			cerr << "Error: no mode specified" << endl;
			exit(1);
		}
		if (!length && begin.length() !=
			end.length()) {
			cerr << "Error: impossible situation" << endl;
			exit(1);
		}
		return;
	}
	void fillDictionary() {
		char SorC;
		int numLine;
		cin >> SorC >> numLine;
		string temp;
		if (SorC == 'S' && !length) {
			dictionary.reserve(numLine);
			while (cin >> temp) {
				if (temp[0] == '/') {
					getline(cin, temp);
				}
				else {
					if (temp.length() == begin.length()) {
						dictionary.push_back(temp);
					}
				}
			}
		}
		else if (SorC == 'S' && length) {
			dictionary.reserve(numLine);
			while (cin >> temp) {
				if (temp[0] == '/') {
					getline(cin, temp);
				}
				else {
					dictionary.push_back(temp);
				}
			}
		}
		else {
			while (cin >> temp) {
				if (temp[0] == '/') {
					getline(cin, temp);
				}
				else {
					generateDict(temp);
				}
			}
		}
		
		
		//dictionary.resize(data.size());
		for (unsigned int i = 0; i < dictionary.size(); i++) {
			if ((dictionary[i]).word == begin) {
				beginI = i;
			}
			if ((dictionary[i]).word == end) {
				endI = i;
			}
		}
		if (beginI == -1 || endI == -1) {
			cerr << "word does not exist" << endl;
			exit(1);
		}
	}
	void generateDict(string &temp) {
		char const *tempptr = temp.c_str();
		string before;
		while (*tempptr) {
			if (*tempptr == '&') {
				tempptr--;
				Dword a(before);
				dictionary.push_back(a);
				reverse(before.begin(), before.end());
				Dword b(before);
				dictionary.push_back(b);
				return;
			}
			else if (*tempptr == '[') {
				tempptr++;
				char const *tempptr2 = tempptr;
				while (*tempptr2 != ']') {
					tempptr2++;
				}
				tempptr2++;
				string after = tempptr2;
				while (*tempptr != ']') {
					Dword a(before + *tempptr + after);
					dictionary.push_back(a);
					tempptr++;
				}
				return;
			}
			else if (*tempptr == '!') {
				char const *tempptr2 = tempptr - 1;
				char const *tempptr3 = tempptr2 - 1;
				tempptr++;
				string after = tempptr;
				string before2 = before.substr(0, before.length() - 2);
				Dword a(before2 + *tempptr3 + *tempptr2 + after);
				dictionary.push_back(a);
				Dword b(before2 + *tempptr2 + *tempptr3 + after);
				dictionary.push_back(b);
				return;
			}
			else if (*tempptr == '?') {
				tempptr--;
				string before2 = before + *tempptr;
				tempptr++;
				tempptr++;
				string after = tempptr;
				Dword a(before + after);
				dictionary.push_back(a);
				Dword b(before2 + after);
				dictionary.push_back(b);
				return;
			}
			else {
				before += *tempptr;
				tempptr++;
			}
		}
		dictionary.push_back(before);
	}
	bool simLength(int a, int b) {
		unsigned int i = 0;
		unsigned int j = 0;
		unsigned int record = 0;
		int diffCount = 0;
		if ((dictionary[a].word).length() -
			(dictionary[b].word).length() == 1) {
			while (j < (dictionary[b].word).length()) {
				if ((dictionary[a].word)[i] != (dictionary[b].word)[j]) {
					diffCount++;
					if (diffCount == 1) {
						i++;
						record = j;
					}
					else {
						i++;
						j++;
					}
				}
				else {
					i++;
					j++;
				}
			}
			if (diffCount == 0) {
				dictionary[a].cids = 'i';
				dictionary[a].diffIndex = i;
				dictionary[a].diffChar = (dictionary[a].word)[i];
				return true;
			}
			else if (diffCount == 1) {
				dictionary[a].cids = 'i';
				dictionary[a].diffIndex = record;
				dictionary[a].diffChar = (dictionary[a].word)[record];
				return true;
			}
			else {
				return false;
			}
		}
		else if ((dictionary[b].word).length() -
			(dictionary[a].word).length() == 1) {
			while (i < (dictionary[a].word).length()) {
				if ((dictionary[a].word)[i] != (dictionary[b].word)[j]) {
					diffCount++;
					if (diffCount == 1) {
						j++;
						record = i;
					}
					else {
						i++;
						j++;
					}
				}
				else {
					i++;
					j++;
				}
			}
			if (diffCount == 0) {
				dictionary[a].cids = 'd';
				dictionary[a].diffIndex = j;
				return true;
			}
			else if (diffCount == 1) {
				dictionary[a].cids = 'd';
				dictionary[a].diffIndex = record;
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}
	bool Similar(int a, int b) {
		if (a == b) {
			dictionary[a].isIn = true;
			return false;
		}
		if ((dictionary[a].word).length() == (dictionary[b].word).length()) {
			int diffCount = 0;
			int ind01 = 0;
			for (unsigned int i = 0; i < (dictionary[a].word).length(); i++) {
				if ((dictionary[a].word)[i] != (dictionary[b].word)[i]) {
					diffCount++;
					if (diffCount >= 3) {
						return false;
					}
					ind01 = i;
				}
			}
			if (diffCount == 1 && change) {
				dictionary[a].cids = 'c';
				dictionary[a].diffIndex = ind01;
				dictionary[a].diffChar = (dictionary[a].word)[ind01];
				return true;
			}
			if (diffCount == 2) {
				if ((dictionary[a].word)[ind01] == (dictionary[b].word)[ind01 - 1]
					&& (dictionary[b].word)[ind01] == (dictionary[a].word)[ind01 - 1]
					&& swap) {
					dictionary[a].cids = 's';
					dictionary[a].diffIndex = ind01 - 1;
					return true;
				}
			}
		}
		else {
			if (length && simLength(a, b)) {
				return true;
			}
		}
		return false;
	}
	bool RoutingScheme() {
		if (begin == end) {
			return true;
		}
		data.push_back(beginI);
		bool temp001 = false;
		while (!data.empty()) {
			temp001 = findSimilar();
			if (temp001) {
				return true;
			}
		}
		return false;
	}
	bool findSimilar() {
		int current;
		if (QS) {
			current = data.front();
			data.pop_front();
		}
		else {
			current = data.back();
			data.pop_back();
		}
		for (unsigned int i = 0; i < dictionary.size(); i++) {
			if (!(dictionary[i]).isIn && Similar(i, current)
				&& (static_cast<int>(i) != current)) {
				data.push_back(i);
				(dictionary[i]).isIn = true;
				(dictionary[i]).prev = current;
				if (static_cast<int>(i) == endI) {
					return true;
				}

			}
		}
		return false;
	}
	void backTrack() {
		int tracking = endI;
		if (begin == end) {
			morph.push_back(beginI);
			return;
		}
		while (tracking != beginI) {
			morph.push_back(tracking);
			tracking = dictionary[tracking].prev;
		}
		morph.push_back(beginI);
	}
	void output() {
		cout << "Words in morph: " << morph.size() << endl << begin << endl;
		if (morph.size() == 1) {
			return;
		}
		for (size_t i = morph.size() - 1; i >= 1; i--) {
			if (WM) {
				cout << dictionary[morph[i - 1]].word << endl;
			}
			else {
				if (dictionary[morph[i - 1]].cids == 'c' || dictionary[morph[i - 1]].cids == 'i') {
					cout << dictionary[morph[i - 1]].cids << "," << dictionary[morph[i - 1]].diffIndex
						<< "," << dictionary[morph[i - 1]].diffChar << endl;
				}
				else {
					cout << dictionary[morph[i - 1]].cids << ","
						<< dictionary[morph[i - 1]].diffIndex << endl;
				}
			}
		}
	}
	int checkCount() {
		int count = 0;
		for (unsigned int i = 0; i < dictionary.size(); i++) {
			if ((dictionary[i]).isIn) {
				count++;
			}
		}
		return count;
	}

private:
	deque<int> data;
	vector<Dword> dictionary;
	vector<int> morph;
	string begin;
	string end;
	int beginI;
	int endI;
	bool QS;
	bool change;
	bool swap;
	bool length;
	bool WM;
};

int main(int argc, char *argv[]) {
	ios_base::sync_with_stdio(false);
	letter proj1;
	proj1.getMode(argc, argv);
	proj1.fillDictionary();
	bool success = proj1.RoutingScheme();
	if (success) {
		proj1.backTrack();
		proj1.output();
	}
	else {
		cout << "No solution, " << proj1.checkCount()
			<< " words checked." << endl;
	}

	return 0;
}

