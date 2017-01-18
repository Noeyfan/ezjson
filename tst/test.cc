#include "../src/ezjson.h"
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

int main() {
    test_parse_null();
    test_parse_bool();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}
