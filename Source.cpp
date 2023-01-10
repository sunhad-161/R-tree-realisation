#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

const int m = 2;
const int M = 4;


/*class Point {
public:
	int x;
	int y;
	Point(int a, int b) {
		x = a; y = b;
	}
	int* GetCoords() {
		int a[2] = { x, y };
		return a;
	}
};*/


bool Between(int* l, int x) {

	return l[0] <= x && x <= l[1];
}

class Joint {

public:
	int rect[2][2];
	Joint* child[2];
	bool leaf;

	Joint(int x1, int y1, int x2, int y2){
		rect[0][0] = x1;
		rect[0][1] = y1;
		rect[1][0] = x2;
		rect[1][1] = y2;

		child[0] = NULL;
		child[1] = NULL;

		leaf = true;
	}

	Joint(int x, int y): Joint(x, y, x + 1, y + 1) {}

	Joint(int* a): Joint(a[0], a[1]) {}

	Joint() : Joint(0, 0) {}

	void SetChild(Joint* a){
		if (child[0] == NULL) child[0] = a;
		else child[1] = a;
	}

	int* GetCoords() {
		return *rect;
	}

	Joint* GetChilds() {
		return *child;
	}

	void WriteCoords(string name) {
		int* a = this->GetCoords();
		cout << name << ": (" << a[0] << ", " << a[1] << "), (" << a[2] << ", " << a[3] << ")" << endl;
		if (not this->leaf) {
			child[0]->WriteCoords(name + "L");
			child[1]->WriteCoords(name + "R");
		}
	}

	bool Inside(Joint other) {
		if (rect[0][0] <= other.rect[0][0] && rect[0][1] <= other.rect[0][1] && rect[1][0] >= other.rect[0][0] && rect[1][1] >= other.rect[0][1]) return true;
		else return false;
	}

	bool Equal(Joint other) {
		if (rect[0][0] == other.rect[0][0] && rect[0][1] == other.rect[0][1] && rect[1][0] == other.rect[1][0] && rect[1][1] == other.rect[1][1]) return true;
		else return false;
	}
};

int Distance(Joint d, Joint j) {

	int x = d.rect[0][0];
	int y = d.rect[0][1];

	int a[2] = { j.rect[0][0], j.rect[1][0] };
	int b[2] = { j.rect[0][1], j.rect[1][1] };

	if (Between(a, x)) {
		return min(abs(y - b[0]), abs(y - b[1]));
	}
	else if (Between(b, y)) {
		return min(abs(x - a[0]), abs(x - a[1]));
	}
	else {
		int p = min(abs(y - b[0]), abs(y - b[1]));
		int q = min(abs(x - a[0]), abs(x - a[1]));
		return sqrt(p * p + q * q);
	}
}

class RTree {

public:
	int size = 0;
	Joint* root = NULL;

	void Insert(Joint* new_j) {
		if (size == 0) {
			root = new_j;
			size = 1;
		} else {
			int depth = 1;
			Joint* cur_j = root;
			do {
				Joint* A = new Joint(cur_j->GetCoords());
				if (cur_j->rect[0][0] > new_j->rect[0][0]) cur_j->rect[0][0] = new_j->rect[0][0];
				if (cur_j->rect[0][1] > new_j->rect[0][1]) cur_j->rect[0][1] = new_j->rect[0][1];
				if (cur_j->rect[1][0] < new_j->rect[1][0]) cur_j->rect[1][0] = new_j->rect[1][0];
				if (cur_j->rect[1][1] < new_j->rect[1][1]) cur_j->rect[1][1] = new_j->rect[1][1];

				if (cur_j->leaf) {
					cur_j->child[1] = A;
					cur_j->child[0] = new_j;
					cur_j->leaf = false;
					if (size == depth) size++;
					break;
				} else {
					// if (*cur_j->child[1].Inside(new_j))
					if (Distance(*new_j, *cur_j->child[0]) <= Distance(*new_j, *cur_j->child[1])) cur_j = cur_j->child[0];
					else cur_j = cur_j->child[1];
					depth++;
				}
			} while (depth <= size);
		}
	}

	int Find(Joint f) {
		int index = 1;
		if (size == 0) return 0;
		else {
			Joint* cur_j = root;
			if (cur_j->Equal(f)) return index;
			do {
				if (f.Inside(*cur_j->child[0])) {
					cur_j = cur_j->child[0];
					index = 2 * index + 0;
				}
				else if (f.Inside(*cur_j->child[1])) {
					cur_j = cur_j->child[1];
					index = 2 * index + 1;
				} else return 0;
				if (cur_j->Equal(f)) return index;
			} while (not cur_j->leaf);
		}
		return 0;
	}

	void Delete(Joint f) {
		int tmp = this->Find(f);
		if (!tmp) cout << "Error: joint not found" << endl;
		else this->del(tmp);
	}

	void del(int n) {
		while (n > 1) {
			int p = n % 2;
			n /= 2;
			if (p) cout << "R";
			else cout << "L";
		}
	}

	void WriteTree() {
		cout << "size of tree: " << size << endl;
		if (size > 0) root->WriteCoords("r");
		cout << endl;
	}
};

Joint* Closest(Joint* A, Joint* B, Joint* c) {
	if (Distance(*c, *A) <= Distance(*c, *B)) return A;
	else return B;
}

void MakeTree() {

	RTree tree;
	Joint A(1, 1);
	Joint B(3, 5);
	Joint C(4, 2);
	Joint D(3, 6);

	tree.Insert(&A);
	tree.Insert(&B);
	tree.Insert(&C);
	tree.Insert(&D);
	tree.WriteTree();

	if (tree.Find(Joint(1, 1, 2, 2))) cout << "OK" << endl;
	else cout << "NOT FOUND" << endl;
	if (tree.Find(Joint(1, 1, 2, 3))) cout << "OK" << endl;
	else cout << "NOT FOUND" << endl;

	cout << endl;

	tree.Delete(Joint(1, 1, 2, 2));
}

bool Check() {


	return true;
}

void Test() {

	int test = 1;
	string line = "";
	ifstream in;
	in.open("input.txt");
	vector <string> input;

	if (in.is_open()) {

		while (getline(in, line)) {	
			input.push_back(line);
		}
	}

	in.close();

	ofstream out;
	out.open("output.txt");
	
	if (out.is_open()) {
		for (int i = 0; i < input.size(); i++) {
			stringstream ss;
			ss << input[i];

			vector<int> out_data;
			for (int i = 0; ss >> i; )
			{
				out_data.push_back(i);
			}
		}
	}

	out.close();

	if (Check()) cout << "test " << test << " OK" << endl;
	else cout << "test " << test << " FAIL" << endl;
}

int main(){
	MakeTree();
	// Test();

	return 0;
}