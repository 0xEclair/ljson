#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "leptjson.h"
#include <iostream>
using namespace lept;
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

static void test_parse_true() {
	lept_value v;
	v.set_type(LEPT_FALSE);
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.lept_parse("true"));
	EXPECT_EQ_INT(LEPT_TRUE, v.lept_get_type());
}

static void test_parse_false() {
	lept_value v;
	v.set_type(LEPT_FALSE);
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.lept_parse("false"));
	EXPECT_EQ_INT(LEPT_FALSE, v.lept_get_type());
}

static void test_parse_null() {
	lept_value v;
	v.set_type(LEPT_FALSE);
	EXPECT_EQ_INT(LEPT_PARSE_OK, v.lept_parse("null"));
	EXPECT_EQ_INT(LEPT_NULL, v.lept_get_type());
}

static void test_parse_expect_value() {
	lept_value v;

	v.set_type(LEPT_FALSE);
	EXPECT_EQ_INT(LEPT_PARSE_EXPECT_VALUE, v.lept_parse(""));
	EXPECT_EQ_INT(LEPT_NULL, v.lept_get_type());

	v.set_type (LEPT_FALSE);
	EXPECT_EQ_INT(LEPT_PARSE_EXPECT_VALUE, v.lept_parse(" "));
	EXPECT_EQ_INT(LEPT_NULL, v.lept_get_type());
}

static void test_parse_invalid_value() {
	lept_value v;
	v.set_type(LEPT_FALSE);
	EXPECT_EQ_INT(LEPT_PARSE_INVALID_VALUE, v.lept_parse("nul"));
	EXPECT_EQ_INT(LEPT_NULL, v.lept_get_type());

	v.set_type(LEPT_FALSE);
	EXPECT_EQ_INT(LEPT_PARSE_INVALID_VALUE, v.lept_parse("?"));
	EXPECT_EQ_INT(LEPT_NULL, v.lept_get_type());
}

static void test_parse_root_not_singular() {
	lept_value v;
	v.set_type (LEPT_FALSE);
	EXPECT_EQ_INT(LEPT_PARSE_ROOT_NOT_SINGULAR, v.lept_parse("null x"));
	EXPECT_EQ_INT(LEPT_NULL, v.lept_get_type());
}



static void test_parse() {
	test_parse_null();
	test_parse_true();
	test_parse_false();
	test_parse_expect_value();
	test_parse_invalid_value();
	test_parse_root_not_singular();
}


int main() {
	//lept_parse_null不能被调用
	//声明在namespace{}里面的只能在文件内被调用
	//lept_value v;
	//lept_context c;
	//lept_parse_null(c, v);
	test_parse();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);

	

	return main_ret;
}
