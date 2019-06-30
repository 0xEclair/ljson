#pragma once
/*		leptjson.hpp		*/
//==========================================================================================================
//tutorial01
#include <assert.h>	/* assert() */
#include <stdlib.h>	/* nullptr strtod() */
//tutorial02
#include <errno.h>   /* errno, ERANGE */
#include <math.h>    /* HUGE_VAL */
//tutorial03
#include <string>	/*   memcpy()     */
#define lept_set_null lept_free

/* c++中size_t是 unsigned long long  */	
#define size_t unsigned int

namespace lept {
	enum lept_type {
		LEPT_NULL,
		LEPT_FALSE,
		LEPT_TRUE,
		LEPT_NUMBER,
		LEPT_STRING,
		LEPT_ARRAY,
		LEPT_OBJECT
	};

	enum {
		LEPT_PARSE_OK = 0,
		LEPT_PARSE_EXPECT_VALUE,
		LEPT_PARSE_INVALID_VALUE,
		LEPT_PARSE_ROOT_NOT_SINGULAR,
		LEPT_PARSE_NUMBER_TOO_BIG,
		LEPT_PARSE_MISS_QUOTATION_MARK,
		LEPT_PARSE_INVALID_STRING_ESCAPE,
		LEPT_PARSE_INVALID_STRING_CHAR,
		LEPT_PARSE_INVALID_UNICODE_HEX,
		LEPT_PARSE_INVALID_UNICODE_SURROGATE,
		LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET
	};
	class lept_value;

	class lept_context;
}

//==============================
//=== class
//=== lept_value 
class lept::lept_value {
	//8bit对齐
	union {	// --------------12->16
		struct
		{
			char* s_;		//8
			size_t len_;	//4->8
			/* data */
		};
		struct {

			size_t size_;
		};
		double n_;			//8
	};
	lept_type type_{ LEPT_NULL };		//4->8
public:
	lept_value* e_;
	//=====================================
	//=====================================
	//tutorial01
	int lept_parse(const char*json);
	lept_type lept_get_type();

	//=====================================
	//=====================================
	//tutorial03
	void lept_free();

	int lept_get_boolean() const;
	void lept_set_boolean(int b);

	double lept_get_number() const;
	template <class T>
	void lept_set_number(T&& n);

	const char* lept_get_string() const {
		assert(this != nullptr && type_ == LEPT_STRING);
		return s_;
	}
	size_t lept_get_string_length() const {
		assert(this != nullptr && type_ == LEPT_STRING);
		return len_;
	}
	void lept_set_string(const char*s, size_t len);
	//=====================================
	//=====================================
	//tutorial05
	size_t lept_get_array_size() const{
		assert(this != nullptr && type_ == LEPT_ARRAY);
		return size_;
	}

	lept_value* lept_get_array_element(size_t index);
	//=====================================
	//=====================================
	//修改/获得private变量接口
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
	template <typename T>
	void set_e(T&& t) {
		e_ = t;
	}
};

//==============================
//=== class
//=== lept_context 
class lept::lept_context {
public:
	const char* json_;		//8
	// 动态堆栈
	char* stack_;			//8
	//堆栈容量
	size_t size_;			//4
	//栈顶位置
	size_t top_;			//4

	// 解析空白
	void lept_parse_whitespace();
	// 解析第二部分的空白
	int lept_parse_whitespace_second();
	// ==
	// toturial03
	void* lept_context_push(size_t size);
	void* lept_context_pop(size_t size);
	// tutorial04
	void lept_encode_utf8(const unsigned& u);
};

namespace {
	using namespace lept;

	int lept_parse_literal(lept_context* c, lept_value* v, const char* literal, lept_type&& type);
	int lept_parse_number(lept_context* c, lept_value* v);
	int lept_parse_string(lept_context* c, lept_value* v);
	int lept_parse_value(lept_context* c, lept_value* v);
	const char* lept_parse_hex4(const char* p, unsigned& u);
	int lept_parse_array(lept_context* c, lept_value* v);
}