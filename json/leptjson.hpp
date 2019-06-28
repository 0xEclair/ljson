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
#include <string>	/*   memcpy     */
#define lept_set_null lept_free

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
		LEPT_PARSE_INVALID_STRING_CHAR
	};
	//=== class
	//=== lept_value 
	class lept_value;

	//=== class
	//=== lept_context 
	class lept_context;
}

//==============================
//=== class
//=== lept_value 
class lept::lept_value {
	union {
		struct
		{
			char* s_;
			size_t len_;
			/* data */
		};
		double n_;
	};
	lept_type type_;

	//=====================================
	//tutorial03

public:

	//=====================================
	//=====================================
	int lept_parse(const char*json);
	lept_type lept_get_type();

	//=====================================
	//=====================================
	//tutorial02
#if 0
	double lept_get_number();
#endif
	//=====================================
	//=====================================
	//tutorial03
	void lept_free();

	int lept_get_boolean();
	void lept_set_boolean(int b);

	double lept_get_number();
	template <class T>
	void lept_set_number(T&& n);

	const char* lept_get_string() {
		assert(this != nullptr && type_ == LEPT_STRING);
		return s_;
	}
	size_t lept_get_string_length() {
		assert(this != nullptr && type_ == LEPT_STRING);
		return len_;
	}
	void lept_set_string(const char*s, size_t len);
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
	// 设置n_
	template <typename T>
	void set_n(T&& t) {
		n_ = t;
	}
	const double& get_n()const {
		return n_;
	}
};

//==============================
//=== class
//=== lept_context 
class lept::lept_context {

public:
	const char* json_;
	// 动态堆栈
	char* stack_;
	//堆栈容量
	size_t size_;
	//栈顶位置
	size_t top_;

	// 解析空白
	void lept_parse_whitespace();
	// 解析第二部分的空白
	int lept_parse_whitespace_second();
	// ==
	// toturial03
	void* lept_context_push(size_t size);
	void* lept_context_pop(size_t size);

};

namespace {
	using namespace lept;

	int lept_parse_literal(lept_context* c, lept_value* v, const char* literal, lept_type&& type);

	int lept_parse_number(lept_context* c, lept_value* v);

	int lept_parse_string(lept_context* c, lept_value* v);

	int lept_parse_value(lept_context* c, lept_value* v);
}