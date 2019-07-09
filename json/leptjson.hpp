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

namespace leptjson {
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
		LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET,
		LEPT_PARSE_MISS_KEY,
		LEPT_PARSE_MISS_COLON,
		LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET
	};
	class lept_value;

	class lept_context;

	class lept_member;
}

//==============================
//=== class
//=== lept_value 
class leptjson::lept_value {
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
			lept_value* e_;
			size_t size_, capacity_;
		};
			/* member */
		struct {
			lept_member* m_;
			size_t size_, capacity_;
		};
		double n_;			//8
	};
	lept_type type_{ LEPT_NULL };		//4->8

public:
 	lept_value*& lept_set_e() {
		assert(type_ == LEPT_ARRAY);
		return e_;
	}

	lept_value* lept_get_e()const {
		assert(type_ == LEPT_ARRAY);
		return e_;
	}

	lept_member*& lept_set_m() {
		assert(type_ == LEPT_OBJECT);
		return m_;
	}
	lept_member* lept_get_m()const {
		assert(type_ == LEPT_OBJECT);
		return m_;
	}
	//=====================================
	//=====================================
	//tutorial01
	int lept_parse(const char*json);
	lept_type lept_get_type()const;

	//=====================================
	//=====================================
	//tutorial03
	void lept_free();

	int lept_get_boolean() const;
	void lept_set_boolean(int b);

	double lept_get_number() const;
	void lept_set_number(double n) {
			lept_free();
			n_ = n;
			type_ = LEPT_NUMBER;
	}

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
	lept_value* lept_get_array_element(size_t index) const;


	size_t lept_get_array_size() const{
		assert(this != nullptr && type_ == LEPT_ARRAY);
		return size_;
	}

	//=====================================
	//=====================================
	//tutorial06
	size_t lept_get_object_size() const {
		return size_;
	}
	const char* lept_get_object_key( size_t index)const;
	char*& lept_set_object_key(size_t index);

	size_t& lept_get_object_key_length(size_t index);
	size_t lept_get_object_key_length(size_t index)const;
	lept_value* lept_get_object_value(size_t index)const;

	//=====================================
	//=====================================
	//tutorial07
	char* lept_stringify(size_t* length);

	//=====================================
	//=====================================
	//tutorial08
	size_t lept_find_object_index(const char* _key, size_t _klen) const;
	lept_value* lept_find_object_value(const char* _key, size_t _klen) const;

	void lept_set_object_value(const char* _key, size_t _klen, const lept_value* value);

	void lept_set_array(size_t _capacity);
	size_t lept_get_array_capacity()const {
		assert(this != nullptr && type_ == LEPT_ARRAY);
		return capacity_;
	}
	void lept_reserve_array(size_t _capacity);
	void lept_shrink_array();
	lept_value* lept_pushback_array_element();
	void lept_popback_array_element() {
		assert(this != nullptr && type_ == LEPT_ARRAY &&size_ > 0);
		e_[--size_].lept_free();
	}
	lept_value* lept_insert_array_element(size_t index);
	void lept_erase_array_element(size_t index, size_t count);
	void lept_clear_array();

	void lept_set_object(size_t _capacity);
	const size_t& lept_get_object_capacity();
	void lept_reserve_object(size_t _capacity);
	void lept_shrink_object();
	void lept_clear_object();
	lept_value* lept_set_object_value(const char* _key, size_t _klen);
	void lept_remove_object_value(size_t index);

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
	template <typename T>
	void set_e(T&& t) {
		e_ = t;
	}
};

//==============================
//=== class
//=== lept_context 
class leptjson::lept_context {
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

//==============================
//=== class
//=== lept_member 
class leptjson::lept_member {
public:
	char* k;				/* member key string */
	size_t klen;			/* key string length */
	lept_value v;			/* member value		 */
};

namespace {
	using namespace leptjson;

	int lept_parse_literal(lept_context* c, lept_value* v, const char* literal, lept_type&& type);
	int lept_parse_number(lept_context* c, lept_value* v);

	//refactor ---> int lept_parse_string(lept_context* c, lept_value* v);
	int lept_parse_value(lept_context* c, lept_value* v);
	const char* lept_parse_hex4(const char* p, unsigned& u);
	int lept_parse_array(lept_context* c, lept_value* v);

	//tutorial06
	int lept_parse_string_raw(lept_context* c, char** str, size_t* len);
	int lept_parse_string(lept_context* c, lept_value* v);
	int lept_parse_object(lept_context* c, lept_value* v);

	//tutorial07
	void lept_stringify_value(lept_context* c, const lept_value* v);
	void lept_stringify_string(lept_context* c, const char* s, size_t len);

}

//tutorial08
int lept_is_equal(const lept_value* _lhs, const lept_value* _rhs);

void lept_copy(lept_value* _dst, lept_value* _src);
void lept_move(lept_value* _dst, lept_value* _src);
void lept_swap(lept_value* _lhs, lept_value* _rhs);