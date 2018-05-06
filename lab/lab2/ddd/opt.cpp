//COMP506
//An Optimizer for ILOC

//Reference:
//Briggs, Preston, Keith D. Cooper, and L. Taylor Simpson. "Value numbering." Software-Practice and Experience 27.6 (1997): 701-724.
//https://github.com/Crejaud/CS-314_Principles-Of-Programming-Languages/
//https://github.com/jinhping/Compiler-Construction/
//http://www.ics.uci.edu/~thornton/ics142/CourseReference.html


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <math.h>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

using namespace std;

class irOperation {

	public:
		int lineNum = 0;
		string label = "";
		string operation = "";
		vector<string> leftOperands;
		vector<string> rightOperands;	
};

void ParseIr(string inputFile, vector<irOperation>& irOpes, unordered_map<string, int>& label_lineNum) {
	cout << "parse IR" << endl;
	ifstream infile;
	infile.open(inputFile);
	string line = "";
	int lineNum = 0;

	while (getline(infile, line)) {
		size_t idx;
		idx = line.find("//");
		if (idx != string::npos) {
			line = line.substr(0, idx);
		}
		lineNum += 1;
		irOperation irOpe;
		irOpe.lineNum = lineNum;
		int lineLength = line.length();
		idx = line.find(":");
		if (idx == string::npos) {
			int i = 1;

			while (i < lineLength && isblank(line[i])) {
				//cout << i << endl;
				//cout << line[i] << endl;
				i++;
			}

			string ope = "";

			while (i < lineLength && !isblank(line[i])) {
				//cout << i << endl;
				//cout << line[i] << endl;
				ope += line[i];
				i++;
			}

			//cout << ope << endl;
			irOpe.operation = ope;
			bool isOpe1 = (ope == "not" || ope == "loadI" || ope == "load" || ope == "cload" || ope == "store" || ope == "cstore"
				|| ope == "i2i" || ope == "c2c" || ope == "i2c" || ope == "c2i");
			bool isOpe2 = (ope == "storeAI" || ope == "storeAO" || ope == "cstoreAI" || ope == "cstoreAO" || ope == "cbr");
			bool isOpe3 = (ope == "br" || ope == "read" || ope == "cread");
			bool isOpe4 = (ope == "output" || ope == "coutput" || ope == "write" || ope == "cwrite");
			if (isOpe1) {
				
				while (i < lineLength && isblank(line[i])) {
					i++;
				}

				string leftOpe = "";

				while (i < lineLength && isalnum(line[i])) {
					leftOpe += line[i];
					i++;
				}

				while (i < lineLength && !isalnum(line[i])) {
					i++;
				}

				string rightOpe = "";

				while (i < lineLength && isalnum(line[i])) {
					rightOpe += line[i];
					i++;
				}

				irOpe.leftOperands.push_back(leftOpe);
				irOpe.rightOperands.push_back(rightOpe);
				irOpes.push_back(irOpe);
			} else if (isOpe2) {
				
				while (i < lineLength && isblank(line[i])) {
					i++;
				}

				string leftOpe = "";

				while (i < lineLength && isalnum(line[i])) {
					leftOpe += line[i];
					i++;
				}

				while (i < lineLength && !isalnum(line[i])) {
					i++;
				}

				string rightOpe1 = "";

				while (i < lineLength && isalnum(line[i])) {
					rightOpe1 += line[i];
					i++;
				}

				while (i < lineLength && !isalnum(line[i])) {
					i++;
				}

				string rightOpe2 = "";

				while (i < lineLength && isalnum(line[i])) {
					rightOpe2 += line[i];
					i++;
				}

				irOpe.leftOperands.push_back(leftOpe);
				irOpe.rightOperands.push_back(rightOpe1);
				irOpe.rightOperands.push_back(rightOpe2);
				irOpes.push_back(irOpe);
			} else if (isOpe3) {
				
				while (i < lineLength && !isalnum(line[i])) {
					i++;
				}

				string rightOpe = "";
				
				while (i < lineLength && isalnum(line[i])) {
					rightOpe += line[i];
					i++;
				}

				irOpe.rightOperands.push_back(rightOpe);
				irOpes.push_back(irOpe);
			} else if (isOpe4) {
				
				while (i < lineLength && isblank(line[i])) {
					i++;
				}

				string leftOpe = "";

				while (i < lineLength && isalnum(line[i])) {
					leftOpe += line[i];
					i++;
				}

				irOpe.leftOperands.push_back(leftOpe);
				irOpes.push_back(irOpe);
			} else if (ope == "halt") {
				irOpes.push_back(irOpe);
			} else { // 2 left, 1 right

				while (i < lineLength && isblank(line[i])) {
					i++;
				}

				string leftOpe1 = "";

				while (i < lineLength && isalnum(line[i])) {
					leftOpe1 += line[i];
					i++;
				}

				while (i < lineLength && !isalnum(line[i])) {
					i++;
				}

				string leftOpe2 = "";

				while (i < lineLength && isalnum(line[i])) {
					leftOpe2 += line[i];
					i++;
				}

				while (i < lineLength && !isalnum(line[i])) {
					i++;
				}

				string rightOpe1 = "";

				while (i < lineLength && isalnum(line[i])) {
					rightOpe1 += line[i];
					i++;
				}

				irOpe.leftOperands.push_back(leftOpe1);
				irOpe.leftOperands.push_back(leftOpe2);
				irOpe.rightOperands.push_back(rightOpe1);
				irOpes.push_back(irOpe);
			}
		} else {
			irOpe.label = line.substr(0, idx);
			irOpe.operation = "nop";
			irOpes.push_back(irOpe);
			label_lineNum[irOpe.label] = irOpe.lineNum;
		}
	}
}

void BuildCfg(vector<int>& leader, vector<int>& last, vector<irOperation> irOpes, unordered_map<string, int> label_lineNum) {
	leader.push_back(1);
	unordered_set<int> leaderSet;
	leaderSet.insert(1);

	for (auto irOpe : irOpes) {
		string ope = irOpe.operation;
		if (ope == "br") {
			string label1 = irOpe.rightOperands[0];
			int lineNum1 = label_lineNum[label1];
			if (!leaderSet.count(lineNum1)) {
				leader.push_back(lineNum1);
				leaderSet.insert(lineNum1);
			}
		} else if (ope == "cbr") {
			string label1 = irOpe.rightOperands[0];
			int lineNum1 = label_lineNum[label1];
			if (!leaderSet.count(lineNum1)) {
				leader.push_back(lineNum1);
				leaderSet.insert(lineNum1);
			}
			string label2 = irOpe.rightOperands[1];
			int lineNum2 = label_lineNum[label2];
			if (!leaderSet.count(lineNum2)) {
				leader.push_back(lineNum2);
				leaderSet.insert(lineNum2);
			}
		}
	}

	int totalLineNum = irOpes.size();

	for (auto l : leader) {
		while (l <= totalLineNum && !leaderSet.count(l + 1)) {
			l++;
		}
		last.push_back(l);
	}
}

void ValueNumbering(vector<int> leader, vector<int> last, vector<irOperation>& irOpes) {
	int leaderNum = leader.size();

	for (int i = 0; i < leaderNum; i++) {
		int begin = leader[i] - 1;
		int end = last[i] - 1;
		unordered_map <string, int> equ_vn;
		unordered_map <string, int> operand_vn;
		unordered_map <int, string> vn_operand;
		int vn = 0;

		for (int j = begin; j <= end; j++) {
			string ope = irOpes[j].operation;
			if (ope == "halt") {
				break;
			}
			bool isOpe = (ope == "nop" || ope == "not" || ope == "i2i" || ope == "c2c" || ope == "i2c" || ope == "c2i" || ope == "br" || ope == "cbr"
				|| ope == "read" || ope == "cread" || ope == "output" || ope == "coutput" || ope == "write" || ope == "cwrite"
				|| ope == "loadI" || ope == "load" || ope == "cload"
				|| ope == "store" || ope == "storeAI" || ope == "storeAO" || ope == "cstore" || ope == "cstoreAI" || ope == "cstoreAO");
			if (isOpe) {
				continue;
			} else {
				string l0 = irOpes[j].leftOperands[0];
				string l1 = irOpes[j].leftOperands[1];
				string r0 = irOpes[j].rightOperands[0];
				if (ope == "multI") {
					int c1 = stoi(l1);
					if ((c1 & (c1 - 1)) == 0) {
						int shift = sqrt(c1);
						irOperation irOpe;
						irOpe.operation = "lshiftI";
						irOpe.leftOperands.push_back(l0);
						irOpe.leftOperands.push_back(to_string(shift));
						irOpe.rightOperands.push_back(r0);
						irOpes[j] = irOpe;
					}
				}
				if (operand_vn.find(l0) == operand_vn.end()) {
					operand_vn[l0] = vn;
					vn += 1;
				}
				if (operand_vn.find(l1) == operand_vn.end()) {
					operand_vn[l1] = vn;
					vn += 1;
				}
				string opeLeft = to_string(operand_vn[l0]);
				string opeRight = to_string(operand_vn[l1]);
				string equ = opeLeft + ope + opeRight;
				if (equ_vn.count(equ)) {
					int equVn = equ_vn[equ];
					string vnOpe = vn_operand[equVn];
					if (operand_vn[vnOpe] == equVn) {
						irOperation irOpe;
						irOpe.operation = "i2i";
						irOpe.leftOperands.push_back(vnOpe);
						irOpe.rightOperands.push_back(r0);
						irOpes[j] = irOpe;
					}
					operand_vn[r0] = equVn;
				} else {
					equ_vn[equ] = vn;
					operand_vn[r0] = vn;
					vn_operand[vn] = r0;
					vn++;
				}
			}
		}

	}

}

void UpdateLineNum(vector<irOperation>& irOpes, unordered_map<string, int>& label_lineNum) {
	int lineNum = 1;
	for (auto irOpe : irOpes) {
		irOpe.lineNum = lineNum;
		string irLabel = irOpe.label;
		if (irLabel != "") {
			label_lineNum[irLabel] = irOpe.lineNum;
		}
		lineNum++;
	}
}

int GetLabelMax(vector<irOperation> irOpes) {
	int labelMax = -1;
	for (auto irOpe : irOpes) {
		string irLabel = irOpe.label;
		if (irLabel[0] == 'L') {
			int labelLength = irLabel.length();
			int lv = stoi(irLabel.substr(1, labelLength - 1));
			if (lv > labelMax) {
				labelMax = lv;
			}
		}
	}
	return labelMax;
}

int GetRegisterMax(vector<irOperation> irOpes) {
	int registerMax = -1;
	for (auto irOpe : irOpes) {
		if (irOpe.leftOperands.size() > 0) {
			for (auto lo : irOpe.leftOperands) {
				if (lo[0] != 'r') {
					continue;
				}
				int loLength = lo.length();
				int rv = stoi(lo.substr(1, loLength - 1));
				if (rv > registerMax) {
					registerMax = rv;
				}
			}
		}
		if (irOpe.rightOperands.size() > 0) {
			for (auto ro : irOpe.rightOperands) {
				if (ro[0] != 'r') {
					continue;
				}
				int roLength = ro.length();
				int rv = stoi(ro.substr(1, roLength - 1));
				if (rv > registerMax) {
					registerMax = rv;
				}
			}
		}
	}
	return registerMax;
}

void LoopUnrolling(vector<int> leader, vector<int> last, vector<irOperation>& irOpes) {
	sort(leader.begin(), leader.end());
	sort(last.begin(), last.end());
	vector<irOperation> irOpesNew;
	int labelMax = GetLabelMax(irOpes);
	int registerMax = GetRegisterMax(irOpes);
	int leaderNum = leader.size();

	for (int i = 0; i < leaderNum; i++) {
		//cout << leader[i] << endl;
		//cout << last[i] << endl;
		int begin = leader[i] - 1;
		int end = last[i] - 1;
		if (irOpes[end - 1].operation == "halt") {

			for (int j = begin; j <= end - 1; j++) {
				irOpesNew.push_back(irOpes[j]);
			}

			irOpes.swap(irOpesNew);
			continue;
		}
		if (irOpes[begin].label != "" && irOpes[end].operation == "cbr") {
			if (irOpes[begin].label == irOpes[end].rightOperands[0]) {
				if (irOpes[end - 2].operation == "addI") {
					irOperation irOpe0;
					irOpe0.label = irOpes[begin].label;
					irOpe0.operation = "nop";
					irOpesNew.push_back(irOpe0);

					irOperation irOpe1;
					irOpe1.operation = "sub";
					irOpe1.leftOperands.push_back(irOpes[end - 1].leftOperands[1]);
					irOpe1.leftOperands.push_back(irOpes[end - 1].leftOperands[0]);
					string registerNew = "r" + to_string(++registerMax);
					irOpe1.rightOperands.push_back(registerNew);
					irOpesNew.push_back(irOpe1);

					irOperation irOpe2;
					irOpe2.operation = "rshiftI";
					irOpe2.leftOperands.push_back(registerNew);
					irOpe2.leftOperands.push_back("2");
					irOpe2.rightOperands.push_back(registerNew);
					irOpesNew.push_back(irOpe2);

					irOperation irOpe3;
					irOpe3.operation = "lshiftI";
					irOpe3.leftOperands.push_back(registerNew);
					irOpe3.leftOperands.push_back("2");
					irOpe3.rightOperands.push_back(registerNew);
					irOpesNew.push_back(irOpe3);

					irOperation irOpe4;
					irOpe4.operation = "add";
					irOpe4.leftOperands.push_back(irOpes[end - 1].leftOperands[0]);
					irOpe4.leftOperands.push_back(registerNew);
					registerNew = "r" + to_string(++registerMax);
					irOpe4.rightOperands.push_back(registerNew);
					irOpesNew.push_back(irOpe4);

					irOperation irOpe5;
					irOpe5.operation = "cbr";
					irOpe5.leftOperands.push_back(registerNew);
					string labelNew1 = "L" + to_string(++labelMax);
					irOpe5.rightOperands.push_back(labelNew1);
					string labelNew2 = "L" + to_string(++labelMax);
					irOpe5.rightOperands.push_back(labelNew2);
					irOpesNew.push_back(irOpe5);

					irOperation irOpe6;
					irOpe6.operation = "nop";
					irOpe6.label = labelNew1;
					irOpesNew.push_back(irOpe6);

					for (int n = 0; n < 4; n++) {
						for (int j = begin + 1; j <= end - 2; j++) {
							irOpesNew.push_back(irOpes[j]);
						}
					}

					irOperation irOpe7;
					if (irOpes[end - 1].operation == "cmp_LE") {
						irOpe7.operation = "cmp_LT";
					}
					else if (irOpes[end - 1].operation == "cmp_LT") {
						irOpe7.operation = "cmp_LE";
					}
					irOpe7.leftOperands.push_back(irOpes[end - 1].leftOperands[0]);
					irOpe7.leftOperands.push_back(registerNew);
					irOpe7.rightOperands.push_back(irOpes[end].leftOperands[0]);
					irOpesNew.push_back(irOpe7);

					irOperation irOpe8;
					irOpe8.operation = "cbr";
					irOpe8.leftOperands.push_back(irOpes[end].leftOperands[0]);
					irOpe8.rightOperands.push_back(labelNew1);
					irOpe8.rightOperands.push_back(labelNew2);
					irOpesNew.push_back(irOpe8);

					irOperation irOpe9;
					irOpe9.label = labelNew2;
					irOpe9.operation = "nop";
					irOpesNew.push_back(irOpe9);

					irOpesNew.push_back(irOpes[end - 1]);

					irOperation irOpe10;
					irOpe10.operation = "cbr";
					irOpe10.leftOperands.push_back(irOpes[end].leftOperands[0]);
					string labelNew3 = "L" + to_string(++labelMax);
					irOpe10.rightOperands.push_back(labelNew3);
					irOpe10.rightOperands.push_back(irOpes[end].rightOperands[1]);
					irOpesNew.push_back(irOpe10);

					irOperation irOpe11;
					irOpe11.label = labelNew3;
					irOpe11.operation = "nop";
					irOpesNew.push_back(irOpe11);

					for (int j = begin + 1; j <= end - 2; j++) {
						irOpesNew.push_back(irOpes[j]);
					}

					irOpesNew.push_back(irOpes[end - 1]);
					irOpesNew.push_back(irOpe10);
				}
			} else {

				for (int j = begin; j <= end; j++) {
					irOpesNew.push_back(irOpes[j]);
				}

				continue;
			}
		} else {

			for (int j = begin; j <= end; j++) {
				irOpesNew.push_back(irOpes[j]);
			}

			continue;
		}
	}
}

void WriteIr(vector<irOperation> irOpes, string inputFile, string flag) {
	cout << "write IR" << endl;

	for (auto irOpe : irOpes) {
		string outputFile = "opt" + flag + "_" + inputFile;
		ofstream outfile;
		outfile.open(outputFile, ios::app);
		string ope = irOpe.operation;
		bool isOpe1 = (ope == "not" || ope == "loadI" || ope == "load" || ope == "cload" || ope == "store" || ope == "cstore"
			|| ope == "i2i" || ope == "c2c" || ope == "i2c" || ope == "c2i");
		bool isOpe2 = (ope == "storeAI" || ope == "storeAO" || ope == "cstoreAI" || ope == "cstoreAO");
		bool isOpe3 = (ope == "read" || ope == "cread");
		bool isOpe4 = (ope == "output" || ope == "coutput" || ope == "write" || ope == "cwrite");

		if (ope == "nop") {
			outfile << irOpe.label << ": " << ope << endl;
		} else if (isOpe1) {
			outfile << ope << " " << irOpe.leftOperands[0] << " => " << irOpe.rightOperands[0] << endl;
		} else if (isOpe2) {
			outfile << ope << " " << irOpe.leftOperands[0] << " => " << irOpe.rightOperands[0] << ", " << irOpe.rightOperands[1] << endl;
		} else if (isOpe3) {
			outfile << ope << " => " << irOpe.rightOperands[0] << endl;
		} else if (isOpe4) {
			outfile << ope << " " << irOpe.leftOperands[0] << endl;
		} else if (ope == "br") {
			outfile << ope << " -> " << irOpe.rightOperands[0] << endl;
		} else if (ope == "cbr") {
			outfile << ope << " " << irOpe.leftOperands[0] << " -> " << irOpe.rightOperands[0] << ", " << irOpe.rightOperands[1] << endl;
		} else if (ope == "halt") {
			outfile << ope << endl;
		} else if (ope == "") {
			outfile << ope << endl;
		} else {
			outfile << ope << " " << irOpe.leftOperands[0] << ", " << irOpe.leftOperands[1] << " => " << irOpe.rightOperands[0] << endl;
		}
		outfile.close();
	}

}


int main(int argc, char* argv[]) {
	//cout << argc << endl;
	//cout << argv[1] << endl;
	if (argc < 3 || argc > 4) {
		cout << "wrong arugment number" << endl;
		return -1;
	}

	string flag1 = "";
	string flag2 = "";
	string inputFile = "";

	if (argc == 3) {
		flag1 = argv[1];
		inputFile = argv[2];
	} else if (argc == 4) {
		flag1 = argv[1];
		flag2 = argv[2];
		inputFile = argv[3];
	}

	if (flag1 == "-i" || flag2 == "-i") {
		cout << "-i: code motion not implemented" << endl;
		return -1;
	}

	vector<irOperation> irOpes;
	unordered_map<string, int> label_lineNum;
	ParseIr(inputFile, irOpes, label_lineNum);
	vector<int> leader;
	vector<int> last;
	BuildCfg(leader, last, irOpes, label_lineNum);
	if (argc == 3) {
		if (flag1 == "-v") {
			cout << "value numbering" << endl;
			ValueNumbering(leader, last, irOpes);			
		} else if (flag1 == "-u") {
			cout << "loop unrolling" << endl;
			LoopUnrolling(leader, last, irOpes);
		} else {
			cout << "wrong arugments" << endl;
			return -1;
		}
	} else if (argc == 4) {
		if (flag1 == "-v" && flag2 == "-u") {
			cout << "value numbering & loop unrolling" << endl;
			ValueNumbering(leader, last, irOpes);
			UpdateLineNum(irOpes, label_lineNum);
			vector<int> leader;
			vector<int> last;
			BuildCfg(leader, last, irOpes, label_lineNum);
			LoopUnrolling(leader, last, irOpes);
		} else if (flag1 == "-u" && flag2 == "-v") {
			cout << "loop unrolling & value numbering" << endl;
			LoopUnrolling(leader, last, irOpes);
			UpdateLineNum(irOpes, label_lineNum);
			vector<int> leader;
			vector<int> last;
			BuildCfg(leader, last, irOpes, label_lineNum);
			ValueNumbering(leader, last, irOpes);
		} else {
			cout << "wrong arugments" << endl;
			return -1;
		}
	}
	WriteIr(irOpes, inputFile, flag1 + flag2);

	return 0;
}