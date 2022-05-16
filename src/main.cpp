#include <iostream>
#include <vector>

using namespace std;

template<typename T>
ostream& operator<<(ostream& out, vector<T> V) {
	for (typename vector<T>::iterator it = V.begin(); it != V.end(); it++) {
		out<<*it<<" ";
	}
	return out;
}

int main(/* int argc, char *argv[] */)
{
	std::cout << "Hello world!" << std::endl;
	vector<int> v(4,0);
	vector<int> u(v);
	v[2] = 7;
	cout<<u;
}