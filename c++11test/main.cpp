//#include <iostream>
//using namespace std;
//
//class A {
//private:
//	const char* a;
//public:
//	const char*& get_a() {
//		return a;
//	}
//	void set(const char* j) {
//		a = j;
//	}
//};
//
//void point(const char* a) {
//	const char tmp[] = "true";
//	a = tmp;
//}
//
//int main(void) {
//
//	A t;
//	const char b[] = "asdasd";
//	t.set(b);
//
//	auto i = t.get_a();
//	i++;
//	t.get_a()++;
//	std::cout << t.get_a() << std::endl;
//	const char* a;
//	point(a);
//	cout << a << endl;
//	return 0;
//}
#include <iostream>
class A {
	union {
		struct {
			char* c;
		};
	};

};
void fun(A* tmp) {
		//like this,note me can't access,but have no error
		tmp->c = new char('a');
}

static union a{	// --------------12->16
	struct
	{
		/* data */
		int s_;		//8
		unsigned int len_;	//4->8
	};
	struct {
		int e_;
		unsigned int size_;
	};
	struct {
		int* m;
		unsigned int size_;
	};
};

int main(void) {

	A t;
	fun(&t);
	//std::cout << t.c<<std::endl;

	a b;
	std::cout << std::endl;
	b.m = new int(0x01020304);
	b.size_ = 0x05050505;
	std::cout << &b << std::endl;
	std::cout << sizeof(b) << std::endl;
	std::cout << &*b.m << std::endl;
	std::cout << b.size_ << std::endl;
	delete b.m;
	return 0;
}