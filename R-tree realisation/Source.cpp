#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <typeinfo>

using namespace std;

// минимальное число ключей

const int m = 2;

// максимальное число ключей

const int M = 4;

// входит ли точка в отрезок

bool Between(int* l, int x) {

	return l[0] <= x && x <= l[1];
}

// парсинг строки на числа

int* split(string& s, char delimeter)
{
	// создаём поток из строки s

	stringstream ss(s);
	string item;

	// макс. 4 значения

	int tokens[4];
	int i = 0;

	while (getline(ss, item, delimeter))
	{
		tokens[i] = stoi(item);
		i++;
	}

	return tokens;
}


// описываем класс прям-ка

class Joint {

public:

	// координаты прям-ка

	int rect[2][2];

	// ссылки на дочерние прям-ки

	Joint* child[2];

	// является ли листом

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

	Joint(int x, int y): Joint(x, y, x, y) {}

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

	/*
	vector<int[4]> GetJoints(){

		vector<int[4]> Y;

		int* coor = this->GetCoords();

		int y[4] = { coor[0], coor[1], coor[2], coor[3] };

		Y.push_back(y);

		if (!leaf) {

			vector<int[4]> A = this->child[0]->GetJoints();
			vector<int[4]> B = this->child[1]->GetJoints();

			Y.insert(Y.end(), A.begin(), A.end());
			Y.insert(Y.end(), B.begin(), B.end());
		}

		return Y;
	}
	*/

	// вывод координат в терминал

	void WriteCoords(string name) {

		int* a = this->GetCoords();

		// пробегаем дерево через центр-лево-право

		if (a[0] == a[2] && a[1] == a[3]) cout << name << ": p(" << a[0] << ", " << a[1] << ")" << endl;
		else cout << name << ": (" << a[0] << ", " << a[1] << "), (" << a[2] << ", " << a[3] << ")" << endl;

		if (child[0] != nullptr) child[0]->WriteCoords(name + "L");
		if (child[1] != nullptr) child[1]->WriteCoords(name + "R");
	}

	// this <= other ?

	bool Inside(Joint other) {

		int* a = this->GetCoords();

		return a[0] >= other.rect[0][0] && a[1] >= other.rect[0][1] && a[2] <= other.rect[1][0] && a[3] <= other.rect[1][1];
	}

	// this == other ?

	bool Equal(Joint other) {

		int* a = this->GetCoords();

		return a[0] == other.rect[0][0] && a[1] == other.rect[0][1] && a[2] == other.rect[1][0] && a[3] == other.rect[1][1];
	}
};

// расстояние между прям-ками

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
		return (int)sqrt(p * p + q * q);
	}
}

// описываем класс R-дерева

class RTree {

public:

	// высота дерева

	int size = 0;

	// ссылка на корень

	Joint* root = NULL;

	void Insert(Joint* new_j) {

		if (size == 0) { // пустое дерево
			root = new_j;
			size = 1;
		} else { // у дерева есть корень

			int depth = 1;

			// начинаем перебор с корня

			Joint* cur_j = root;

			do {

				// делаем копию A

				Joint* A = new Joint(cur_j->GetCoords());

				// меняем текущий прям-ник

				if (cur_j->rect[0][0] > new_j->rect[0][0]) cur_j->rect[0][0] = new_j->rect[0][0];
				if (cur_j->rect[0][1] > new_j->rect[0][1]) cur_j->rect[0][1] = new_j->rect[0][1];
				if (cur_j->rect[1][0] < new_j->rect[1][0]) cur_j->rect[1][0] = new_j->rect[1][0];
				if (cur_j->rect[1][1] < new_j->rect[1][1]) cur_j->rect[1][1] = new_j->rect[1][1];

				if (cur_j->leaf) {

					// скопированный прям-ник A вставляем в измененный текущий

					cur_j->child[1] = A;

					// новый тоже вставляем

					cur_j->child[0] = new_j;

					// текущий прям-ник больше не является листом

					cur_j->leaf = false;

					if (size == depth) size++;

					break;
				} else {

					// ищем ближайший прям-ник

					if (Distance(*new_j, *cur_j->child[0]) <= Distance(*new_j, *cur_j->child[1])) cur_j = cur_j->child[0];
					else cur_j = cur_j->child[1];

					depth++;
				}
			} while (depth <= size);
		}
	}

	// алгоритм поиска с возвратом индекса, где 1 - корень, 2 - прям-ник 2 уровня слева
	// 3 - прям-ник 2 уровня справа, 0 - прям-ник не обнаружен

	int Find(Joint f) {

		int index = 1;

		if (size == 0) return 0; // дерево пустое
		else {

			Joint* cur_j = root;

			do {

				if (cur_j->Equal(f)) return index;

				if (f.Inside(*cur_j->child[0])) {

					cur_j = cur_j->child[0];
					index = 2 * index + 0;
				}

				else if (f.Inside(*cur_j->child[1])) {

					cur_j = cur_j->child[1];
					index = 2 * index + 1;
				}
				
				else return 0;

			} while (not cur_j->leaf);

		}

		return 0;
	}

	// удаление (проверка на наличие прям-ка)

	void Delete(Joint f) {

		int tmp = this->Find(f);

		if (!tmp) cout << "Error: joint not found" << endl;
		else this->del(tmp);
	}

	// алгоритм удаления

	void del(int n) {

		// сюда записываем путь, где 0 - L, 1 - R

		vector<bool> path;

		while (n > 1) {
			int p = n % 2;
			n /= 2;
			path.push_back(p);
		}

		int s = path.size();

		Joint* cur_j = root;

		// доходим до нужного узла

		for (int i = s - 1; i >= 1; i--) cur_j = cur_j->child[path[i]];

		// обнуляем ссылку на дочерний узел

		cur_j->child[path[0]] = nullptr;
	}

	// вывод дерева в терминал

	void WriteTree() {

		cout << "size of tree: " << size << endl << endl << "structure:" << endl;
		if (size > 0) root->WriteCoords("r");
		cout << endl;
	}

	// возврат дерева в скалярном формате

	/*
	vector<int[4]> GetTree() {

		return root->GetJoints();
	}
	*/
};

// ближайший из двух

Joint* Closest(Joint* A, Joint* B, Joint* c) {

	if (Distance(*c, *A) <= Distance(*c, *B)) return A;
	else return B;
}

void MakeTree() {

	RTree tree;

	// int* R = tree.GetTree();
	// cout << R[0] << ' ' << R[1] << endl;

	Joint A(1, 1);
	Joint B(3, 5);
	Joint C(4, 2);
	Joint D(3, 6);

	tree.Insert(&A);
	tree.Insert(&B);
	tree.Insert(&C);
	tree.Insert(&D);
	tree.WriteTree();

	if (tree.Find(Joint(1, 1))) cout << "OK" << endl;
	else cout << "NOT FOUND" << endl;
	if (tree.Find(Joint(3, 5, 3, 6))) cout << "OK" << endl;
	else cout << "NOT FOUND" << endl;
	if (tree.Find(Joint(2, 3))) cout << "OK" << endl;
	else cout << "NOT FOUND" << endl;

	cout << endl;

	tree.Delete(Joint(1, 1));

	tree.WriteTree();
}

bool Check() {


	return true;
}

void Simple_test() {

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

	RTree tree;

	for (string i : input) {

		int* numbers = split(i, ' ');

		int a = numbers[0];
		int b = numbers[1];

		Joint* A = new Joint(a, b);
		tree.Insert(A);
	}

	tree.WriteTree();
}

void Test() {

	int test_time = 0;
	int min_time = NULL;
	int max_time = 0;

	int test = 1;
	string range;
	fstream form;
	fstream cursor;

	form.open("test.txt", fstream::in);
	cursor.open("input.txt", fstream::out);
	getline(form, range);
	int max_test = stoi(range);

	for (; test <= max_test;) {

		getline(form, range);

		if (range != "0") cursor << range << endl;
		else {

			cursor.close();

			clock_t begin = clock();

			Simple_test();

			clock_t end = clock();

			int time = end - begin;
			if (min_time == NULL || min_time > time) min_time = time;
			if (max_time < time) max_time = time;

			if (Check()) cout << "test " << test << " OK" << endl;
			else cout << "test " << test << " FAIL" << endl;

			test_time += (end - begin);
			cout << "time: " << (double)time / CLOCKS_PER_SEC << "s" << endl << endl;
			cout << "-----press ENTER to continue-----" << endl << endl;

			cin.get();

			test++;
			cursor.open("input.txt", fstream::out);
		}

	}

	form.close();

	cout << "minimum time: " << (double)min_time / CLOCKS_PER_SEC << endl;
	cout << "maximum time: " << (double)max_time / CLOCKS_PER_SEC << endl;
	cout << "middle time: " << (double)(test_time / max_test) / CLOCKS_PER_SEC << endl;
}

int main(){

	// MakeTree();
	Test();
	// Simple_test();

	return 0;
}