/*		leptjson.cpp		*/
#include "leptjson.h"
#include <assert.h>	/* assert() */
#include <stdlib.h>	/* NULL */
#include <utility>

using namespace lept;

#define EXPECT(c,ch) \
		do {\
			assert(*c->json_ == ch);\
			c->json_++;\
		} while (0)

//无名命名空间
//只能在文件内调用
//暂时只解析符号
namespace {
	//解析null
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
	//解析value
	int lept_parse_value(lept_context* c, lept_value* v) {
		switch (*c->json_) {
		case 'n':
			return lept_parse_null(c, v);
		case '\0':
			return LEPT_PARSE_EXPECT_VALUE;
		case 't':
			return lept_parse_true(c, v);
		case 'f':
			return lept_parse_false(c, v);
		default:
			return LEPT_PARSE_INVALID_VALUE;
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
	this->type_ = LEPT_NULL;
	c.lept_parse_whitespace();
	int res=lept_parse_value(&c,this);
	if (res == LEPT_PARSE_OK) {
		c.lept_parse_whitespace();
		if (*c.json_ != '\0') {
			res = LEPT_PARSE_ROOT_NOT_SINGULAR;
		}
	}
	return res;
}

lept_type lept_value::lept_get_type() {
	assert(this != nullptr);
	return type_;
}
