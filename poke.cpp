// Project Identifier: 5949F553E20B650AB0FB2266D3C0822B13D248B0
#include <iostream>
#include <cmath>
#include <stdarg.h>
#include <numeric>
#include <string>
#include <vector>
#include <getopt.h>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <climits>
#include <list>
using namespace std;
using Point = pair<double, double>;

struct PrimObj {
	PrimObj(Point temp) :
		pt(temp), d(numeric_limits<double>::infinity()), k(false)
	{
		if ((temp.first == 0 && temp.second <= 0) || (temp.second == 0 && temp.first <= 0)) location = 'c';
		else if (temp.first < 0 && temp.second < 0) location = 's';
		else location = 'l';
	}
	Point pt;
	double d;//inf
	int p;
	bool k;
	char location;
};

class poke {
public:
	bool OverSea(PrimObj &a, PrimObj &b) {
		if (a.location == 'c' || b.location == 'c') return false;
		else if (a.location == b.location) return false;
		else return true;
	}
	double getDistSq(Point &a, Point &b) {
		double diff1 = (a.first - b.first);
		double diff2 = (a.second - b.second);
		return diff1 * diff1 + diff2 * diff2;
	}
	void getMode(int argc, char * argv[]) {
		opterr = true;
		int choice;
		int option_index = 0;
		option long_options[] = {
			{ "help",    no_argument,       nullptr, 'h' },
			{ "mode",    required_argument,       nullptr, 'm' },
			{ nullptr, 0,                 nullptr, '\0' }
		};

		while ((choice = getopt_long(argc, argv, "hm:",
			long_options, &option_index)) != -1) {
			switch (choice) {
			case 'h': {
				cerr << "go check speck" << endl;
				exit(0);
				break;
			}
			case 'm': {
				string mode;
				mode = optarg;
				if (mode == "MST") Mode = 'A';
				else if (mode == "FASTTSP") Mode = 'B';
				else if (mode == "OPTTSP") Mode = 'C';
				else {
					cerr << "invalid mode";
					exit(1);
				}
				break;
			}
			default:
				cerr << "Error: invalid option" << endl;
				exit(1);
			}
		}
		return;
	}
	bool promising(int permLength) {
		double predict = MST_helper(permLength);
		if (predict < 0) return false;
		double known = 0;
		double shortToB = numeric_limits<double>::infinity();
		double shortToE = numeric_limits<double>::infinity();
		known += GenpLen;
		for (int i = permLength; i < static_cast<int>(Genp.size()); i++) {
			double tempE = sqrt(getDistSq(Fast[Genp[i]].first, Fast[Genp[permLength - 1]].first));
			//double tempE = distMatrix[Genp[i]][Genp[permLength - 1]];
			double tempB = sqrt(getDistSq(Fast[Genp[i]].first, Fast[Genp[0]].first));
			//double tempB = distMatrix[Genp[i]][Genp[0]];
			if (tempB < shortToB) shortToB = tempB;
			if (tempE < shortToE) shortToE = tempE;
			//if (known + predict > OptLen) return false;
		}
		predict += shortToB;
		predict += shortToE;
		return (known + predict > OptLen) ? false : true;
	}
	void genPerms(int permLength) {
		if (static_cast<int>(Genp.size()) == permLength) {
			double temp1 = sqrt(getDistSq(Fast[Genp[0]].first, Fast[Genp[permLength - 1]].first));
			//double temp1 = distMatrix[Genp[0]][Genp[permLength - 1]];
			GenpLen += temp1;
			if (GenpLen < OptLen) {
				Opt = Genp;
				OptLen = GenpLen;
			}
			GenpLen -= temp1;
			return;
		} // if
		if ((numV - permLength >= 5) && !promising(permLength)) return;
		//if (GenpLen >= OptLen) return;
		for (size_t i = permLength; i < Genp.size(); ++i) {
			swap(Genp[permLength], Genp[i]);
			double temp2 = sqrt(getDistSq(Fast[Genp[permLength - 1]].first, Fast[Genp[permLength]].first));
			//double temp2 = distMatrix[Genp[permLength - 1]][Genp[permLength]];
			GenpLen += temp2;
			genPerms(permLength + 1);
			swap(Genp[permLength], Genp[i]);
			GenpLen -= temp2;
		} // for
	} // genPerms()
	double MST_helper(int permLength) {
		Prim.clear();
		Prim.reserve(numV - permLength);
		for (int i = permLength; i < numV; i++) Prim.push_back(PrimObj(Fast[Genp[i]].first));
		int current = 0;
		double totalDist = 0;
		Prim[current].d = 0;
		Prim[current].p = -1;
		for (int i = 0; i <= static_cast<int>(Prim.size()); i++) {
			for (int j = 0; j < static_cast<int>(Prim.size()); j++) {
				if (!Prim[j].k) {
					current = j;
					break;
				}
			}
			for (int j = current + 1; j < static_cast<int>(Prim.size()); j++) {
				if (!Prim[j].k && Prim[j].d < Prim[current].d) current = j;
			}
			Prim[current].k = true;
			if (i < static_cast<int>(Prim.size())) totalDist += sqrt(Prim[current].d);
			if (totalDist >= OptLen) return -1;
			for (int j = 0; j < static_cast<int>(Prim.size()); j++) {
				if (!Prim[j].k) {
					double tempDist = getDistSq(Prim[current].pt, Prim[j].pt);
					//double tempDist = distMatrix[current + permLength][j + permLength];
					if (tempDist < Prim[j].d) {
						Prim[j].d = tempDist;
						Prim[j].p = current;
					}
				}
			}
		}
		return totalDist;
	}
	void driverA() {
		cin >> numV;
		Prim.reserve(numV);
		for (int i = 0; i < numV; i++) {
			double a;
			double b;
			cin >> a >> b;
			Point temp = { a, b };
			Prim.push_back(PrimObj(temp));
		}
		int current = 0;
		double totalDist = 0;
		Prim[current].d = 0;
		Prim[current].p = -1;
		for (int i = 0; i <= numV; i++) {
			int tempInd = Prim[current].p;
			if (tempInd != -1) PrimIndex.push_back({ min(current, tempInd), max(current, tempInd) });
			for (int j = 0; j < static_cast<int>(Prim.size()); j++) {
				if (!Prim[j].k) {
					current = j;
					break;
				}
			}
			for (int j = current + 1; j < static_cast<int>(Prim.size()); j++) {
				if (!Prim[j].k && Prim[j].d < Prim[current].d) current = j;
			}
			Prim[current].k = true;
			if (i < numV) totalDist += sqrt(Prim[current].d);
			if (Prim[current].d == numeric_limits<double>::infinity()) {
				cerr << "Cannot construct MST" << endl;
				exit(1);
			}
			for (int j = 0; j < static_cast<int>(Prim.size()); j++) {
				if (!Prim[j].k && !OverSea(Prim[current], Prim[j])) {
					double tempDist = getDistSq(Prim[current].pt, Prim[j].pt);
					if (tempDist < Prim[j].d) {
						Prim[j].d = tempDist;
						Prim[j].p = current;
					}
				}
			}
		}
		cout << totalDist << '\n';
		for (unsigned i = 0; i < PrimIndex.size(); i++) {
			cout << PrimIndex[i].first << " " << PrimIndex[i].second << '\n';
		}

	}
	void driverB(bool bc) {
		double totalDist = 0;
		cin >> numV;
		Fast.reserve(numV);
		for (int i = 0; i < 3; i++) {
			double a, b;
			cin >> a >> b;
			Fast.push_back({ {a, b}, i - 1 });
		}
		totalDist += sqrt(getDistSq(Fast[0].first, Fast[1].first));
		totalDist += sqrt(getDistSq(Fast[1].first, Fast[2].first));
		totalDist += sqrt(getDistSq(Fast[0].first, Fast[2].first));
		Fast[0].second = 2;
		for (int i = 3; i < numV; i++) {
			int minInd = 0;
			double minCost = INT_MAX;
			double a;
			double b;
			cin >> a >> b;
			Point temp = { a,b };
			int j = 0;
			unsigned loopCount = 0;
			while (loopCount <= Fast.size()) {
				double testCost = sqrt(getDistSq(temp, Fast[Fast[j].second].first)) - sqrt(getDistSq(Fast[j].first, Fast[Fast[j].second].first))
					+ sqrt(getDistSq(temp, Fast[j].first));
				if (testCost < minCost) {
					minInd = j;
					minCost = testCost;
				}
				loopCount++;
				j = Fast[j].second;
			}
			totalDist += minCost;
			Fast.push_back({ temp, Fast[minInd].second });
			Fast[minInd].second = i;
		}
		if (bc) {
			cout << totalDist << endl;
			int loopCount = 1;
			int j = 0;
			while (loopCount <= numV) {
				cout << j << " ";
				j = Fast[j].second;
				loopCount++;
			}
			cout << endl;
		}
		else {
			/*distMatrix.resize(numV);
			for (int i = 0; i < numV; i++) {
				distMatrix[i].resize(numV);
				for (int j = 0; j < numV; j++) {
					distMatrix[i][j] = sqrt(getDistSq(Fast[i].first, Fast[j].first));
				}
			}*/
			OptLen = totalDist;
			GenpLen = 0;
			int loopCount = 1;
			int j = 0;
			Opt.resize(numV);
			Genp.resize(numV);
			while (loopCount <= numV) {
				Opt[loopCount - 1] = j;
				Genp[loopCount - 1] = j;
				j = Fast[j].second;
				loopCount++;
			}
		}
	}
	void driverC() {
		driverB(false);
		genPerms(1);
		cout << OptLen << '\n';
		for (int i = 0; i < numV; i++) cout << Opt[i] << " ";
		cout << endl;
	}
	void driver() {
		if (Mode == 'A') driverA();
		else if (Mode == 'B') driverB(true);
		else driverC();
	}


private:
	vector<PrimObj> Prim;
	vector<pair<int, int>> PrimIndex;
	vector<pair<Point, int>> Fast;
	//vector<vector<double>> distMatrix;
	vector<int> Opt;
	vector<int> Genp;
	double OptLen;
	double GenpLen;
	int numV;
	char Mode;//ABC
};

int main(int argc, char*argv[]) {
	ios_base::sync_with_stdio(false);
	cout << setprecision(2);
	cout << fixed;
	poke p4;
	p4.getMode(argc, argv);
	p4.driver();
	return 0;
}