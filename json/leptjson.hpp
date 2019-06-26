#pragma once
/*		leptjson.hpp		*/

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
		LEPT_PARSE_NUMBER_TOO_BIG
	};
	//==============================
	//=== class
	//=== lept_value 
	class lept_value {
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
		void lept_set_string(const char*s, size_t len);
		void lept_free();
	public:

		//=====================================
		//=====================================
		int lept_parse(const char*json);
		lept_type lept_get_type();

		//=====================================
		//=====================================
		//tutorial02
		double lept_get_number();
		// 设置type_
		template <typename T>
		void set_type( T&& t) {
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
	class lept_context {
	public:
		const char* json_;

		// 解析空白
		void lept_parse_whitespace();
		// 解析第二部分的空白
		int lept_parse_whitespace_second();
	};
}