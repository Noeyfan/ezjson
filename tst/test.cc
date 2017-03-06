#include "../src/ezjson.h"
#include "../src/any_stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

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

#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%f")

#define EXPECT_EQ_STRING(expect, actual) EXPECT_EQ_BASE(!strcmp(expect,actual), expect, actual, "%s")

#define TEST_ERROR(expect, actual)\
  do {\
      json_value v;\
      EXPECT_EQ_INT(ERROR, parse_json(&v, actual));\
  } while(0)

#define TEST_NUMBER(expect, actual)\
  do {\
      json_value v;\
      EXPECT_EQ_INT(OK, parse_json(&v, actual));\
      EXPECT_EQ_INT(JSON_NUMBER, get_json_type(&v));\
      EXPECT_EQ_DOUBLE(expect, v.number);\
  } while(0)

#include <iostream>
#define TEST_STRING(expect, actual)\
  do {\
      json_value v;\
      v.s.len = 0;\
      EXPECT_EQ_INT(OK, parse_json(&v, actual));\
      EXPECT_EQ_INT(JSON_STRING, get_json_type(&v));\
      EXPECT_EQ_STRING(expect, v.s.s);\
  } while(0)

static void test_any_array() {
    any_stack as;
    *as.push<char>() = 'a';
    *as.push<char>() = 'b';
    *as.push<char>() = 'c';
    as.pop<char>(1);
    *as.push<char>() = 'd';
    *as.push<char>() = '\0';
    as.pop<char>(4);
    EXPECT_EQ_STRING(as.bottom<char>(), "abd");
}

static void test_any_array_composite() {
    any_stack as;
    json_value v1;
    json_value v2;
    parse_json(&v1, "\"hello\"");
    parse_json(&v2, "\"world\"");
    *as.push<json_value>() = v1;
    *as.push<json_value>() = v2;
    EXPECT_EQ_STRING("hello", as.bottom<json_value>()->s.s);
    EXPECT_EQ_STRING("world", (as.bottom<json_value>() + 1)->s.s);
}

static void test_parse_null() {
    json_value v;
    v.type = JSON_BOOLEAN;
    EXPECT_EQ_INT(ERROR, parse_json(&v, "xxx null"));
    EXPECT_EQ_INT(OK, parse_json(&v, "null"));
    EXPECT_EQ_INT(OK, parse_json(&v, "   null"));
    EXPECT_EQ_INT(ERROR, parse_json(&v, "null  xxx"));
    EXPECT_EQ_INT(JSON_NULL, get_json_type(&v));
}

static void test_parse_bool() {
    json_value v;
    v.type = JSON_NULL;
    EXPECT_EQ_INT(OK, parse_json(&v, "true"));
    EXPECT_EQ_INT(JSON_BOOLEAN, get_json_type(&v));
    v.type = JSON_NULL;
    EXPECT_EQ_INT(OK, parse_json(&v, "false"));
    EXPECT_EQ_INT(JSON_BOOLEAN, get_json_type(&v));
}

static void test_parse_number() {
    TEST_NUMBER(123.0, "123.0");
    TEST_NUMBER(0.0, "0.0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_ERROR(0.1, "-.1");
    TEST_ERROR(0.1, ".1");
    TEST_ERROR(0.0, ".0");
    TEST_ERROR(123, "0123");
    TEST_ERROR(123, "+123");
}

static void test_parse_string() {
    TEST_STRING("hello", "\"hello\"");
    TEST_STRING("", "\"\"");
    TEST_STRING("hi", "\"hi\" ");
}

// naive testing
static void test_parse_array_simple() {
    json_value v;
    v.a.size = 0;
    int expect[3] = {1,2,3};
    EXPECT_EQ_INT(OK, parse_json(&v, "[1,2,3]"));
    EXPECT_EQ_INT(JSON_ARRAY, get_json_type(&v));
    EXPECT_EQ_INT(1, (int)(v.a.e)->number);
    EXPECT_EQ_INT(2, (int)(v.a.e + 1)->number);
    EXPECT_EQ_INT(3, (int)(v.a.e + 2)->number);
    EXPECT_EQ_INT((int)sizeof(expect)/(int)sizeof(int), (int)v.a.size);
}

static void test_parse_array_combine() {
    json_value v;
    EXPECT_EQ_INT(OK, parse_json(&v, "[[1,\"hello\"],\"world\",true,null]"));
    EXPECT_EQ_INT(JSON_ARRAY, get_json_type(&v));
    EXPECT_EQ_INT(4, (int)v.a.size);
    EXPECT_EQ_DOUBLE((double)1, (v.a.e->a.e)->number);
    EXPECT_EQ_STRING("hello", (v.a.e->a.e + 1)->s.s);
    EXPECT_EQ_INT(6, (int)(v.a.e->a.e + 1)->s.len);
    EXPECT_EQ_STRING("world", (v.a.e + 1)->s.s);
    EXPECT_EQ_INT(JSON_BOOLEAN, get_json_type(v.a.e + 2));
    EXPECT_EQ_INT(JSON_NULL, get_json_type(v.a.e + 3));
}

static void test_parse_object_simple() {
    json_value v;
    EXPECT_EQ_INT(OK, parse_json(&v, "{hello:1, world:\"yes\"}"));
    EXPECT_EQ_DOUBLE((double)1, v.o.m->v.number);
    EXPECT_EQ_STRING("hello", v.o.m->k);
    EXPECT_EQ_STRING("yes", (v.o.m+1)->v.s.s);
    EXPECT_EQ_STRING("world", (v.o.m+1)->k);
    EXPECT_EQ_INT(JSON_OBJECT, get_json_type(&v));
}

int main() {
    test_any_array();
    test_any_array_composite();
    test_parse_null();
    test_parse_bool();
    test_parse_number();
    test_parse_string();
    test_parse_array_simple();
    test_parse_array_combine();
    test_parse_object_simple();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}
