#pragma once
/*		leptjson.h		*/

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
		LEPT_PARSE_ROOT_NOT_SINGULAR
	};
	//==============================
	//=== class
	//=== lept_value 
	class lept_value {
		lept_type type_;

	public:
		//===========
		//tutorial01
		int lept_parse(const char*json);
		lept_type lept_get_type();

		// 设置type_
		template <typename T>
		void set_type( T&& t) {
			//左值 -> T& 右值 -> T&&
			//没有把引用作参数，所以不用转发
			type_ = t;
		}
	};


	//==============================
	//=== class
	//=== lept_context 
	class lept_context {
	public:
		const char* json_;

		// 解析空白
		void lept_parse_whitespace();
		// 解析第二部分的空白
		int lept_parse_whitespace_second();
	};

}


