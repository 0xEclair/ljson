#pragma once
/*		ljson.h		*/
/*     "l" is lept   */
#include <assert.h>	/* assert() */
#include <stdlib.h>	/* nullptr strtod() */
#include <errno.h>   /* errno, ERANGE */
#include <math.h>    /* HUGE_VAL */
#include <string>	/*   memcpy()     */

/* c++中size_t是 unsigned long long  */	
#define size_t unsigned int

#define set_null free

namespace ljson {
	enum type {
		LEPT_NULL,
		LEPT_FALSE,
		LEPT_TRUE,
		LEPT_NUMBER,
		LEPT_STRING,
		LEPT_ARRAY,
		LEPT_OBJECT
	};

	enum {
		PARSE_OK = 0,
		PARSE_EXPECT_VALUE,
		PARSE_INVALID_VALUE,
		PARSE_ROOT_NOT_SINGULAR,
		PARSE_NUMBER_TOO_BIG,
		PARSE_MISS_QUOTATION_MARK,
		PARSE_INVALID_STRING_ESCAPE,
		PARSE_INVALID_STRING_CHAR,
		PARSE_INVALID_UNICODE_HEX,
		PARSE_INVALID_UNICODE_SURROGATE,
		PARSE_MISS_COMMA_OR_SQUARE_BRACKET,
		PARSE_MISS_KEY,
		PARSE_MISS_COLON,
		PARSE_MISS_COMMA_OR_CURLY_BRACKET
	};
	class value;class context;class member;

	int equal(const value* _lhs, const value* _rhs);

	void copy(value* _dst, value* _src);
	void move(value* _dst, value* _src);
	void swap(value* _lhs, value* _rhs);
}

//==============================
//=== class
//=== value 
class ljson::value {
	//8bit对齐
	union {	// --------------12->16
		struct
		{
			/* data */
			char* s_;		//8
			size_t len_;	//4->8
		};
			/* array */
		struct {
			value* e_;
			size_t size_, capacity_;
		};
			/* member */
		struct {
			member* m_;
			size_t size_, capacity_;
		};
		double n_;			//8
	};
	type type_{ LEPT_NULL };		//4->8

public:
 	value*& set_e() {
		assert(type_ == LEPT_ARRAY);
		return e_;
	}

	value* get_e()const {
		assert(type_ == LEPT_ARRAY);
		return e_;
	}

	member*& set_m() {
		assert(type_ == LEPT_OBJECT);
		return m_;
	}
	member* get_m()const {
		assert(type_ == LEPT_OBJECT);
		return m_;
	}
	//=====================================
	//=====================================
	//tutorial01
	int parse(const char*json);
	type get_type()const;

	//=====================================
	//=====================================
	//tutorial03
	void free();

	int get_boolean() const;
	void set_boolean(int b);

	double get_number() const;
	void set_number(double n) {
			free();
			n_ = n;
			type_ = LEPT_NUMBER;
	}

	const char* get_string() const {
		assert(this != nullptr && type_ == LEPT_STRING);
		return s_;
	}
	size_t get_string_length() const {
		assert(this != nullptr && type_ == LEPT_STRING);
		return len_;
	}
	void set_string(const char*s, size_t len);
	//=====================================
	//=====================================
	//tutorial05
	value* get_array_element(size_t index) const;


	size_t get_array_size() const{
		assert(this != nullptr && type_ == LEPT_ARRAY);
		return size_;
	}

	//=====================================
	//=====================================
	//tutorial06
	size_t get_object_size() const {
		return size_;
	}
	const char* get_object_key( size_t index)const;
	char*& set_object_key(size_t index);

	size_t& get_object_key_length(size_t index);
	size_t get_object_key_length(size_t index)const;
	value* get_object_value(size_t index)const;

	//=====================================
	//=====================================
	//tutorial07
	char* stringify(size_t* length);

	//=====================================
	//=====================================
	//tutorial08
	size_t find_object_index(const char* _key, size_t _klen) const;
	value* find_object_value(const char* _key, size_t _klen) const;

	void set_array(size_t _capacity);
	size_t get_array_capacity()const {
		assert(this != nullptr && type_ == LEPT_ARRAY);
		return capacity_;
	}
	void reserve_array(size_t _capacity);
	void shrink_array();
	value* pushback_array_element();
	void popback_array_element() {
		assert(this != nullptr && type_ == LEPT_ARRAY &&size_ > 0);
		e_[--size_].free();
	}
	value* insert_array_element(size_t index);
	void erase_array_element(size_t index, size_t count);
	void clear_array();

	void set_object(size_t _capacity);
	const size_t& get_object_capacity()const;
	void reserve_object(size_t _capacity);
	void shrink_object();
	void clear_object();
	value* set_object_value(const char* _key, size_t _klen);
	void remove_object_value(size_t index);

	//=====================================
	//=====================================
	// 修改/获得private变量接口
	// 设置type_
	template <typename T>
	void set_type(T&& t) {
		//左值 -> T& 右值 -> T&&
		//没有把引用作参数，所以不用转发
		type_ = t;
	}
	template <typename T>
	void set_size(T&& t) {
		size_ = t;
	}
};

//==============================
//=== class
//=== context 
class ljson::context {
public:
	const char* json_;		//8
	// 动态堆栈
	char* stack_;			//8
	//堆栈容量
	size_t size_;			//4
	//栈顶位置
	size_t top_;			//4

	// 解析空白
	void ParseWhitespace();
	// 解析第二部分的空白
	int ParseSecondWhitespace();
	// ==
	// toturial03
	void* push(size_t size);
	void* pop(size_t size);
	// tutorial04
	void EncodeUtf8(const unsigned& u);
};

//==============================
//=== class
//=== member 
class ljson::member {
public:
	char* k;				/* member key string */
	size_t klen;			/* key string length */
	value v;				/* member value		 */
};

namespace {
	using namespace ljson;

	int ParseLiteral(context* c, value* v, const char* literal, type&& type);
	int ParseNumber(context* c, value* v);

	//refactor ---> int ParseString(context* c, value* v);
	int ParseValue(context* c, value* v);
	const char* ParseHex4(const char* p, unsigned& u);
	int ParseArray(context* c, value* v);

	//tutorial06
	int ParseStringRaw(context* c, char** str, size_t* len);
	int ParseString(context* c, value* v);
	int ParseObject(context* c, value* v);

	//tutorial07
	void StringifyValue(context* c, const value* v);
	void StringifyString(context* c, const char* s, size_t len);

}

