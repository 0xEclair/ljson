#include <iostream>
using namespace std;

class A {
private:
	const char* a;
public:
	const char*& get_a() {
		return a;
	}
	void set(const char* j) {
		a = j;
	}
};

void point(const char* a) {
	const char tmp[] = "true";
	a = tmp;
}

int main(void) {

	A t;
	const char b[] = "asdasd";
	t.set(b);

	auto i = t.get_a();
	i++;
	t.get_a()++;
	std::cout << t.get_a() << std::endl;
	const char* a;
	point(a);
	cout << a << endl;
	return 0;
}