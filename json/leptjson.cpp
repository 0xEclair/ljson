/*		leptjson.cpp		*/
#include "leptjson.hpp"
#ifdef _WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

using std::string;
//=======================================================
//toturial01
#define EXPECT(c,ch) \
		do {\
			assert(*c->json_ == ch);\
			c->json_++;\
		} while (0)

//=======================================================
//toturial02
#define ISDIGIT(ch) ((ch)>='0' && (ch)<='9')
#define ISDIGIT1TO9(ch) ((ch)>='1' && (ch)<='9')

//=======================================================
//toturial03
#define lept_init() \
	do{\
		type_=LEPT_NULL;\
	}while(0)

#ifndef LEPT_PARSE_STACK_INIT_SIZE
#define LEPT_PARSE_STACK_INTI_SIZE 256
#endif

#define PUTC(c,ch) \
	do{\
		*(char*)c->lept_context_push(sizeof(char))=(ch);\
	}while(0)

//无名命名空间
//只能在文件内调用
//暂时只解析符号
namespace {

	//==========
	//tutorial01
	//解析null
	//==========
	//tutorial02
	int lept_parse_literal(lept_context* c, lept_value* v, const char* literal,lept_type&& type) {
		size_t i;
		EXPECT(c, literal[0]);
		for (i = 0; literal[i + 1]; ++i) {
			if (c->json_[i] != literal[i + 1]) {
				return LEPT_PARSE_INVALID_VALUE;
			}
		}
		c->json_ += i;
		v->set_type(type);
		return LEPT_PARSE_OK;
	}

	int lept_parse_number(lept_context* c, lept_value* v) {
		const char* p = c->json_;
		if (*p == '-') {
			++p;
		}

		if (*p == '0') {
			++p;
		}
		else {
			if (!ISDIGIT1TO9(*p)) {
				return LEPT_PARSE_INVALID_VALUE;
			}
			//暗含ISDIGIT1TO9(*p)==true
			for (++p; ISDIGIT(*p); ++p);
		}

		if (*p == '.') {
			p++;
			if (!ISDIGIT(*p)) {
				return LEPT_PARSE_INVALID_VALUE;
			}
			for (++p; ISDIGIT(*p); ++p);
		}

		if (*p == 'e' || *p == 'E') {
			p++;
			if (*p == '+' || *p == '-') {
				p++;
			}
			if (!ISDIGIT(*p)) {
				return LEPT_PARSE_INVALID_VALUE;
			}
			for (++p; ISDIGIT(*p); ++p);
		}
		errno = 0;
		v->lept_set_number(strtod(c->json_, NULL));
		if (errno == ERANGE && (v->lept_get_number() == HUGE_VAL || v->lept_get_number() == -HUGE_VAL)) {
			v->set_type(LEPT_NULL);			/* 教程里面没写这步 
											因为上面lept_set_number会改变type_为number类型  */
			return LEPT_PARSE_NUMBER_TOO_BIG;
		}
		v->set_type(LEPT_NUMBER) ;
		c->json_ = p;
		return LEPT_PARSE_OK;

	}

	int lept_parse_string(lept_context* c, lept_value* v) {
		size_t head = c->top_, len;
		const char* p;
		EXPECT(c, '\"');
		p = c->json_;
		for (;;) {
			char ch = *p++;
			switch (ch) {
			case '\"':
				len = c->top_ - head;
				v->lept_set_string((const char*)c->lept_context_pop(len), len);
				c->json_ = p;
				return LEPT_PARSE_OK;
			case '\0':
				c->top_ = head;
				return LEPT_PARSE_MISS_QUOTATION_MARK;
			case '\\':
				switch (*p++) {
					case '\"':PUTC(c, '\"'); break;
					case '\\':PUTC(c, '\\'); break;
					case '/':PUTC(c, '/'); break;
					case 'b':PUTC(c, '\b'); break;
					case 'f':PUTC(c, '\f'); break;
					case 'n':PUTC(c, '\n'); break;
					case 'r':PUTC(c, '\r'); break;
					case 't':PUTC(c, '\t'); break;
					case 'u':
					default:
						c->top_ = head;
						return LEPT_PARSE_INVALID_STRING_ESCAPE;
					}
				break;
			default:
				if ((unsigned char)ch < 0x20) {
					c->top_ = head;
					return LEPT_PARSE_INVALID_STRING_CHAR;
				}
				PUTC(c, ch);
			}
		}
	}

	//解析value
	int lept_parse_value(lept_context* c, lept_value* v) {
		switch (*c->json_) {
		case 't': return lept_parse_literal(c, v, "true", LEPT_TRUE);
		case 'f': return lept_parse_literal(c, v, "false", LEPT_FALSE);
		case 'n': return lept_parse_literal(c, v, "null", LEPT_NULL);
		default:  return lept_parse_number(c, v);
		case '"':  return lept_parse_string(c, v);
		case '\0':return LEPT_PARSE_EXPECT_VALUE;
		}
	}


}

namespace lept {
	//=====================================================
	// lept_context
	void lept_context::lept_parse_whitespace() {
		const char* p = json_;
		while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
			p++;
		}
		json_ = p;
	}

	int lept_context::lept_parse_whitespace_second() {
		const char* p = json_;
		while (*p != '\0') {
			if (*p != ' ' || *p != '\t' || *p != '\n' || *p != '\r') {
				return LEPT_PARSE_ROOT_NOT_SINGULAR;
			}
			p++;
		}
		json_ = p;
		return 0;
	}

	void* lept_context::lept_context_push(size_t size) {
		void* res;
		assert(size > 0);

		if (top_ + size >= size_) {
			if (size_ == 0) {
				size_ = LEPT_PARSE_STACK_INTI_SIZE;
			}
			while (top_ + size >= size_) {
				size_ += size_ >> 1;	/* size_ * 1.5 */
			}
			/*
			如果stack_=nullptr
			tmp也等于nullptr
			模仿realloc()
			*/
			auto tmp = stack_;
			stack_ = new char[size_];
			if (tmp != nullptr) {
				memcpy(stack_, tmp, sizeof(tmp));
				delete[] tmp;
			}
		}
		res = stack_ + top_;
		top_ += size;
		return res;
	}

	void* lept_context::lept_context_pop(size_t size) {
		assert(top_ >= size);
		return stack_ + (top_ -= size);
	}

	//=====================================================
	// lept_value
	int lept_value::lept_parse(const char* json) {
		lept_context c;
		assert(this != nullptr);
		c.json_ = json;
		c.stack_ = nullptr;
		c.size_ = c.top_ = 0;
		//若 lept_parse() 失败，会把 v 设为 null 类型
		//所以这里先把它设为 null
		//让 lept_parse_value() 写入解析出来的根值。
		lept_init();
		c.lept_parse_whitespace();
		int res = lept_parse_value(&c, this);
		if (res == LEPT_PARSE_OK) {
			c.lept_parse_whitespace();
			if (*c.json_ != '\0') {
				type_ = LEPT_NULL;
				res = LEPT_PARSE_ROOT_NOT_SINGULAR;
			}
		}
		assert(c.top_ == 0);
		delete[] c.stack_;
		return res;
	}

	lept_type lept_value::lept_get_type() {
		assert(this != nullptr);
		return type_;
	}

	//==========================================================================================================
	//tutorial02
	double lept::lept_value::lept_get_number() const{
		assert(this != nullptr && type_ == LEPT_NUMBER);
		return n_;
	}

	//==========================================================================================================
	//tutorial03
	void lept_value::lept_set_string(const char* s, size_t len) {
		assert(this != nullptr && (s != nullptr || len == 0));
		lept_free();
		s_ = new char[len + 1];
		memcpy(s_, s, len);
		s_[len] = '\0';
		len_ = len;
		set_type(LEPT_STRING);
	}

	void lept_value::lept_free() {
		assert(this != nullptr);
		if (type_ == LEPT_STRING) {
			delete[] s_;
		}
		type_ = LEPT_NULL;
	}

	int lept_value::lept_get_boolean()const {
		assert(this!= NULL && (type_ == LEPT_TRUE || type_ == LEPT_FALSE));		
		return type_==LEPT_TRUE;
	}

	void lept_value::lept_set_boolean(int b) {
		lept_free();
		set_type((b ? LEPT_TRUE : LEPT_FALSE));
	}

	template <class T>
	void lept_value::lept_set_number(T&& n) {
		lept_free();
		n_ = n;
		type_ = LEPT_NUMBER;
	}
}