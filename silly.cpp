// Project Identifier: 01BD41C3BF016AD7E8B6F837DF18926EC3E83350
#include <getopt.h>
#include <stdarg.h>
#include <string>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <map>
#include <unordered_map>
#include "TableEntry.h"

using namespace std;
using Row = vector<TableEntry>;
using Table = vector<Row>;

struct TableInfo {
	TableInfo() :
	bstIndexCurr(-1), hashIndexCurr(-1) {}
	Table table;
	vector<char> colType;
	vector<string> colName;
	map<TableEntry, vector<int>> bstIndex;
	int bstIndexCurr;
	unordered_map<TableEntry, vector<int>> hashIndex;
	int hashIndexCurr;
};

class GreaterThan {
public:
	GreaterThan(int a, TableEntry &b) :
		colIndex(a), value(b) {}

	bool operator() (const vector<TableEntry> &r) {
		return r[colIndex] > value;
	}
private:
	int colIndex;
	TableEntry value;
};
class LessThan {
public:
	LessThan(int a, TableEntry &b) :
		colIndex(a), value(b) {}

	bool operator() (const vector<TableEntry> &r) {
		return r[colIndex] < value;
	}
private:
	int colIndex;
	TableEntry value;
};
class EqualTo {
public:
	EqualTo(int a, TableEntry &b) :
		colIndex(a), value(b) {}

	bool operator() (const vector<TableEntry> &r) {
		return r[colIndex] == value;
	}
private:
	int colIndex;
	TableEntry value;
};

class Silly {
public:
	bool tableError(string &tableName) {
		auto it = allTable.find(tableName);
		string trash;
		if (it == allTable.end()) {
			getline(cin, trash);
			cout << "Error: " << tableName << " does not name a table in the database" << '\n';
			return true;
		}
		else return false;
	}
	bool colError(int findIndex, string &colName, string &tableName) {
		string trash;
		if (findIndex == -1) {
			getline(cin, trash);
			cout << "Error: " << colName << " does not name a column in " << tableName << '\n';
			return true;
		}
		else return false;
	}
	void getMode(int argc, char * argv[]) {
		quiet = false;
		opterr = true;
		int choice;
		int option_index = 0;
		option long_options[] = {
			{ "help",    no_argument,       nullptr, 'h' },
			{ "quiet",    no_argument,       nullptr, 'q' },
			{ nullptr, 0,                 nullptr, '\0' }
		};

		while ((choice = getopt_long(argc, argv, "hq",
			long_options, &option_index)) != -1) {
			switch (choice) {
			case 'h': {
				cerr << "go check speck" << endl;
				exit(0);
				break;
			}
			case 'q': {
				quiet = true;
				break;
			}

			default:
				cerr << "Error: invalid option" << endl;
				exit(1);
			}
		}
		return;
	}
	void insertFour(vector<char> &colType, Table &table) {
		table.resize(table.size() + 1);
		table.back().reserve(colType.size());
		Row tempRow;
		for (size_t j = 0; j < colType.size(); j++) {
			switch (colType[j]) {
			case 'i': {
				int temp;
				cin >> temp;
				table.back().emplace_back(temp);
				break;
			}
			case 'd': {
				double temp;
				cin >> temp;
				table.back().emplace_back(temp);
				break;
			}
			case 's': {
				string temp;
				cin >> temp;
				table.back().emplace_back(temp);
				break;
			}
			case 'b': {
				bool temp;
				cin >> temp;
				table.back().emplace_back(temp);
				break;
			}
			}
		}
	}
	void GenerateIndexHelper(string &tableName, int findIndex, bool bst_hash) {
		if (allTable[tableName].bstIndexCurr != -1) allTable[tableName].bstIndex.clear();
		if (allTable[tableName].hashIndexCurr != -1) allTable[tableName].hashIndex.clear();
		allTable[tableName].bstIndexCurr = -1;
		allTable[tableName].hashIndexCurr = -1;
		for (size_t i = 0; i < allTable[tableName].table.size(); i++) {
			if (bst_hash) {
				(allTable[tableName].bstIndex[allTable[tableName].table[i][findIndex]]).push_back(static_cast<int>(i));
			}
			else {
				(allTable[tableName].hashIndex[allTable[tableName].table[i][findIndex]]).push_back(static_cast<int>(i));
			}
		}
		if (bst_hash) {
			allTable[tableName].bstIndexCurr = findIndex;
		}
		else {
			allTable[tableName].hashIndexCurr = findIndex;
		}
	}
	void readComd() {
		char comd;
		string trash;
		string tableName;
		int size;
		cout << "% ";
		while (cin >> comd) {
			switch (comd) {
			case 'C': {
				cin >> trash >> tableName >> size;
				auto it = allTable.find(tableName);
				if (it != allTable.end()) {
					getline(cin, trash);
					cout << "Error: Cannot create already existing table " << tableName << '\n';
					break;
				}
				TableInfo temp1;
				for (int i = 0; i < size; i++) {
					char typeChar;
					cin >> typeChar >> trash;
					switch (typeChar) {
					case 'i':
						temp1.colType.push_back('i');
						break;
					case 'd':
						temp1.colType.push_back('d');
						break;
					case 's':
						temp1.colType.push_back('s');
						break;
					case 'b':
						temp1.colType.push_back('b');
						break;
					}
				}
				cout << "New table " << tableName << " with column(s) ";
				for (int i = 0; i < size; i++) {
					string tempS;
					cin >> tempS;
					temp1.colName.push_back(tempS);
					cout << tempS << " ";
				}
				cout << "created" << '\n';
				allTable.emplace(tableName, temp1);
				break;
			}
			case 'I': {
				cin >> trash >> trash >> tableName >> size >> trash;
				auto it = allTable.find(tableName);
				if (it == allTable.end()) {
					cout << "Error: " << tableName << " does not name a table in the database" << '\n';
					break;
				}
				int begin = static_cast<int>(allTable[tableName].table.size());
				int end = begin + size - 1;
				allTable[tableName].table.reserve(allTable[tableName].table.size() + size);//////////
				for (int i = 0; i < size; i++) {
					insertFour(allTable[tableName].colType, allTable[tableName].table);
				}
				if (allTable[tableName].hashIndexCurr != -1) GenerateIndexHelper(tableName, allTable[tableName].hashIndexCurr, false);
				if (allTable[tableName].bstIndexCurr != -1) GenerateIndexHelper(tableName, allTable[tableName].bstIndexCurr, true);

				cout << "Added " << size << " rows to " << tableName
					<< " from position " << begin << " to " << end << '\n';
				break;
			}
			case 'D': {
				char op;
				string cinColName;
				vector<TableEntry> type;
				int deleted = 0;
				cin >> trash >> trash >> tableName >> trash >> cinColName >> op;
				if (tableError(tableName)) break;
				int findIndex = -1;
				for (size_t i = 0; i < allTable[tableName].colName.size(); i++) {
					if (allTable[tableName].colName[i] == cinColName) {
						findIndex = static_cast<int>(i);
						break;
					}
				}
				if (colError(findIndex, cinColName, tableName)) break;
				switch (allTable[tableName].colType[findIndex]) {
				case 'i': {
					int a;
					cin >> a;
					type.emplace_back(a);
					break;
				}
				case 'd': {
					double a;
					cin >> a;
					type.emplace_back(a);
					break;
				}
				case 's': {
					string a;
					cin >> a;
					type.emplace_back(a);
					break;
				}
				case 'b': {
					bool a;
					cin >> a;
					type.emplace_back(a);
					break;
				}
				}
				switch (op) {
				case '>': {
					GreaterThan tempOp(findIndex, type[0]);
					for (size_t k = 0; k < allTable[tableName].table.size();) {
						if (tempOp(allTable[tableName].table[k])) {
							allTable[tableName].table.erase(allTable[tableName].table.begin() + k);
							deleted++;
						}
						else {
							k++;
						}
					}
					break;
				}
				case '<': {
					LessThan tempOp(findIndex, type[0]);
					for (size_t k = 0; k < allTable[tableName].table.size();) {
						if (tempOp(allTable[tableName].table[k])) {
							allTable[tableName].table.erase(allTable[tableName].table.begin() + k);
							deleted++;
						}
						else {
							k++;
						}
					}
					break;
				}
				case '=': {
					EqualTo tempOp(findIndex, type[0]);
					for (size_t k = 0; k < allTable[tableName].table.size();) {
						if (tempOp(allTable[tableName].table[k])) {
							allTable[tableName].table.erase(allTable[tableName].table.begin() + k);
							deleted++;
						}
						else {
							k++;
						}
					}
					break;
				}
				}
				if (deleted > 0) {
					if (allTable[tableName].hashIndexCurr != -1) GenerateIndexHelper(tableName, allTable[tableName].hashIndexCurr, false);
					if (allTable[tableName].bstIndexCurr != -1) GenerateIndexHelper(tableName, allTable[tableName].bstIndexCurr, true);
				}
				cout << "Deleted " << deleted << " rows from " << tableName << '\n';
				break;
			}
			case 'G': {
				string indexType;
				string indexObj;
				bool bst_hash;
				int findIndex = -1;
				cin >> trash >> trash >> tableName >> indexType >> trash >> trash >> indexObj;
				auto it = allTable.find(tableName);
				if (it == allTable.end()) {
					cout << "Error: " << tableName << " does not name a table in the database" << '\n';
					break;
				}
				bst_hash = (indexType == "bst") ? true : false;
				for (size_t i = 0; i < allTable[tableName].colName.size(); i++) {
					if (allTable[tableName].colName[i] == indexObj) {
						findIndex = static_cast<int>(i);
						break;
					}
				}
				if (findIndex == -1) {
					cout << "Error: " << indexObj << " does not name a column in " << tableName << '\n';
					break;
				}
				GenerateIndexHelper(tableName, findIndex, bst_hash);
				cout << "Created " << indexType << " index for table " << tableName << " on column " << indexObj << '\n';
				break;
			}
			case 'J': {
				int coutCount = 0;
				string tableName1;
				string tableName2;
				string colName1;
				string colName2;
				int findIndex1 = -1;
				int findIndex2 = -1;
				int findIndex = -1;
				string coltemp;
				int oneTwo;
				vector<pair<int, int>> outPut;//<index, 1/2>
				cin >> trash >> tableName1 >> trash >> tableName2 >> trash >> colName1 >> trash >> colName2
					>> trash >> trash >> size;
				outPut.reserve(size);
				if (tableError(tableName1)) break;
				if (tableError(tableName2)) break;
				for (size_t j = 0; j < allTable[tableName1].colName.size(); j++) {
					if (allTable[tableName1].colName[j] == colName1) {
						findIndex1 = static_cast<int>(j);
						break;
					}
				}
				for (size_t j = 0; j < allTable[tableName2].colName.size(); j++) {
					if (allTable[tableName2].colName[j] == colName2) {
						findIndex2 = static_cast<int>(j);
						break;
					}
				}
				if (colError(findIndex1, colName1, tableName1)) break;
				if (colError(findIndex2, colName2, tableName2)) break;
				bool toBreak = false;
				vector<string> coutRecord;
				coutRecord.reserve(size);
				for (int i = 0; i < size; i++) {
					cin >> coltemp;
					cin >> oneTwo;
					coutRecord.push_back(coltemp);
					if (oneTwo == 1) {
						for (size_t j = 0; j < allTable[tableName1].colName.size(); j++) {
							if (allTable[tableName1].colName[j] == coltemp) {
								findIndex = static_cast<int>(j);
								break;
							}
						}
						if (findIndex == -1) {
							if (i < size - 1) getline(cin, trash);
							cout << "Error: " << coltemp << " does not name a column in " << tableName1 << '\n';
							toBreak = true;
							break;
						}
					}
					else {
						for (size_t j = 0; j < allTable[tableName2].colName.size(); j++) {
							if (allTable[tableName2].colName[j] == coltemp) {
								findIndex = static_cast<int>(j);
								break;
							}
						}
						if (findIndex == -1) {
							if (i < size - 1) getline(cin, trash);
							cout << "Error: " << coltemp << " does not name a column in " << tableName2 << '\n';
							toBreak = true;
							break;
						}
					}
					outPut.push_back({findIndex, oneTwo});
				}
				if (toBreak) break;
				for (size_t i = 0; i < coutRecord.size(); i++) {
					if (!quiet) cout << coutRecord[i] << " ";
				}
				if (!quiet) cout << '\n';
				if (allTable[tableName2].hashIndexCurr == findIndex2) {
					for (size_t i = 0; i < allTable[tableName1].table.size(); i++) {
						auto ittemp = allTable[tableName2].hashIndex.find(allTable[tableName1].table[i][findIndex1]);
						if (ittemp != allTable[tableName2].hashIndex.end()) {
							if (quiet) coutCount += static_cast<int>(allTable[tableName2].hashIndex[allTable[tableName1].table[i][findIndex1]].size());
							else {
								for (size_t j = 0; j < allTable[tableName2].hashIndex[allTable[tableName1].table[i][findIndex1]].size(); j++) {
									for (size_t k = 0; k < outPut.size(); k++) {
										if (outPut[k].second == 1) {
											cout << allTable[tableName1].table[i][outPut[k].first] << " ";
										}
										else {
											cout << allTable[tableName2].table[allTable[tableName2].
												hashIndex[allTable[tableName1].table[i][findIndex1]][j]][outPut[k].first] << " ";
										}
									}
									coutCount++;
									cout << '\n';
								}
							}
						}
					}
				}
				else if (allTable[tableName2].bstIndexCurr == findIndex2) {
					for (size_t i = 0; i < allTable[tableName1].table.size(); i++) {
						auto ittemp = allTable[tableName2].bstIndex.find(allTable[tableName1].table[i][findIndex1]);
						if (ittemp != allTable[tableName2].bstIndex.end()) {
							if (quiet) coutCount += static_cast<int>(allTable[tableName2].bstIndex[allTable[tableName1].table[i][findIndex1]].size());
							else {
								for (size_t j = 0; j < allTable[tableName2].bstIndex[allTable[tableName1].table[i][findIndex1]].size(); j++) {
									for (size_t k = 0; k < outPut.size(); k++) {
										if (outPut[k].second == 1) {
											cout << allTable[tableName1].table[i][outPut[k].first] << " ";
										}
										else {
											cout << allTable[tableName2].table[allTable[tableName2].
												bstIndex[allTable[tableName1].table[i][findIndex1]][j]][outPut[k].first] << " ";
										}
									}
									coutCount++;
									cout << '\n';
								}
							}
						}
					}
				}
				else {//////////////custom gererate index
					GenerateIndexHelper(tableName2, findIndex2, false);
					for (size_t i = 0; i < allTable[tableName1].table.size(); i++) {
						auto ittemp = allTable[tableName2].hashIndex.find(allTable[tableName1].table[i][findIndex1]);
						if (ittemp != allTable[tableName2].hashIndex.end()) {
							if (quiet) coutCount += static_cast<int>(allTable[tableName2].hashIndex[allTable[tableName1].table[i][findIndex1]].size());
							else {
								for (size_t j = 0; j < allTable[tableName2].hashIndex[allTable[tableName1].table[i][findIndex1]].size(); j++) {
									for (size_t k = 0; k < outPut.size(); k++) {
										if (outPut[k].second == 1) {
											cout << allTable[tableName1].table[i][outPut[k].first] << " ";
										}
										else {
											cout << allTable[tableName2].table[allTable[tableName2].
												hashIndex[allTable[tableName1].table[i][findIndex1]][j]][outPut[k].first] << " ";
										}
									}
									coutCount++;
									cout << '\n';
								}
							}
						}
					}
					allTable[tableName2].hashIndex.clear();
					allTable[tableName2].hashIndexCurr = -1;
				}
				cout << "Printed " << coutCount << " rows from joining " << tableName1 << " to " << tableName2 << '\n';
				break;
			}
			case 'R': {
				cin >> trash >> tableName;
				auto it = allTable.find(tableName);
				if (it == allTable.end()) {
					cout << "Error: " << tableName << " does not name a table in the database" << '\n';
					break;
				}
				else {
					allTable.erase(it);
				}
				cout << "Table " << tableName << " deleted" << '\n';
				break;
			}
			case 'P': {
				char temp;
				char op;
				bool toBreak = false;
				string cinColName;
				vector<TableEntry> type;
				string temp2;
				cin >> trash >> trash >> tableName >> size;
				if (tableError(tableName)) break;
				int findIndex = -1;
				vector<int> outIndex;
				for (int i = 0; i < size; i++) {
					cin >> temp2;
					bool found = false;
					for (size_t j = 0; j < allTable[tableName].colName.size(); j++) {
						if (temp2 == allTable[tableName].colName[j]) {
							outIndex.push_back(static_cast<int>(j));
							found = true;
						}
					}
					if (!found) {
						getline(cin, trash);
						cout << "Error: " << temp2 << " does not name a column in " << tableName << '\n';
						toBreak = true;
						break;
					}
				}
				if (toBreak) break;
				cin >> temp;
				int coutCount = 0;
				
				if (temp == 'W') {
					cin >> trash >> cinColName >> op;
					for (size_t i = 0; i < allTable[tableName].colName.size(); i++) {
						if (allTable[tableName].colName[i] == cinColName) {
							findIndex = static_cast<int>(i);
							break;
						}
					}
					if (colError(findIndex, cinColName, tableName)) break;
					if (!quiet) {
						for (size_t i = 0; i < outIndex.size(); i++) {
							cout << allTable[tableName].colName[outIndex[i]] << " ";
						}
						cout << '\n';
					}
					switch (allTable[tableName].colType[findIndex]) {
					case 'i': {
						int a;
						cin >> a;
						type.emplace_back(a);
						break;
					}
					case 'd': {
						double a;
						cin >> a;
						type.emplace_back(a);
						break;
					}
					case 's': {
						string a;
						cin >> a;
						type.emplace_back(a);
						break;
					}
					case 'b': {
						bool a;
						cin >> a;
						type.emplace_back(a);
						break;
					}
					}
					switch (op) {
					case '>': {
						GreaterThan tempOp(findIndex, type[0]);
						if (allTable[tableName].bstIndexCurr == findIndex) {
							for (auto it = allTable[tableName].bstIndex.begin(); it != allTable[tableName].bstIndex.end(); it++) {
								if (tempOp(allTable[tableName].table[(*it).second[0]])) {
									if (quiet) coutCount += static_cast<int>((*it).second.size());
									else {
										for (size_t i = 0; i < (*it).second.size(); i++) {
											for (size_t j = 0; j < outIndex.size(); j++) {
												cout << allTable[tableName].table[(*it).second[i]][outIndex[j]] << " ";
											}
											cout << '\n';
											coutCount++;
										}
									}
								}
							}
						}
						else {
							for (size_t i = 0; i < allTable[tableName].table.size(); i++) {
								if (tempOp(allTable[tableName].table[i])) {
									for (size_t j = 0; j < outIndex.size(); j++) {
										if (!quiet) cout << allTable[tableName].table[i][outIndex[j]] << " ";
									}
									if (!quiet) cout << '\n';
									coutCount++;
								}
							}
						}
						break;
					}
					case '<': {
						LessThan tempOp(findIndex, type[0]);
						if (allTable[tableName].bstIndexCurr == findIndex) {
							for (auto it = allTable[tableName].bstIndex.begin(); it != allTable[tableName].bstIndex.end(); it++) {
								if (tempOp(allTable[tableName].table[(*it).second[0]])) {
									if (quiet) coutCount += static_cast<int>((*it).second.size());
									else {
										for (size_t i = 0; i < (*it).second.size(); i++) {
											for (size_t j = 0; j < outIndex.size(); j++) {
												cout << allTable[tableName].table[(*it).second[i]][outIndex[j]] << " ";
											}
											cout << '\n';
											coutCount++;
										}
									}
								}
							}
						}
						else {
							for (size_t i = 0; i < allTable[tableName].table.size(); i++) {
								if (tempOp(allTable[tableName].table[i])) {
									for (size_t j = 0; j < outIndex.size(); j++) {
										if (!quiet) cout << allTable[tableName].table[i][outIndex[j]] << " ";
									}
									if (!quiet) cout << '\n';
									coutCount++;
								}
							}
						}
						break;
					}
					case '=': {
						EqualTo tempOp(findIndex, type[0]);
						if (allTable[tableName].bstIndexCurr == findIndex) {
							for (auto it = allTable[tableName].bstIndex.begin(); it != allTable[tableName].bstIndex.end(); it++) {
								if (tempOp(allTable[tableName].table[(*it).second[0]])) {
									if (quiet) coutCount += static_cast<int>((*it).second.size());
									else {
										for (size_t i = 0; i < (*it).second.size(); i++) {
											for (size_t j = 0; j < outIndex.size(); j++) {
												cout << allTable[tableName].table[(*it).second[i]][outIndex[j]] << " ";
											}
											cout << '\n';
											coutCount++;
										}
									}
								}
							}
						}
						else if (allTable[tableName].bstIndexCurr == -1 && allTable[tableName].hashIndexCurr == findIndex) {
							for (auto it = allTable[tableName].hashIndex.begin(); it != allTable[tableName].hashIndex.end(); it++) {
								if (tempOp(allTable[tableName].table[(*it).second[0]])) {
									if (quiet) coutCount += static_cast<int>((*it).second.size());
									else {
										for (size_t i = 0; i < (*it).second.size(); i++) {
											for (size_t j = 0; j < outIndex.size(); j++) {
												cout << allTable[tableName].table[(*it).second[i]][outIndex[j]] << " ";
											}
											cout << '\n';
											coutCount++;
										}
									}
								}
							}
						}
						else { 
							for (size_t i = 0; i < allTable[tableName].table.size(); i++) {
								if (tempOp(allTable[tableName].table[i])) {
									for (size_t j = 0; j < outIndex.size(); j++) {
										if (!quiet) cout << allTable[tableName].table[i][outIndex[j]] << " ";
									}
									if (!quiet) cout << '\n';
									coutCount++;
								}
							}
						}
						break;
					}
					}
					
				}
				else {
					cin >> trash;
					for (size_t i = 0; i < outIndex.size(); i++) {
						if (!quiet) cout << allTable[tableName].colName[outIndex[i]] << " ";
					}
					if (!quiet) cout << '\n';
					for (size_t i = 0; i < allTable[tableName].table.size(); i++) {
						for (size_t j = 0; j < outIndex.size(); j++) {
							if (!quiet) cout << allTable[tableName].table[i][outIndex[j]] << " ";
						}
						if (!quiet) cout << '\n';
						coutCount++;
					}
				}
				cout << "Printed " << coutCount << " matching rows from " << tableName << '\n';
				break;
			}
			case '#': {
				getline(cin, trash);
				break;
			}
			case 'Q': {
				cout << "Thanks for being silly!" << endl;
				return;
			}
			default: {
				getline(cin, trash);
				cout << "Error: unrecognized command" << '\n';
				break;
			}
			}
			cout << "% ";
		}
	}
private:
	unordered_map<string, TableInfo> allTable;
	bool quiet;
};

int main(int argc, char *argv[]) {
	ios_base::sync_with_stdio(false);
	cin >> std::boolalpha;
	cout << std::boolalpha;
	Silly silly;
	silly.getMode(argc, argv);
	silly.readComd();
	return 0;
}