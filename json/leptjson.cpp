/*		leptjson.cpp		*/
//==========================================================================================================
//tutorial01
#include "leptjson.hpp"
#include <assert.h>	/* assert() */
#include <stdlib.h>	/* nullptr strtod() */
//tutorial02
#include <errno.h>   /* errno, ERANGE */
#include <math.h>    /* HUGE_VAL */
//tutorial03
#include <memory>	/*   memcpy     */
using namespace lept;

#define EXPECT(c,ch) \
		do {\
			assert(*c->json_ == ch);\
			c->json_++;\
		} while (0)

#define lept_init() \
	do{\
		set_type(LEPT_NULL);\
	}while(0)

#define ISDIGIT(ch) ((ch)>='0' && (ch)<='9')
#define ISDIGIT1TO9(ch) ((ch)>='1' && (ch)<='9')

//无名命名空间
//只能在文件内调用
//暂时只解析符号
namespace {
	//==========
	//tutorial01
	//解析null
	#if 0
	int lept_parse_null(lept_context* c, lept_value* v) {
		EXPECT(c, 'n');
		if (c->json_[0] != 'u' || c->json_[1] != 'l' || c->json_[2] != 'l') {
			return LEPT_PARSE_INVALID_VALUE;
		}
		c->json_ += 3;
		v->set_type(LEPT_NULL);
		return LEPT_PARSE_OK;
	}
	//解析true
	int lept_parse_true(lept_context* c, lept_value* v) {
		EXPECT(c, 't');
		if (c->json_[0] != 'r' || c->json_[1] != 'u' || c->json_[2] != 'e') {
			return LEPT_PARSE_INVALID_VALUE;
		}
		c->json_ += 3;
		v->set_type(LEPT_TRUE);
		return LEPT_PARSE_OK;
	}

	//解析false
	int lept_parse_false(lept_context* c, lept_value* v) {
		EXPECT(c, 'f');
		if (c->json_[0] != 'a' || c->json_[1] != 'l' || c->json_[2] != 's'||c->json_[3]!='e') {
			return LEPT_PARSE_INVALID_VALUE;
		}
		c->json_ += 4;
		v->set_type(LEPT_FALSE);
		return LEPT_PARSE_OK;
	}
	#endif
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
		////检测首位合法性
		//if (*c->json_ != '-' && !(ISDIGIT(*c->json_))) {
		//	return LEPT_PARSE_INVALID_VALUE;
		//}
		////检测0后字符的合法性
		//if (*c->json_ == '0') {
		//	if (c->json_[1] != '.' && c->json_[1] != 'e' && c->json_[1] != 'E' && c->json_[1] != '\0') {
		//		return LEPT_PARSE_ROOT_NOT_SINGULAR;
		//	}
		//}
		//const char* tmp = c->json_;
		//while (*(++tmp) != '\0') {
		//	if (*tmp == '.' && !ISDIGIT(tmp[1])) {
		//			return LEPT_PARSE_INVALID_VALUE;
		//		}
		//}

		//char* end;
		///* \TODO validate number */
		//errno = 0;
		//v->set_n(strtod(c->json_, &end));
		//if (c->json_ == end) {
		//	return LEPT_PARSE_INVALID_VALUE;
		//}
		//c->json_ = end;
		//if (errno == ERANGE && (v->get_n() == HUGE_VAL|| v->get_n() == -HUGE_VAL)) {
		//	return LEPT_PARSE_NUMBER_TOO_BIG;
		//}
		//v->set_type(LEPT_NUMBER);
		//return LEPT_PARSE_OK;
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
		v->set_n(strtod(c->json_, NULL));
		if (errno == ERANGE && (v->get_n() == HUGE_VAL || v->get_n() == -HUGE_VAL))
			return LEPT_PARSE_NUMBER_TOO_BIG;
		v->set_type(LEPT_NUMBER) ;
		c->json_ = p;
		return LEPT_PARSE_OK;

	}

	//解析value
	int lept_parse_value(lept_context* c, lept_value* v) {
		switch (*c->json_) {
		case 't': return lept_parse_literal(c, v, "true", LEPT_TRUE);
		case 'f': return lept_parse_literal(c, v, "false", LEPT_FALSE);
		case 'n': return lept_parse_literal(c, v, "null", LEPT_NULL);
		default:  return lept_parse_number(c, v);
		case '\0':return LEPT_PARSE_EXPECT_VALUE;
		}
	}



}
//=====================================================
// lept_context
void lept_context::lept_parse_whitespace(){
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
//=====================================================
// lept_value
int lept_value::lept_parse(const char* json) {
	lept_context c;
	assert(this != nullptr);
	c.json_ = json;
	//若 lept_parse() 失败，会把 v 设为 null 类型
	//所以这里先把它设为 null
	//让 lept_parse_value() 写入解析出来的根值。
	type_ = LEPT_NULL;
	c.lept_parse_whitespace();
	int res=lept_parse_value(&c,this);
	if (res == LEPT_PARSE_OK) {
		c.lept_parse_whitespace();
		if (*c.json_ != '\0') {
			type_ = LEPT_NULL;
			res = LEPT_PARSE_ROOT_NOT_SINGULAR;
		}
	}
	return res;
}

lept_type lept_value::lept_get_type() {
	assert(this != nullptr);
	return type_;
}

//==========================================================================================================
//tutorial02
double lept::lept_value::lept_get_number(){
	assert(this != nullptr && type_ == LEPT_NUMBER);
	return n_;
}

//=======================================
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