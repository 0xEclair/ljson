/*		leptjson.cpp		*/
#include "leptjson.hpp"
#ifdef _WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
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
#define LEPT_PARSE_STACK_INTI_SIZE 512
#endif

constexpr size_t LEPT_PARSE_STRINGIFY_INIT_SIZE = 256;

#define PUTC(c,ch) \
	do{\
		*(char*)c->lept_context_push(sizeof(char))=(ch);\
	}while(0)

#define PUTS(c, s, len)     memcpy(c->lept_context_push(len), s, len)


#define STRING_ERROR(res) \
		do { \
			c->top_ = head;\
			return res;\
		} while(0)

#define OBJECT_ERROR(res) \
		do { \
			if (m.k != nullptr)	delete[] m.k;\
			m.k=nullptr;\
			c->top_ = head;\
			return res;\
		} while(0)

constexpr size_t LEPT_KEY_NOT_EXIST = -1;

//匿名命名空间
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


/*
	int lept_parse_string(lept_context* c, lept_value* v) {
		size_t head = c->top_, len;
		const char* p;
		unsigned u, u2;
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
				STRING_ERROR(LEPT_PARSE_MISS_QUOTATION_MARK);
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
					if (!(p = lept_parse_hex4(p, u))) {
						STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX);
					}
					if (u >= 0xD800 && u <= 0xDBFF) {
						if (*p++ != '\\')
							STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE);
						if (*p++ != 'u')
							STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE);
						if (!(p = lept_parse_hex4(p, u2))) {
							STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX);
						}
						if (u2 < 0xDC00 || u2>0xDFFF)
							STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE);
						u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
					}
					//++++ TODO surrogate handling ++++//
					c->lept_encode_utf8(u);
					break;
				default:
					STRING_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE);
				}
				break;
			default:
				if ((unsigned char)ch < 0x20) {
					STRING_ERROR(LEPT_PARSE_INVALID_STRING_CHAR);
				}
				PUTC(c, ch);
			}
		}
	}
*/

	//解析value
	int lept_parse_value(lept_context* c, lept_value* v) {
		switch (*c->json_) {
		case 't': return lept_parse_literal(c, v, "true", LEPT_TRUE);
		case 'f': return lept_parse_literal(c, v, "false", LEPT_FALSE);
		case 'n': return lept_parse_literal(c, v, "null", LEPT_NULL);
		default:  return lept_parse_number(c, v);
		case '"':  return lept_parse_string(c, v);
		case '[':return lept_parse_array(c, v);
		case '{':return lept_parse_object(c, v);
		case '\0':return LEPT_PARSE_EXPECT_VALUE;
		}
	}

	const char* lept_parse_hex4(const char* p, unsigned& u) {
		/*  p已经指向\uXXXX 中第一个X  */
		u = 0;
		for (int i = 0; i != 4; ++i) {
			char ch = *p++;
			u <<= 4;
			if (ch >= '0'&&ch <= '9')
				u |= ch - '0';
			else if (ch >= 'A'&&ch <= 'F')
				u |= ch - ('A' - 10);
			else if (ch >= 'a'&&ch <= 'f')
				u |= ch - ('a' - 10);
			else return nullptr;
		}
		return p;
	}

	int lept_parse_array(lept_context* c, lept_value* v) {
		size_t size = 0;
		EXPECT(c, '[');
		c->lept_parse_whitespace();
		if (*c->json_ == ']') {
			c->json_++;
			v->set_type(LEPT_ARRAY);
			v->set_size(0);
			v->lept_set_e() = nullptr;
			return LEPT_PARSE_OK;
		}
		int res;
		for (;;) {
			/* 相当于new一个lept_value */
			auto e=(lept_value*)c->lept_context_push(sizeof(lept_value));
			/*  类内使用{LEPT_NULL}初始化  */
			if ((res = lept_parse_value(c, e))!=LEPT_PARSE_OK) {
				return res;
			}
			//memcpy(c->lept_context_push(sizeof(lept_value)), &e, sizeof(lept_value));
			size++;
			c->lept_parse_whitespace();
			if (*c->json_ == ',') {
				c->json_++;
				c->lept_parse_whitespace();
			}
			else if (*c->json_ == ']') {
				c->json_++;
				v->set_type(LEPT_ARRAY);
				v->set_size(size);
				size *= sizeof(lept_value);
				memcpy((v->lept_set_e()= new lept_value[size/sizeof(lept_value)]), c->lept_context_pop(size), size);
				return LEPT_PARSE_OK;
			}
			else {
				c->lept_context_pop(sizeof(lept_value));
				return LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
			}
		}
	}

	int lept_parse_string(lept_context* c, lept_value* v) {
		int res;
		char* s;
		size_t len;
		if ((res = lept_parse_string_raw(c, &s, &len)) == LEPT_PARSE_OK)
			v->lept_set_string(s, len);
		return res;
	}

	int lept_parse_string_raw(lept_context* c, char** str, size_t* len) {
		size_t head = c->top_;
		const char* p;
		unsigned u, u2;
		EXPECT(c, '\"');
		p = c->json_;
		for (;;) {
			char ch = *p++;
			switch (ch) {
			case '\"':
				*len = c->top_ - head;
				c->json_ = p;
				*str=(char*)c->lept_context_pop(*len);
				return LEPT_PARSE_OK;
			case '\0':
				STRING_ERROR(LEPT_PARSE_MISS_QUOTATION_MARK);
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
					if (!(p = lept_parse_hex4(p, u))) {
						STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX);
					}
					if (u >= 0xD800 && u <= 0xDBFF) {
						if (*p++ != '\\')
							STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE);
						if (*p++ != 'u')
							STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE);
						if (!(p = lept_parse_hex4(p, u2))) {
							STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX);
						}
						if (u2 < 0xDC00 || u2>0xDFFF)
							STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE);
						u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
					}
					c->lept_encode_utf8(u);
					break;
				default:
					STRING_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE);
				}
				break;
			default:
				if ((unsigned char)ch < 0x20) {
					STRING_ERROR(LEPT_PARSE_INVALID_STRING_CHAR);
				}
				PUTC(c, ch);
			}
		}
		return 0;
	}

	int lept_parse_object(lept_context* c, lept_value* v) {
		size_t size,head=c->top_;
		lept_member m;
		int res;
		c->lept_parse_whitespace();
		EXPECT(c, '{');
		c->lept_parse_whitespace();
		if (*c->json_ == '}') {
			c->json_++;
			v->set_type(LEPT_OBJECT);
			v->lept_set_m() = nullptr;
			v->set_size(0);
			return LEPT_PARSE_OK;
		}
		m.k = nullptr;
		size = 0;
		for (;;) {
			m.v.set_type(LEPT_NULL);
			c->lept_parse_whitespace();
			if (*c->json_ != '\"') {
				OBJECT_ERROR(LEPT_PARSE_MISS_KEY);
			}
			/* parse k and klen */
			char* s;
			if (lept_parse_string_raw(c, &s, &m.klen) == LEPT_PARSE_OK) {
				m.k = new char[m.klen+1];
				memcpy(m.k, s, m.klen);
				m.k[m.klen] = '\0';
			}
			c->lept_parse_whitespace();
			if (*c->json_++ != ':') {
				OBJECT_ERROR(LEPT_PARSE_MISS_COLON);
			}
			c->lept_parse_whitespace();
			if ((res = lept_parse_value(c, &m.v)) != LEPT_PARSE_OK) {
				OBJECT_ERROR(res);
			}
			memcpy(c->lept_context_push(sizeof(lept_member)), &m, sizeof(lept_member));
			size++;
			c->lept_parse_whitespace();
			if (*c->json_ == ',') { 
				c->json_++;
				continue;
			}
			else if(*c->json_=='}'){
				c->json_++;
				v->set_type(LEPT_OBJECT);
				v->set_size(size);
				size *= sizeof(lept_member);
				memcpy(v->lept_set_m() = new lept_member[size / sizeof(lept_member)], c->lept_context_pop(size), size);
				return LEPT_PARSE_OK;
			}
			else {
				OBJECT_ERROR(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET);
			}
		}
	}

	void lept_stringify_value(lept_context* c, lept_value* v) {
		size_t i;
		switch (v->lept_get_type()) {
		case LEPT_NULL:   PUTS(c, "null", 4); break;
		case LEPT_FALSE:  PUTS(c, "false", 5); break;
		case LEPT_TRUE:   PUTS(c, "true", 4); break;
		case LEPT_NUMBER: c->top_ -= 32 - sprintf((char*)c->lept_context_push(32), "%.17g", v->lept_get_number()); break;
		case LEPT_STRING: lept_stringify_string(c, v->lept_get_string(), v->lept_get_string_length()); break;
		case LEPT_ARRAY:
			/* ... */
			PUTC(c, '[');
			for (i = 0; i < v->lept_get_array_size(); ++i) {
				if (i > 0)
					PUTC(c, ',');
				lept_stringify_value(c, v->lept_get_array_element(i));
			}
			PUTC(c, ']');
			break;
		case LEPT_OBJECT:
			/* ... */
			PUTC(c, '{');
			for (i = 0; i < v->lept_get_object_size(); ++i) {
				if (i > 0)
					PUTC(c, ',');
				lept_stringify_string(c, v->lept_get_object_key(i), v->lept_get_object_key_length(i));
				PUTC(c, ':');
				lept_stringify_value(c, v->lept_get_object_value(i));
			}
			PUTC(c, '}');
			break;
		default: assert(0 && "invalid type");
		}
	}

	void lept_stringify_string(lept_context* c, const char* s, size_t len) {
		constexpr char hex_digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
		size_t i, size;
		char* head, *p;
		assert(s != nullptr);
		p = head = (char*)c->lept_context_push(size = len * 6 + 2);
		*p++ = '"';
		for (i = 0; i < len; ++i) {
			uint8_t ch = (uint8_t)s[i];
			switch (ch) {
				case '\"': *p++ = '\\'; *p++ = '\"'; break;
				case '\\': *p++ = '\\'; *p++ = '\\'; break;
				case '\b': *p++ = '\\'; *p++ = 'b';  break;
				case '\f': *p++ = '\\'; *p++ = 'f';  break;
				case '\n': *p++ = '\\'; *p++ = 'n';  break;
				case '\r': *p++ = '\\'; *p++ = 'r';  break;
				case '\t': *p++ = '\\'; *p++ = 't';  break;
				default:
					if (ch < 0x20) {
						*p++ = '\\'; *p++ = 'u'; *p++ = '0'; *p++ = '0';
						*p++ = hex_digits[ch >> 4];
						*p++ = hex_digits[ch & 15];
					}
					else
						*p++ = s[i];
			}
		}
		*p++ = '"';
		c->top_ -= size - (p - head);
	}
}


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
			memcpy(stack_, tmp, top_);
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

//==========================================================================================================
//tutorial04
void lept_context::lept_encode_utf8(const unsigned& u) {
	if (u <= 0x7F)
		PUTC(this, u & 0xFF);
	else if (u <= 0x7FF) {
		PUTC(this, 0xC0 | ((u >> 6) & 0xFF));
		PUTC(this, 0x80 | ( u		& 0x3F));
	}
	else if (u <= 0xFFFF) {
		PUTC(this, 0xE0 | ((u >> 12) & 0xFF));
		PUTC(this, 0x80 | ((u >> 6)  & 0x3F));
		PUTC(this, 0x80 | ( u		 & 0x3F));
	}
	else {
		assert(u <= 0x10FFFF);
		PUTC(this, 0xF0 | ((u >> 18) & 0xFF));
		PUTC(this, 0x80 | ((u >> 12) & 0x3F));
		PUTC(this, 0x80 | ((u >> 6)  & 0x3F));
		PUTC(this, 0x80 | ( u		 & 0x3F));
	}
}

//=====================================================
// lept_value
int lept_value::lept_parse(const char* json) {
	lept_context c;
	assert(this != nullptr);
	lept_free();
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

lept_type lept_value::lept_get_type() const{
	assert(this != nullptr);
	return type_;
}

//==========================================================================================================
//tutorial02
double leptjson::lept_value::lept_get_number() const{
	assert(this != nullptr && type_ == LEPT_NUMBER);
	return n_;
}

//==========================================================================================================
//tutorial03
void lept_value::lept_free() {
	assert(this != nullptr);
	if (type_ == LEPT_STRING) {
		delete[] s_;
	}
	else if (type_ == LEPT_ARRAY) {
		for (size_t i = 0; i < size_;++i) {
			lept_get_array_element(i)->lept_free();
		}
		delete[] e_;
	}
	else if (type_ == LEPT_OBJECT) {
		for (size_t i = 0; i < size_; ++i) {
			delete[] lept_get_object_key(i);
			lept_get_object_value(i)->lept_free();
		}
		delete[] m_;
	}
	type_ = LEPT_NULL;
}

void lept_value::lept_set_string(const char* s, size_t len) {
	assert(this != nullptr && (s != nullptr || len == 0));
	lept_free();
	s_ = new char[len + 1];
	memcpy(s_, s, len);
	s_[len] = '\0';
	len_ = len;
	set_type(LEPT_STRING);
}

int lept_value::lept_get_boolean()const {
	assert(this!= NULL && (type_ == LEPT_TRUE || type_ == LEPT_FALSE));		
	return type_==LEPT_TRUE;
}

void lept_value::lept_set_boolean(int b) {
	lept_free();
	set_type((b ? LEPT_TRUE : LEPT_FALSE));
}

lept_value* lept_value::lept_get_array_element(size_t index)const {
	assert(this != nullptr&& type_ == LEPT_ARRAY);
	assert(index < size_);
	return &e_[index];
}

//==========================================================================================================
//tutorial06
const char* lept_value::lept_get_object_key(size_t index) const {
	assert(this != nullptr && type_ == LEPT_OBJECT);
	assert(index < size_);
	return m_[index].k;
}

char*& lept_value::lept_set_object_key(size_t index) {
	assert(this != nullptr && type_ == LEPT_OBJECT);
	assert(index < size_);
	return m_[index].k;
}


size_t& lept_value::lept_get_object_key_length(size_t index)  {
	assert(this != nullptr && type_ == LEPT_OBJECT);
	assert(index < size_);
	return m_[index].klen;
}

size_t lept_value::lept_get_object_key_length(size_t index)const {
	assert(this != nullptr && type_ == LEPT_OBJECT);
	assert(index < size_);
	return m_[index].klen;
}

lept_value* lept_value::lept_get_object_value(size_t index) const{
	assert(this != nullptr && type_ == LEPT_OBJECT);
	assert(index < size_);
	return &m_[index].v;
}

//==========================================================================================================
//tutorial07
char* lept_value::lept_stringify(size_t* length) {
	lept_context c;
	assert(this != nullptr);
	c.stack_ = new char[LEPT_PARSE_STRINGIFY_INIT_SIZE];
	c.top_ = 0;
	lept_stringify_value(&c, this);
	if (length)*length = c.top_;
	*(char*)c.lept_context_push(sizeof(char)) = ('\0');
	return c.stack_;
}

//==========================================================================================================
//tutorial08
size_t lept_value::lept_find_object_index(const char* _key, size_t _klen)const {
	size_t i;
	assert(this != nullptr && type_ == LEPT_OBJECT && _key != nullptr);
	for (i = 0; i < size_; ++i)
		if (m_[i].klen == _klen && memcmp(m_[i].k, _key, _klen) == 0)
			return i;
	return LEPT_KEY_NOT_EXIST;
}

lept_value* lept_value::lept_find_object_value(const char* _key, size_t _klen)const {
	size_t index = lept_find_object_index(_key, _klen);
	return index != LEPT_KEY_NOT_EXIST ? &m_[index].v : nullptr;
}


void lept_value::lept_set_array(size_t _capacity) {
	assert(this != nullptr);
	lept_free();
	type_ = LEPT_ARRAY;
	size_ = 0;
	capacity_ = _capacity;
	e_ = _capacity > 0 ? (new lept_value[_capacity]) : nullptr;
}

void lept_value::lept_reserve_array(size_t _capacity) {
	assert(this != nullptr&& type_ == LEPT_ARRAY);
	if (capacity_ < _capacity) {
		/* realloc() */
		auto tmp = e_;
		e_ = new lept_value[_capacity];
		if (tmp != nullptr) {
			memcpy(e_, tmp, capacity_*sizeof(lept_value));
			delete[] tmp;
		}
		capacity_ = _capacity;
	}
}

void lept_value::lept_shrink_array() {
	assert(this != nullptr && type_ == LEPT_ARRAY);
	if (capacity_ > size_) {
		capacity_ = size_;
		auto tmp = e_;
		e_ = new lept_value[size_];
		if (tmp != nullptr) {
			memcpy(e_, tmp, size_*sizeof(lept_value));
			delete[] tmp;
		}
	}
}

lept_value* lept_value::lept_pushback_array_element() {
	assert(this != nullptr &&type_ == LEPT_ARRAY);
	if (size_ == capacity_)
		lept_reserve_array(capacity_ == 0 ? 1 : capacity_ * 2);
	e_[size_].set_type(LEPT_NULL);
	return &e_[size_++];
}

lept_value* lept_value::lept_insert_array_element(size_t index){
	assert(this != nullptr &&type_ == LEPT_ARRAY);
	if (size_++ == capacity_)
		lept_reserve_array(capacity_ == 0 ? 1 : capacity_ * 2);
	//0--> 
	memcpy(&e_[index + 1], &e_[index],(size_ - index)*sizeof(lept_value));
	e_[index].lept_free();
	return &e_[index];
}

void lept_value::lept_erase_array_element(size_t index, size_t count) {
	assert(this != nullptr &&type_ == LEPT_ARRAY && size_-index>=count);
	for (size_t i = 0; i < count; ++i) {
		e_[index + i].lept_free();
	}
	memcpy(&e_[index], &e_[index + count], (size_-index-count)*sizeof(lept_value));
	size_ -= count;
}

void lept_value::lept_clear_array() {
	assert(this != nullptr &&type_ == LEPT_ARRAY);
	for (size_t i = 0; i < size_; ++i) {
		e_[i].lept_free();
	}
	size_ = 0;
}

void lept_value::lept_set_object(size_t _capacity) {
	assert(this != nullptr);
	lept_free();
	type_ = LEPT_OBJECT;
	size_ = 0;
	capacity_ = _capacity;
	m_ = _capacity > 0 ? (new lept_member[_capacity]) : nullptr;
}

const size_t& lept_value::lept_get_object_capacity()const{
	assert(this != nullptr && type_ == LEPT_OBJECT);
	return capacity_;
}

void lept_value::lept_reserve_object(size_t _capacity){
	assert(this != nullptr && type_ == LEPT_OBJECT);
	if (capacity_ < _capacity) {
		auto tmp = m_;
		m_ = new lept_member[_capacity];
		if (tmp != nullptr) {
			memcpy(e_, tmp, capacity_ * sizeof(lept_member));
			delete[] tmp;
		}
		capacity_ = _capacity;
	}
}

void lept_value::lept_shrink_object(){
	assert(this!= nullptr&& type_ == LEPT_OBJECT);
	if (capacity_ > size_) {
		capacity_ = size_;
		auto tmp = m_;
		m_ = new lept_member[size_];
		if (tmp != nullptr) {
			memcpy(e_, tmp, size_ * sizeof(lept_member));
			delete[] tmp;
		}
	}
}

void lept_value::lept_clear_object(){
	assert(this != nullptr&& type_ == LEPT_OBJECT);
	for (size_t i = 0; i < size_; ++i) {
		delete[] m_[i].k;
		m_[i].klen = 0;
		m_[i].v.lept_free();
	}
	size_ = 0;
}

lept_value* lept_value::lept_set_object_value(const char * _key, size_t _klen){
	assert(this != nullptr&& type_ == LEPT_OBJECT);
	auto key = lept_find_object_index(_key, _klen);
	if (key == LEPT_KEY_NOT_EXIST) {
		if (size_ == capacity_) {
			lept_reserve_object(capacity_ == 0 ? 1 : capacity_ * 2);
		}
		m_[size_].k = new char[_klen + 1];
		memcpy(m_[size_].k, _key, _klen);
		m_[size_].k[_klen] = '\0';
		m_[size_].klen = _klen;
		return &m_[size_++].v;
	}
	else {
		m_[key].v.lept_free();
		return &m_[key].v;
	}
}

void lept_value::lept_remove_object_value(size_t index){
	assert(this != nullptr&& type_ == LEPT_OBJECT);
	assert(index < size_);
	delete[] m_[index].k;
	m_[index].klen = 0;
	m_[index].v.lept_free();
	memcpy(&m_[index], &m_[index + 1], (--size_-index)*sizeof(lept_member));
	m_[size_].k = nullptr;
	m_[size_].klen = 0;
	m_[size_].v.set_type(LEPT_NULL);
}

//==========================================================================================================
//==========================================================================================================

int lept_is_equal(const lept_value* _lhs, const lept_value* _rhs) {
	assert(_lhs != nullptr && _rhs != nullptr);
	if (_lhs->lept_get_type() != _rhs->lept_get_type())
		return 0;
	size_t i;
	switch (_lhs->lept_get_type()) {
	case LEPT_STRING:
		return _lhs->lept_get_string_length() == _rhs->lept_get_string_length() && memcmp(_lhs->lept_get_string(), _rhs->lept_get_string(), _lhs->lept_get_string_length())==0;
	case LEPT_NUMBER:
		return _lhs->lept_get_number() == _rhs->lept_get_number();
	case LEPT_ARRAY:
		if (_lhs->lept_get_array_size() != _rhs->lept_get_array_size())
			return 0;
		for (i = 0; i < _lhs->lept_get_array_size(); ++i)
			if (!lept_is_equal(_lhs->lept_get_array_element(i), _rhs->lept_get_array_element(i)))
				return 0;
		return 1; 
	case LEPT_OBJECT:
		if (_lhs->lept_get_object_size() != _rhs->lept_get_object_size())
			return 0;
		for (i = 0; i < _lhs->lept_get_object_size(); ++i) {
			/* 如果没找到 返回 LEPT_KEY_NOT_EXIST */
			auto tmp = _lhs->lept_find_object_index(_rhs->lept_get_object_key(i), _rhs->lept_get_object_key_length(i));
			/* if lvalue equals rvalue */
			if (tmp == LEPT_KEY_NOT_EXIST)
				return 0;
			auto tmp2 = lept_is_equal(_lhs->lept_get_object_value(tmp), _rhs->lept_get_object_value(i));
			if (!tmp2)
				return 0;
		}
		return 1;
	default:
		return 1;
	}
}

void lept_copy(lept_value* _dst, lept_value* _src) {
	assert(_src != nullptr && _dst != nullptr && _src != _dst);
	size_t i;
	switch (_src->lept_get_type()) {
	case LEPT_STRING:
		_dst->lept_set_string(_src->lept_get_string(), _src->lept_get_string_length());
		break;
	case LEPT_ARRAY: {
		_dst->lept_free();
		_dst->set_type(LEPT_ARRAY);
		auto size = _src->lept_get_object_size();
		_dst->lept_set_e() = new lept_value[size];
		_dst->set_size(size);
		for (i = 0; i < size; ++i) {
			auto len = _src->lept_get_array_size();
			lept_copy(_dst->lept_get_array_element(i), _src->lept_get_array_element(i));
		}
		break; 
	}
	case LEPT_OBJECT:{
		_dst->lept_free();
		_dst->set_type(LEPT_OBJECT);
		auto size = _src->lept_get_object_size();
		_dst->lept_set_m() = new lept_member[size];
		_dst->set_size(size);
		//memcpy(_dst->lept_get_m(), _src->lept_get_m(), i * sizeof(lept_member));
		for (i = 0; i < size; ++i) {
			auto len = _src->lept_get_object_key_length(i);
			_dst->lept_set_object_key(i) = new char[len+1];
			memcpy(_dst->lept_set_object_key(i), _src->lept_get_object_key(i), len);
			_dst->lept_set_object_key(i)[len] = '\0';
			_dst->lept_get_object_key_length(i) = _src->lept_get_object_key_length(i);
			lept_copy(_dst->lept_get_object_value(i), _src->lept_get_object_value(i));
		}
		break;
	}
	default:
		_dst->lept_free();
		auto tmp = _src->lept_get_type();
		_dst->set_type(tmp);
		memcpy(_dst, _src, sizeof(lept_value));
		break;
	}
}

void lept_move(lept_value* _dst, lept_value* _src) {
	assert(_dst != nullptr && _src != nullptr && _src != _dst);
	_dst->lept_free();
	*_dst = std::move(*_src);
	_src->set_type(LEPT_NULL);
}

void lept_swap(lept_value* _lhs, lept_value* _rhs) {
	assert(_lhs != nullptr && _rhs != nullptr);
	if (_lhs != _rhs) {
		auto temp=std::move(*_rhs);
		//memcpy(&temp, _lhs, sizeof(lept_value));
		//memcpy(_lhs, _rhs, sizeof(lept_value));
		//memcpy(_rhs, &temp, sizeof(lept_value));
		*_rhs = std::move(*_lhs);
		*_lhs = std::move(temp);
	}
}