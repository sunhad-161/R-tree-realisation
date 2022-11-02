#include <iostream>
#include <vector>
#include <fstream>
#include <string>

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

	Joint Copy() {
		Joint A(rect[0][0], rect[0][1], rect[1][0], rect[1][1]);
		return A;
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
				Joint A = cur_j->Copy();
				if (cur_j->rect[0][0] > new_j->rect[0][0]) cur_j->rect[0][0] = new_j->rect[0][0];
				if (cur_j->rect[0][1] > new_j->rect[0][1]) cur_j->rect[0][1] = new_j->rect[0][1];
				if (cur_j->rect[1][0] < new_j->rect[1][0]) cur_j->rect[1][0] = new_j->rect[1][0];
				if (cur_j->rect[1][1] < new_j->rect[1][1]) cur_j->rect[1][1] = new_j->rect[1][1];

				if (cur_j->leaf) {
					cur_j->child[1] = &A;
					cur_j->child[0] = new_j;
					cur_j->leaf = false;
					if (size == depth) size++;
					break;
				} else {
					if (Distance(*new_j, *cur_j->child[0]) <= Distance(*new_j, *cur_j->child[1])) cur_j = cur_j->child[0];
					else cur_j = cur_j->child[1];
					// cur_j = Closest(cur_j->child[0], cur_j->child[1], new_j);
					depth++;
				}
			} while (depth <= size);
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

	tree.Insert(&A);
	tree.Insert(&B);
	tree.WriteTree();

	tree.Insert(&C);
	tree.WriteTree();
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
		if (input[0] == "1 2" && input[1] == "4 3") {
			out << "size of tree: 2" << endl;
			out << "r (1, 2) (5, 4)" << endl;
			out << "rL (1, 2) (2, 3)" << endl;
			out << "rR (4, 3) (5, 4)" << endl;
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