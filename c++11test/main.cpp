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
static union a {	// --------------12->16
	struct
	{
		/* data */
		int s_;		//8
		size_t len_;	//4->8
	};
	struct {
		int e_;
		size_t size_;
	};
	struct {
		int* m;
		size_t size_;
	};
};

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
		tmp->c = new char('c');
}

int main(void) {
	A t;
	fun(&t);
	//std::cout << t.c<<std::endl;
	delete t.c;

	a b;
	b.m = new int(1321);
	b.size_ = 0x05050505;
	std::cout << &b << std::endl;
	std::cout << sizeof(b) << std::endl;
	std::cout << &*b.m << std::endl;
	std::cout << b.size_ << std::endl;
	delete b.m;


	_CrtDumpMemoryLeaks();
	return 0;
}