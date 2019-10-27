#pragma once
#ifdef _WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "ljson.h"
#include <iostream>
using namespace leptjson;
static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;
using namespace std;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")
#define EXPECT_EQ_STRING(expect, actual, alength) \
    EXPECT_EQ_BASE(sizeof(expect) - 1 == alength && memcmp(expect, actual, alength) == 0, expect, actual, "%s")
#define EXPECT_TRUE(actual) EXPECT_EQ_BASE((actual) != 0, "true", "false", "%s")
#define EXPECT_FALSE(actual) EXPECT_EQ_BASE((actual) == 0, "false", "true", "%s")

#if defined(_MSC_VER)
#define EXPECT_EQ_SIZE_T(expect, actual) EXPECT_EQ_BASE((expect) == (actual), (size_t)expect, (size_t)actual, "%Iu")
#else
#define EXPECT_EQ_SIZE_T(expect, actual) EXPECT_EQ_BASE((expect) == (actual), (size_t)expect, (size_t)actual, "%zu")
#endif

void test_parse_true() {
	lept_value v;
	v.lept_set_boolean(0);
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.lept_parse("true"));
	EXPECT_EQ_INT(LEPT_TRUE, v.lept_get_type());
	v.lept_free();
}

void test_parse_false() {
	lept_value v;
	v.lept_set_boolean(1);
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.lept_parse("false"));
	EXPECT_EQ_INT(LEPT_FALSE, v.lept_get_type());
	v.lept_free();
}

void test_parse_null() {
	lept_value v;
	v.lept_set_boolean(0);
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.lept_parse("null"));
	EXPECT_EQ_INT(LEPT_NULL, v.lept_get_type());
	v.lept_free();
}

//==========================================================================
//tutorial02
#define TEST_NUMBER(expect, json)\
    do {\
        lept_value v;\
        EXPECT_EQ_INT(LEPT_PARSE_OK, v.lept_parse(json));\
        EXPECT_EQ_INT(LEPT_NUMBER, v.lept_get_type());\
        EXPECT_EQ_DOUBLE(expect, v.lept_get_number());\
		v.lept_free();\
    } while(0)
void test_parse_number() {
	TEST_NUMBER(0.0, "0");
	TEST_NUMBER(0.0, "-0");
	TEST_NUMBER(0.0, "-0.0");
	TEST_NUMBER(1.0, "1");
	TEST_NUMBER(-1.0, "-1");
	TEST_NUMBER(1.5, "1.5");
	TEST_NUMBER(-1.5, "-1.5");
	TEST_NUMBER(3.1416, "3.1416");
	TEST_NUMBER(1E10, "1E10");
	TEST_NUMBER(1e10, "1e10");
	TEST_NUMBER(1E+10, "1E+10");
	TEST_NUMBER(1E-10, "1E-10");
	TEST_NUMBER(-1E10, "-1E10");
	TEST_NUMBER(-1e10, "-1e10");
	TEST_NUMBER(-1E+10, "-1E+10");
	TEST_NUMBER(-1E-10, "-1E-10");
	TEST_NUMBER(1.234E+10, "1.234E+10");
	TEST_NUMBER(1.234E-10, "1.234E-10");
	TEST_NUMBER(0.0, "1e-10000"); /* must underflow */
	TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
	TEST_NUMBER(4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
	TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
	TEST_NUMBER(2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
	TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
	TEST_NUMBER(2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
	TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
	TEST_NUMBER(1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
	TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}

#define TEST_STRING(expect, json)\
    do {\
        lept_value v;\
        v.set_type(LEPT_NULL);\
        EXPECT_EQ_INT(LEPT_PARSE_OK, v.lept_parse(json));\
        EXPECT_EQ_INT(LEPT_STRING, v.lept_get_type());\
        EXPECT_EQ_STRING(expect, v.lept_get_string(), v.lept_get_string_length());\
        v.lept_free();\
    } while(0)

static void test_parse_string() {
	TEST_STRING("", "\"\"");
	TEST_STRING("Hello", "\"Hello\"");
#if 1
	TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
	TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
#endif
}

#define TEST_ERROR(error, json)\
    do {\
        lept_value v;\
        v.set_type(LEPT_FALSE);\
        EXPECT_EQ_INT(error, v.lept_parse(json));\
        EXPECT_EQ_INT(LEPT_NULL, v.lept_get_type());\
		v.lept_free();\
    } while(0)

static void test_parse_expect_value() {
	TEST_ERROR(LEPT_PARSE_EXPECT_VALUE, "");
	TEST_ERROR(LEPT_PARSE_EXPECT_VALUE, " ");
}

static void test_parse_invalid_value() {
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "nul");
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "?");
#if 1
	/* invalid number */
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "+0");
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "+1");
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, ".123"); /* at least one digit before '.' */
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "1.");   /* at least one digit after '.' */
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "INF");
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "inf");
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "NAN");
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "nan");
#endif
}

static void test_parse_root_not_singular() {
	TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "null x");

#if 1
	/* invalid number */
	TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0123"); /* after zero should be '.' , 'E' , 'e' or nothing */
	TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0x0");
	TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0x123");
#endif
}

static void test_parse_number_too_big() {
#if 1
	TEST_ERROR(LEPT_PARSE_NUMBER_TOO_BIG, "1e309");
	TEST_ERROR(LEPT_PARSE_NUMBER_TOO_BIG, "-1e309");
#endif
}

void test_parse_missing_quotation_mark() {
	TEST_ERROR(LEPT_PARSE_MISS_QUOTATION_MARK, "\"");
	TEST_ERROR(LEPT_PARSE_MISS_QUOTATION_MARK, "\"abc");
}

static void test_parse_invalid_string_escape() {
#if 1
	TEST_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\v\"");
	TEST_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\'\"");
	TEST_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\0\"");
	TEST_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\x12\"");
#endif
}

static void test_parse_invalid_string_char() {
#if 1
	TEST_ERROR(LEPT_PARSE_INVALID_STRING_CHAR, "\"\x01\"");
	TEST_ERROR(LEPT_PARSE_INVALID_STRING_CHAR, "\"\x1F\"");
#endif
}


void test_access_null() {
	lept_value v;
	v.lept_set_string("a", 1);
	v.lept_set_null();
	EXPECT_EQ_INT(LEPT_NULL, v.lept_get_type());
	v.lept_free();
}

void test_access_boolean() {
	lept_value v;
	v.lept_set_string("a", 1);
	v.lept_set_boolean(1);
	EXPECT_TRUE(v.lept_get_boolean());
	v.lept_set_boolean(0);
	EXPECT_FALSE(v.lept_get_boolean());
	v.lept_free();
}

void test_access_number() {
	lept_value v;
	v.lept_set_string("a", 1);
	v.lept_set_number(1234.5);
	EXPECT_EQ_DOUBLE(1234.5, v.lept_get_number());
	v.lept_free();
}

void test_access_string() {
	lept_value v;
	v.lept_set_string( "", 0);
	EXPECT_EQ_STRING("", v.lept_get_string(), v.lept_get_string_length());
	v.lept_set_string( "Hello", 5);
	EXPECT_EQ_STRING("Hello", v.lept_get_string(), v.lept_get_string_length());
	v.lept_free();
}
void test_parse_invalid_unicode_hex() {
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u0\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u01\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u012\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u/000\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\uG000\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u0/00\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u0G00\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u00/0\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u00G0\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u000/\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u000G\"");
}


void test_parse_invalid_unicode_surrogate() {
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uDBFF\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\\\\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\uDBFF\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\uE000\"");
}

void test_parse_array() {
	size_t i, j;
	lept_value v;
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.lept_parse( "[ ]"));
	EXPECT_EQ_INT(LEPT_ARRAY, v.lept_get_type());
	EXPECT_EQ_SIZE_T(0, v.lept_get_array_size());
	v.lept_free();

	v.set_type(LEPT_NULL);
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.lept_parse("[ null , false , true , 123 , \"abc\" ]"));
	EXPECT_EQ_INT(LEPT_ARRAY, v.lept_get_type());
	EXPECT_EQ_SIZE_T(5, v.lept_get_array_size());
	EXPECT_EQ_INT(LEPT_NULL, v.lept_get_array_element(0)->lept_get_type());
	EXPECT_EQ_INT(LEPT_FALSE, v.lept_get_array_element(1)->lept_get_type());
	EXPECT_EQ_INT(LEPT_TRUE, v.lept_get_array_element(2)->lept_get_type());
	EXPECT_EQ_INT(LEPT_NUMBER, v.lept_get_array_element(3)->lept_get_type());
	EXPECT_EQ_INT(LEPT_STRING, v.lept_get_array_element(4)->lept_get_type());
	EXPECT_EQ_DOUBLE(123.0, v.lept_get_array_element(3)->lept_get_number());
	EXPECT_EQ_STRING("abc", v.lept_get_array_element(4)->lept_get_string(), v.lept_get_array_element(4)->lept_get_string_length());
	v.lept_free();

	v.set_type(LEPT_NULL);
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.lept_parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]"));
	EXPECT_EQ_INT(LEPT_ARRAY, v.lept_get_type());
	EXPECT_EQ_SIZE_T(4, v.lept_get_array_size());
	for (i = 0; i < 4; i++) {
		lept_value* a = v.lept_get_array_element(i);
		EXPECT_EQ_INT(LEPT_ARRAY, a->lept_get_type());
		EXPECT_EQ_SIZE_T(i, a->lept_get_array_size());
		for (j = 0; j < i; j++) {
			lept_value* e = a->lept_get_array_element( j);
			EXPECT_EQ_INT(LEPT_NUMBER,e->lept_get_type());
			EXPECT_EQ_DOUBLE((double)j, e->lept_get_number());
		}
	}
	v.lept_free();
}

void test_parse_object() {
	lept_value v;
	size_t i;

	EXPECT_EQ_INT(LEPT_PARSE_OK, v.lept_parse(" { } "));
	EXPECT_EQ_INT(LEPT_OBJECT, v.lept_get_type());
	EXPECT_EQ_SIZE_T(0, v.lept_get_object_size());
	v.lept_free();

	v.set_type(LEPT_NULL);
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.lept_parse(
		" { "
		"\"n\" : null , "
		"\"f\" : false , "
		"\"t\" : true , "
		"\"i\" : 123 , "
		"\"s\" : \"abc\", "
		"\"a\" : [ 1, 2, 3 ],"
		"\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
		" } "
	));
	EXPECT_EQ_INT(LEPT_OBJECT, v.lept_get_type());
	EXPECT_EQ_SIZE_T(7, v.lept_get_object_size());
	EXPECT_EQ_STRING("n", v.lept_get_object_key(0), v.lept_get_object_key_length(0));
	EXPECT_EQ_INT(LEPT_NULL, v.lept_get_object_value(0)->lept_get_type());
	EXPECT_EQ_STRING("f", v.lept_get_object_key(1), v.lept_get_object_key_length(1));
	EXPECT_EQ_INT(LEPT_FALSE, v.lept_get_object_value(1)->lept_get_type());
	EXPECT_EQ_STRING("t", v.lept_get_object_key(2), v.lept_get_object_key_length(2));
	EXPECT_EQ_INT(LEPT_TRUE, v.lept_get_object_value(2)->lept_get_type());
	EXPECT_EQ_STRING("i", v.lept_get_object_key(3), v.lept_get_object_key_length(3));
	EXPECT_EQ_INT(LEPT_NUMBER, v.lept_get_object_value(3)->lept_get_type());
	EXPECT_EQ_DOUBLE(123.0, v.lept_get_object_value(3)->lept_get_number());
	EXPECT_EQ_STRING("s", v.lept_get_object_key(4), v.lept_get_object_key_length(4));
	EXPECT_EQ_INT(LEPT_STRING, v.lept_get_object_value(4)->lept_get_type());
	EXPECT_EQ_STRING("abc", v.lept_get_object_value(4)->lept_get_string(), v.lept_get_object_value(4)->lept_get_string_length());
	EXPECT_EQ_STRING("a", v.lept_get_object_key(5), v.lept_get_object_key_length(5));
	EXPECT_EQ_INT(LEPT_ARRAY, v.lept_get_object_value(5)->lept_get_type());
	EXPECT_EQ_SIZE_T(3, v.lept_get_object_value(5)->lept_get_array_size());
	for (i = 0; i < 3; i++) {
		lept_value* e = v.lept_get_object_value(5)->lept_get_array_element(i);
		EXPECT_EQ_INT(LEPT_NUMBER, e->lept_get_type());
		EXPECT_EQ_DOUBLE(i + 1.0, e->lept_get_number());
	}
	EXPECT_EQ_STRING("o", v.lept_get_object_key(6), v.lept_get_object_key_length(6));
	{
		lept_value* o = v.lept_get_object_value(6);
		EXPECT_EQ_INT(LEPT_OBJECT, o->lept_get_type());
		for (i = 0; i < 3; i++) {
			lept_value* ov = o->lept_get_object_value(i);
			EXPECT_TRUE('1' + i == o->lept_get_object_key(i)[0]);
			EXPECT_EQ_SIZE_T(1, o->lept_get_object_key_length(i));
			EXPECT_EQ_INT(LEPT_NUMBER, ov->lept_get_type());
			EXPECT_EQ_DOUBLE(i + 1.0, ov->lept_get_number());
		}
	}
	v.lept_free();
}

void test_parse_miss_comma_or_square_bracket() {
#if 1
	TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1");
	TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1}");
	TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1 2");
	TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[[]");
#endif
}

void test_parse_miss_key() {
	TEST_ERROR(LEPT_PARSE_MISS_KEY, "{:1,");
	TEST_ERROR(LEPT_PARSE_MISS_KEY, "{1:1,");
	TEST_ERROR(LEPT_PARSE_MISS_KEY, "{true:1,");
	TEST_ERROR(LEPT_PARSE_MISS_KEY, "{false:1,");
	TEST_ERROR(LEPT_PARSE_MISS_KEY, "{null:1,");
	TEST_ERROR(LEPT_PARSE_MISS_KEY, "{[]:1,");
	TEST_ERROR(LEPT_PARSE_MISS_KEY, "{{}:1,");
	TEST_ERROR(LEPT_PARSE_MISS_KEY, "{\"a\":1,");
}

void test_parse_miss_colon() {
	TEST_ERROR(LEPT_PARSE_MISS_COLON, "{\"a\"}");
	TEST_ERROR(LEPT_PARSE_MISS_COLON, "{\"a\",\"b\"}");
}

void test_parse_miss_comma_or_curly_bracket() {
	TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1");
	TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1]");
	TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1 \"b\"");
	TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":{}");
}


#define TEST_ROUNDTRIP(json)\
    do {\
        lept_value v;\
        char* json2;\
        size_t length;\
        EXPECT_EQ_INT(LEPT_PARSE_OK, v.lept_parse(json));\
        json2 = v.lept_stringify(&length);\
        EXPECT_EQ_STRING(json, json2, length);\
        v.lept_free();\
        delete[] json2;\
    } while(0)

void test_stringify_number() {
	TEST_ROUNDTRIP("0");
	TEST_ROUNDTRIP("-0");
	TEST_ROUNDTRIP("1");
	TEST_ROUNDTRIP("-1");
	TEST_ROUNDTRIP("1.5");
	TEST_ROUNDTRIP("-1.5");
	TEST_ROUNDTRIP("3.25");
	TEST_ROUNDTRIP("1e+20");
	TEST_ROUNDTRIP("1.234e+20");
	TEST_ROUNDTRIP("1.234e-20");

	TEST_ROUNDTRIP("1.0000000000000002"); /* the smallest number > 1 */
	TEST_ROUNDTRIP("4.9406564584124654e-324"); /* minimum denormal */
	TEST_ROUNDTRIP("-4.9406564584124654e-324");
	TEST_ROUNDTRIP("2.2250738585072009e-308");  /* Max subnormal double */
	TEST_ROUNDTRIP("-2.2250738585072009e-308");
	TEST_ROUNDTRIP("2.2250738585072014e-308");  /* Min normal positive double */
	TEST_ROUNDTRIP("-2.2250738585072014e-308");
	TEST_ROUNDTRIP("1.7976931348623157e+308");  /* Max double */
	TEST_ROUNDTRIP("-1.7976931348623157e+308");
}

void test_stringify_string() {
	TEST_ROUNDTRIP("\"\"");
	TEST_ROUNDTRIP("\"Hello\"");
	TEST_ROUNDTRIP("\"Hello\\nWorld\"");
	TEST_ROUNDTRIP("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
	TEST_ROUNDTRIP("\"Hello\\u0000World\"");
}

void test_stringify_array() {
	TEST_ROUNDTRIP("[]");
	TEST_ROUNDTRIP("[null,false,true,123,\"abc\",[1,2,3]]");
}

void test_stringify_object() {
	TEST_ROUNDTRIP("{}");
	TEST_ROUNDTRIP("{\"n\":null,\"f\":false,\"t\":true,\"i\":123,\"s\":\"abc\",\"a\":[1,2,3],\"o\":{\"1\":1,\"2\":2,\"3\":3}}");
}

void test_stringify() {
	TEST_ROUNDTRIP("null");
	TEST_ROUNDTRIP("false");
	TEST_ROUNDTRIP("true");
	test_stringify_number();
	test_stringify_string();
	test_stringify_array();
	test_stringify_object();

	lept_value o, *v;
	size_t index;
	o.lept_parse("{\"name\":\"Milo\", \"gender\":\"C\"}");
	if ((v = o.lept_find_object_value("gender", 6))!=nullptr) {
		printf("%s\n", v->lept_get_string());
	}
	o.lept_free();
}

#define TEST_EQUAL(json1, json2, equality) \
    do {\
        lept_value v1, v2;\
        EXPECT_EQ_INT(LEPT_PARSE_OK, v1.lept_parse(json1));\
        EXPECT_EQ_INT(LEPT_PARSE_OK, v2.lept_parse(json2));\
        EXPECT_EQ_INT(equality, lept_is_equal(&v1, &v2));\
        v1.lept_free();\
        v2.lept_free();\
    } while(0)

static void test_equal() {
	TEST_EQUAL("true", "true", 1);
	TEST_EQUAL("true", "false", 0);
	TEST_EQUAL("false", "false", 1);
	TEST_EQUAL("null", "null", 1);
	TEST_EQUAL("null", "0", 0);
	TEST_EQUAL("123", "123", 1);
	TEST_EQUAL("123", "456", 0);
	TEST_EQUAL("\"abc\"", "\"abc\"", 1);
	TEST_EQUAL("\"abc\"", "\"abcd\"", 0);
	TEST_EQUAL("[]", "[]", 1);
	TEST_EQUAL("[]", "null", 0);
	TEST_EQUAL("[1,2,3]", "[1,2,3]", 1);
	TEST_EQUAL("[1,2,3]", "[1,2,3,4]", 0);
	TEST_EQUAL("[[]]", "[[]]", 1);
	TEST_EQUAL("{}", "{}", 1);
	TEST_EQUAL("{}", "null", 0);
	TEST_EQUAL("{}", "[]", 0);
	TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2}", 1);
	TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"b\":2,\"a\":1}", 1);
	TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":3}", 0);
	TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2,\"c\":3}", 0);
	TEST_EQUAL("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":{}}}}", 1);
	TEST_EQUAL("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":[]}}}", 0);
}

static void test_copy() {
	lept_value v1, v2;
	v1.lept_parse("{\"t\":true,\"f\":false,\"n\":null,\"d\":1.5,\"a\":[1,2,3]}");
	lept_copy(&v2, &v1);
	EXPECT_TRUE(lept_is_equal(&v2, &v1));
	v1.lept_free();
	v2.lept_free();
}

static void test_move() {
	lept_value v1, v2, v3;
	v1.lept_parse("{\"t\":true,\"f\":false,\"n\":null,\"d\":1.5,\"a\":[1,2,3]}");
	lept_copy(&v2, &v1);
	lept_move(&v3, &v2);
	EXPECT_EQ_INT(LEPT_NULL, v2.lept_get_type());
	EXPECT_TRUE(lept_is_equal(&v3, &v1));
	printf("1\n");
	v1.lept_free();
	v2.lept_free();
	v3.lept_free();
}

static void test_swap() {
	lept_value v1, v2;
	v1.lept_set_string("Hello", 5);
	v2.lept_set_string("World!", 6);
	lept_swap(&v1, &v2);
	EXPECT_EQ_STRING("World!", v1.lept_get_string(), v1.lept_get_string_length());
	EXPECT_EQ_STRING("Hello", v2.lept_get_string(), v2.lept_get_string_length());
	v1.lept_free();
	v2.lept_free();
}

void test_access_array() {
	lept_value a, e;
	size_t i, j;

	for (j = 0; j <= 5; j += 5) {
		a.lept_set_array(j);
		EXPECT_EQ_SIZE_T(0, a.lept_get_array_size());
		EXPECT_EQ_SIZE_T(j, a.lept_get_array_capacity());
		for (i = 0; i < 10; i++) {
			e.set_type(LEPT_NULL);
			e.lept_set_number(i);
			lept_move(a.lept_pushback_array_element(), &e);
			e.lept_free();
		}

		EXPECT_EQ_SIZE_T(10, a.lept_get_array_size());
		for (i = 0; i < 10; i++)
			EXPECT_EQ_DOUBLE((double)i, a.lept_get_array_element(i)->lept_get_number());
	}

	a.lept_popback_array_element();
	EXPECT_EQ_SIZE_T(9, a.lept_get_array_size());
	for (i = 0; i < 9; i++)
		EXPECT_EQ_DOUBLE((double)i, a.lept_get_array_element(i)->lept_get_number());

	a.lept_erase_array_element(4, 0);
	EXPECT_EQ_SIZE_T(9, a.lept_get_array_size());
	for (i = 0; i < 9; i++)
		EXPECT_EQ_DOUBLE((double)i, a.lept_get_array_element(i)->lept_get_number());

	a.lept_erase_array_element(8, 1);
	EXPECT_EQ_SIZE_T(8, a.lept_get_array_size());
	for (i = 0; i < 8; i++)
		EXPECT_EQ_DOUBLE((double)i, a.lept_get_array_element(i)->lept_get_number());

	a.lept_erase_array_element(0, 2);
	EXPECT_EQ_SIZE_T(6, a.lept_get_array_size());
	for (i = 0; i < 6; i++)
		EXPECT_EQ_DOUBLE((double)i + 2, a.lept_get_array_element(i)->lept_get_number());

#if 1
	for (i = 0; i < 2; i++) {
		e.set_type(LEPT_NULL);
		e.lept_set_number(i);
		lept_move(a.lept_insert_array_element(i), &e);
		e.lept_free();
	}

	EXPECT_EQ_SIZE_T(8, a.lept_get_array_size());
	for (i = 0; i < 8; i++)
		EXPECT_EQ_DOUBLE((double)i, a.lept_get_array_element(i)->lept_get_number());

	EXPECT_TRUE(a.lept_get_array_capacity() > 8);
	a.lept_shrink_array();
	EXPECT_EQ_SIZE_T(8, a.lept_get_array_capacity());
	EXPECT_EQ_SIZE_T(8, a.lept_get_array_size());
	for (i = 0; i < 8; i++)
		EXPECT_EQ_DOUBLE((double)i, a.lept_get_array_element(i)->lept_get_number());

	e.lept_set_string("Hello", 5);
	lept_move(a.lept_pushback_array_element(), &e);     /* Test if element is freed */
	e.lept_free();

	i = a.lept_get_array_capacity();
	a.lept_clear_array();
	EXPECT_EQ_SIZE_T(0, a.lept_get_array_size());
	EXPECT_EQ_SIZE_T(i, a.lept_get_array_capacity());   /* capacity remains unchanged */
	a.lept_shrink_array();
	EXPECT_EQ_SIZE_T(0, a.lept_get_array_capacity());

	a.lept_free();
#endif
}

void test_access_object() {
#if 1
	lept_value o, v, *pv;
	size_t i, j, index;

	for (j = 0; j <= 5; j += 5) {
		o.lept_set_object(j);
		EXPECT_EQ_SIZE_T(0, o.lept_get_object_size());
		EXPECT_EQ_SIZE_T(j, o.lept_get_object_capacity());
		for (i = 0; i < 10; i++) {
			char key[2] = "a";
			key[0] += i;
			v.set_type(LEPT_NULL);
			v.lept_set_number(i);
			lept_move(o.lept_set_object_value(key, 1), &v);
			v.lept_free();
		}
		EXPECT_EQ_SIZE_T(10, o.lept_get_object_size());
		for (i = 0; i < 10; i++) {
			char key[] = "a";
			key[0] += i;
			index = o.lept_find_object_index(key, 1);
			EXPECT_TRUE(index != (unsigned int)(-1));
			pv = o.lept_get_object_value(index);
			EXPECT_EQ_DOUBLE((double)i, pv->lept_get_number());
		}
	}

	index = o.lept_find_object_index("j", 1);
	EXPECT_TRUE(index != (unsigned int)(-1));
	o.lept_remove_object_value(index);
	index = o.lept_find_object_index("j", 1);
	EXPECT_TRUE(index == (unsigned int)(-1));
	EXPECT_EQ_SIZE_T(9, o.lept_get_object_size());

	index = o.lept_find_object_index("a", 1);
	EXPECT_TRUE(index != (unsigned int)(-1));
	o.lept_remove_object_value(index);
	index = o.lept_find_object_index("a", 1);
	EXPECT_TRUE(index == (unsigned int)(-1));
	EXPECT_EQ_SIZE_T(8, o.lept_get_object_size());

	EXPECT_TRUE(o.lept_get_object_capacity() > 8);
	o.lept_shrink_object();
	EXPECT_EQ_SIZE_T(8, o.lept_get_object_capacity());
	EXPECT_EQ_SIZE_T(8, o.lept_get_object_size());
	for (i = 0; i < 8; i++) {
		char key[] = "a";
		key[0] += i + 1;
		EXPECT_EQ_DOUBLE((double)i + 1, o.lept_get_object_value(o.lept_find_object_index(key, 1))->lept_get_number());
	}

	v.lept_set_string("Hello", 5);
	lept_move(o.lept_set_object_value("World", 5), &v); /* Test if element is freed */
	v.lept_free();

	pv = o.lept_find_object_value("World", 5);
	EXPECT_TRUE(pv != nullptr);
	EXPECT_EQ_STRING("Hello", pv->lept_get_string(), pv->lept_get_string_length());

	i = o.lept_get_object_capacity();
	o.lept_clear_object();
	EXPECT_EQ_SIZE_T(0, o.lept_get_object_size());
	EXPECT_EQ_SIZE_T(i, o.lept_get_object_capacity()); /* capacity remains unchanged */
	o.lept_shrink_object();
	EXPECT_EQ_SIZE_T(0, o.lept_get_object_capacity());

	o.lept_free();
#endif
}

static void test_parse() {
	test_parse_null();
	test_parse_true();
	test_parse_false();
	test_parse_number();
	test_parse_string();
	test_parse_array();
	test_parse_object();

	test_parse_expect_value();
	test_parse_invalid_value();
	test_parse_root_not_singular();
	test_parse_number_too_big();
	test_parse_missing_quotation_mark();
	test_parse_invalid_string_escape();
	test_parse_invalid_string_char();
	test_parse_invalid_unicode_hex();
	test_parse_invalid_unicode_surrogate();
	test_parse_miss_comma_or_square_bracket();
	test_parse_miss_key();
	test_parse_miss_colon();
	test_parse_miss_comma_or_curly_bracket();

	test_access_null();
	test_access_boolean();
	test_access_number();
	test_access_string();
	test_access_array();
	test_access_object();

	test_stringify();
	test_equal();
	test_copy();
	test_move();
	test_swap();

	const char* json = "{\"a\":[1,2],\"b\":3}";
	char *out;
	lept_value v;
	v.lept_parse(json);
	lept_copy(
		v.lept_find_object_value("b", 1),
		v.lept_find_object_value("a", 1));
	printf("%s\n", out = v.lept_stringify(nullptr)); /* {"a":[1,2],"b":[1,2]} */
	delete[] out;

	v.lept_parse(json);
	lept_move(
		v.lept_find_object_value("b", 1),
		v.lept_find_object_value("a", 1));
	printf("%s\n", out = v.lept_stringify(nullptr)); /* {"a":null,"b":[1,2]} */
	delete[] out;

	v.lept_parse(json);
	lept_swap(
		v.lept_find_object_value("b", 1),
		v.lept_find_object_value("a", 1));
	printf("%s\n", out = v.lept_stringify(nullptr)); /* {"a":3,"b":[1,2]} */
	delete[] out;

	v.lept_free();
}

int main(void) {
	//_CrtSetBreakAlloc(212);
#ifdef _WINDOWS
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	test_parse();

	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);

	std::cout << sizeof(lept_value) << std::endl;
	std::cout << sizeof(lept_context) << std::endl;
	std::cout << sizeof(lept_member) << std::endl;

	_CrtDumpMemoryLeaks();
	return main_ret;
}